#include "lantern.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string>
#include <unordered_map>
#include <android/log.h>

#include "bytehook.h"
#include "xdl.h"

#define XDL_DEFAULT           0x00

#define LOG_TAG "lantern"

#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)

/*
* ===================================================================================================================================================================
* API-LEVEL   VERSION   FT_Open_Face   FT_New_Size   hb_blob_create   hb_font_reference   hb_font_get_face   remark
* ===================================================================================================================================================================
* 21          5.0       libskia.so     libskia.so
* 22          5.1       libskia.so     libskia.so
* 23          6.0       libskia.so     libskia.so
* -------------------------------------------------------------------------------------------------------------------------------------------------------------------
* 24          7.0       libskia.so     libskia.so    libminikin.so    libminikin.so
* 25          7.1       libskia.so     libskia.so    libminikin.so    libminikin.so
* 26          8.0       libskia.so     libskia.so    libminikin.so    libminikin.so
* 27          8.1       libskia.so     libskia.so    libminikin.so    libminikin.so
* -------------------------------------------------------------------------------------------------------------------------------------------------------------------
* 28          9         libhwui.so     libskia.so    libminikin.so                        libminikin.so
* 29          10        libhwui.so     libskia.so    libminikin.so                        libminikin.so
* 30          11        libhwui.so     libskia.so    libminikin.so                        libminikin.so
* ===================================================================================================================================================================
*/

// libtf2.so
using FT_New_Memory_Face_t = int (*)(void* library, const char* file_base, int file_size, int face_index, void** aface);
static FT_New_Memory_Face_t FT_New_Memory_Face_ = nullptr;

using FT_Done_Face_t = int (*)(void* face);
static FT_Done_Face_t FT_Done_Face_ = nullptr;

// libharfbuzz_ng.so
using hb_destroy_func_t = void (*)(void*);

using hb_blob_destroy_t = void (*)(void*);
static hb_blob_destroy_t hb_blob_destroy_ = nullptr;

using hb_face_reference_blob_t = void* (*)(void*);
static hb_face_reference_blob_t hb_face_reference_blob_ = nullptr;

using hb_blob_create_t = void* (*)(const char*, unsigned int, unsigned int, void*, hb_destroy_func_t);
static hb_blob_create_t hb_blob_create_ = nullptr;

using hb_font_get_face_t = void* (*)(void*);
static hb_font_get_face_t hb_font_get_face_ = nullptr;

using hb_blob_set_user_data_t = int (*)(void*, void*, void*, hb_destroy_func_t, int);
static hb_blob_set_user_data_t hb_blob_set_user_data_ = nullptr;

using hb_blob_get_user_data_t = void* (*)(void*, void*);
static hb_blob_get_user_data_t hb_blob_get_user_data_ = nullptr;

using hb_ot_font_set_funcs_t = void (*)(void*);
static hb_ot_font_set_funcs_t hb_ot_font_set_funcs_ = nullptr;

using hb_font_set_user_data_t = int (*)(void*, void*, void*, hb_destroy_func_t, int);
static hb_font_set_user_data_t hb_font_set_user_data_ = nullptr;

using hb_font_get_user_data_t = void* (*)(void*, void*);
static hb_font_get_user_data_t hb_font_get_user_data_ = nullptr;

using hb_font_get_parent_t = void* (*)(void*);
static hb_font_get_parent_t hb_font_get_parent_ = nullptr;

using hb_face_create_t = void* (*)(void*, unsigned int);
static hb_face_create_t hb_face_create_ = nullptr;

using hb_face_get_index_t = unsigned int (*)(const void*);
static hb_face_get_index_t hb_face_get_index_ = nullptr;

using hb_font_set_face_t = void (*)(void*, void*);
static hb_font_set_face_t hb_font_set_face_ = nullptr;

using hb_face_destroy_t = void (*)(void*);
static hb_face_destroy_t hb_face_destroy_ = nullptr;

using hb_face_make_immutable_t = void (*)(void* face);
static hb_face_make_immutable_t hb_face_make_immutable_ = nullptr;

using hb_face_reference_t = void* (*)(void* face);
static hb_face_reference_t hb_face_reference_ = nullptr;

using hb_face_get_upem_t = unsigned int (*)(void* face);
static hb_face_get_upem_t hb_face_get_upem_ = nullptr;

using hb_font_set_scale_t = void (*)(void* font, int x_scale, int y_scale);
static hb_font_set_scale_t hb_font_set_scale_ = nullptr;

static int sdk_ver_ = 0;

static std::unordered_map<uintptr_t, std::pair<uintptr_t, std::string>> fontsMaps_;

static int get_blob_data_offset()
{
    int offset = 0;

    switch (sdk_ver_)
    {
        case __ANDROID_API_R__: // 11
        case __ANDROID_API_Q__: // 10
            offset = 0xc;
            break;

        case __ANDROID_API_P__: // 9
            offset = 0x24;
            break;

        case __ANDROID_API_O_MR1__: // 8.1
        case __ANDROID_API_O__: // 8.0
        case __ANDROID_API_N_MR1__: // 7.1
        case __ANDROID_API_N__: // 7.0
        default:
            offset = 0x30;
            break;
    }

    return offset;
}

using MapsEntryCallback = std::function<bool(uintptr_t, uintptr_t, char[4], const char*, void*)>;

static bool IterateMaps(const MapsEntryCallback& cb, void* args = nullptr)
{
    FILE* fp = nullptr;
    char line[PATH_MAX] = {};

    if ((fp = ::fopen("/proc/self/maps", "r")) == nullptr)
    {
        return false;
    }

    while (::fgets(line, sizeof(line), fp) != nullptr)
    {
        uintptr_t start = 0;
        uintptr_t end = 0;
        char perm[4] = {};
        int pathnamePos = 0;

        if (::sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4c %*x %*x:%*x %*d%n",
                     &start, &end, perm, &pathnamePos) != 3)
        {
            continue;
        }

        if (pathnamePos <= 0)
        {
            continue;
        }

        while (::isspace(line[pathnamePos]) && pathnamePos <= static_cast<int>(sizeof(line) - 1))
        {
            ++pathnamePos;
        }

        if (pathnamePos > static_cast<int>(sizeof(line) - 1))
        {
            continue;
        }

        size_t pathLen = ::strlen(line + pathnamePos);

        if (pathLen == 0 || pathLen > static_cast<int>(sizeof(line) - 1))
        {
            continue;
        }

        char* pathname = line + pathnamePos;

        while (pathLen > 0 && pathname[pathLen - 1] == '\n')
        {
            pathname[pathLen - 1] = '\0';
            --pathLen;
        }

        if (cb(start, end, perm, pathname, args))
        {
            ::fclose(fp);
            return true;
        }
    }

    ::fclose(fp);
    return false;
}

static const char* find_self_maps(const char* base)
{
    const char* new_base = nullptr;

    if (base == nullptr)
    {
        return new_base;
    }

    auto&& mapIt = fontsMaps_.find(reinterpret_cast<uintptr_t>(base));

    if (mapIt != fontsMaps_.end())
    {
        if (mapIt->second.first == 0)
        {
            uintptr_t newAddr = 0;

            for (auto&& it : fontsMaps_)
            {
                if (it.second.first != 0 && it.second.second == mapIt->second.second)
                {
                    newAddr = it.second.first;
                    break;
                }
            }

            if (newAddr == 0)
            {
                int fd = open(mapIt->second.second.c_str(), O_RDONLY);

                if (fd == -1)
                {
                    return new_base;
                }

                struct stat st = {};

                if (fstat(fd, &st) != 0)
                {
                    close(fd);
                    return new_base;
                }

                LOG("find_self_maps file:%s size:%zu", mapIt->second.second.c_str(), static_cast<size_t>(st.st_size));
                void* addr = mmap(nullptr, static_cast<size_t>(st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);

                if (addr == nullptr)
                {
                    close(fd);
                    return new_base;
                }

                close(fd);

                LOG("find_self_maps insert new maps old:%p new:%p", base, addr);

                mapIt->second.first = reinterpret_cast<uintptr_t>(addr);
            }
            else
            {
                mapIt->second.first = newAddr;
            }
        }

        LOG("find_self_maps get maps old:%p new:%p", base, reinterpret_cast<void*>(mapIt->second.first));

        new_base = reinterpret_cast<const char*>(mapIt->second.first);
    }

    return new_base;
}

#define GET_OFFSET_PTR(data, offset, type) \
    reinterpret_cast<type>(reinterpret_cast<size_t>(data) + offset)
#define GET_OFFSET_DATA(data, offset, type) \
    *(GET_OFFSET_PTR(data, offset, type))

// num_charmaps offset --> face->num_charmaps --> FT_Set_Charmap
#define FT_FACE_NUM_CHARMAPS_OFFSET (0x24)
// num_charmaps offset --> face->charmaps --> FT_Set_Charmap
#define FT_FACE_CHARMAPS_OFFSET (0x28)
// num_charmaps offset --> face->charmap --> FT_Set_Charmap
#define FT_FACE_CHARMAP_OFFSET (0x5c)
// driver offset --> face->driver --> FT_New_Size
#define FT_FACE_DRIVER_OFFSET (0x60)
// stream offset --> face->driver + 8
#define FT_FACE_STREAM_OFFSET (FT_FACE_DRIVER_OFFSET + 8)
// extensions offset --> face->internal --> FT_Reference_Face
#define FT_FACE_INTERNAL_OFFSET (0x80)
// extensions offset --> face->internal - 4
#define FT_FACE_EXTERNSIONS_OFFSET (FT_FACE_INTERNAL_OFFSET - 4)
// ttface offset --> face->internal + 4
#define FT_FACE_TTFACE_OFFSET (FT_FACE_INTERNAL_OFFSET + 4)

#define SAFE_SWAP(left, right, type) \
    if (left != right) \
    { \
        type tmp = left; \
        left = right; \
        right = tmp; \
    }

#define OFFSET_PTR_SAFE_SWAP(left, right, offset, type, name) \
    type* left_##name##_ptr = GET_OFFSET_PTR(left, offset, type*); \
    type* right_##name##_ptr = GET_OFFSET_PTR(right, offset, type*); \
    LOG("FT_New_Size_proxy left_%s:%p right_%s:%p", #name, *left_##name##_ptr, #name, *right_##name##_ptr); \
    SAFE_SWAP(*left_##name##_ptr, *right_##name##_ptr, type);

#define FT_FACE_SET_EXTENSIONS(face) \
    void** extensions_ptr = GET_OFFSET_PTR(face, FT_FACE_EXTERNSIONS_OFFSET, void**); \
    *extensions_ptr = reinterpret_cast<void*>(find_self_maps);

int FT_New_Size_proxy(void* oldface, void** asize)
{
    LOG("FT_New_Size_proxy begin");

    void** extensions_ptr = GET_OFFSET_PTR(oldface, FT_FACE_EXTERNSIONS_OFFSET, void**);
    LOG("FT_New_Size_proxy extensions:%p", *extensions_ptr);

    if (*extensions_ptr == nullptr)
    {
        void* stream = GET_OFFSET_DATA(oldface, FT_FACE_STREAM_OFFSET, void**);
        // face->stream->base
        const char* stream_base = GET_OFFSET_DATA(stream, 0x00, const char**);
        LOG("FT_New_Size_proxy face:%p stream_base:%p", oldface, stream_base);

        const char* new_start = find_self_maps(stream_base);

        if (new_start != nullptr)
        {
            // face->face_index
            signed int face_index = GET_OFFSET_DATA(oldface, 0x04, signed int*);
            // face->stream->size
            unsigned int stream_size = GET_OFFSET_DATA(stream, 0x04, unsigned int*);
            // face->driver
            void* driver = GET_OFFSET_DATA(oldface, FT_FACE_DRIVER_OFFSET, void**);
            // face->driver->root->library offset
            void* library = GET_OFFSET_DATA(driver, 0x04, void**);

            // face->driver->clazz offset --> open_face
            void* face_clazz = GET_OFFSET_DATA(driver, 0x0c, void**);
            // face->driver->clazz->face_object_size offset --> open_face
            unsigned int face_size = GET_OFFSET_DATA(face_clazz, 0x24, unsigned int*);
            LOG("FT_New_Size_proxy face_size:%d", face_size);

            void* newface = nullptr;
            int error = FT_New_Memory_Face_(library, new_start, stream_size, face_index, &newface);
            LOG("FT_New_Size_proxy newface:%p error:%d", newface, error);

            signed int num_charmaps = GET_OFFSET_DATA(oldface, FT_FACE_NUM_CHARMAPS_OFFSET, signed int*);
            assert(num_charmaps == 4);
            // swap face->charmaps
            OFFSET_PTR_SAFE_SWAP(oldface, newface, FT_FACE_CHARMAPS_OFFSET, void**, charmaps);

            for (int i = 0; i < num_charmaps; ++i)
            {
                OFFSET_PTR_SAFE_SWAP((*left_charmaps_ptr)[i], (*right_charmaps_ptr)[i], 0x00, void*, charmaps_face);
            }

            // swap face->charmap
            OFFSET_PTR_SAFE_SWAP(oldface, newface, FT_FACE_CHARMAP_OFFSET, void*, charmap);
            // swap face->stream
            OFFSET_PTR_SAFE_SWAP(oldface, newface, FT_FACE_STREAM_OFFSET, void*, stream);
            // modify face->extensions
            FT_FACE_SET_EXTENSIONS(oldface);

            // swap ttface
            size_t ttface_offset = face_size - FT_FACE_TTFACE_OFFSET;
            assert(ttface_offset > 0);
            void* ttface = malloc(ttface_offset);
            memcpy(ttface, GET_OFFSET_PTR(oldface, FT_FACE_TTFACE_OFFSET, void*), ttface_offset);
            memcpy(GET_OFFSET_PTR(oldface, FT_FACE_TTFACE_OFFSET, void*), GET_OFFSET_PTR(newface, FT_FACE_TTFACE_OFFSET, void*), ttface_offset);
            memcpy(GET_OFFSET_PTR(newface, FT_FACE_TTFACE_OFFSET, void*), ttface, ttface_offset);
            free(ttface);

            FT_Done_Face_(newface);
        }
    }

    BYTEHOOK_STACK_SCOPE();
    int ret = BYTEHOOK_CALL_PREV(FT_New_Size_proxy, oldface, asize);

    LOG("FT_New_Size_proxy end");

    return ret;
}

#define FT_OPEN_MEMORY    0x1

static int FT_Open_Face_proxy(void* library, const void* args, int face_index, void** aface)
{
    LOG("FT_Open_Face_proxy begin");

    unsigned int flags = GET_OFFSET_DATA(args, 0, unsigned int*);

    if (flags == FT_OPEN_MEMORY)
    {
        const char** memory_base_ptr = GET_OFFSET_PTR(args, sizeof(unsigned int), const char**);
        LOG("FT_Open_Face_proxy memory_base:%p", *memory_base_ptr);

        const char* new_base = find_self_maps(*memory_base_ptr);

        if (new_base != nullptr)
        {
            *memory_base_ptr = new_base;
        }
    }

    BYTEHOOK_STACK_SCOPE();
    int ret = BYTEHOOK_CALL_PREV(FT_Open_Face_proxy, library, args, face_index, aface);
    // modify face->extensions
    FT_FACE_SET_EXTENSIONS(*aface);

    LOG("FT_Open_Face_proxy end");

    return ret;
}

#define HB_BLOB_SET_USER_DATA(blob) \
    hb_blob_set_user_data_(blob,    \
        reinterpret_cast<void*>(find_self_maps), \
        reinterpret_cast<void*>(find_self_maps), \
        nullptr,                    \
        1);

#define HB_BLOB_GET_USER_DATA(blob) \
    hb_blob_get_user_data_(blob, reinterpret_cast<void*>(find_self_maps));

#define HB_FONT_SET_USER_DATA(font) \
    hb_font_set_user_data_(font, \
        reinterpret_cast<void*>(find_self_maps), \
        reinterpret_cast<void*>(find_self_maps), \
        nullptr, \
        1);

#define HB_FONT_GET_USER_DATA(font) \
    hb_font_get_user_data_(font, reinterpret_cast<void*>(find_self_maps));

// face offset --> font->driver --> hb_font_get_face
#define HB_FONT_T_FACE_OFFSET 0x34 // 7.0 7.1 8.0 8.1

void hb_font_set_face_self(void* font, void* face)
{
    LOG("hb_font_set_face_self");
    void** old_face_ptr = GET_OFFSET_PTR(font, HB_FONT_T_FACE_OFFSET, void**);
    void* old_face = *old_face_ptr;

    hb_face_make_immutable_(face);
    *old_face_ptr = hb_face_reference_(face);

    hb_face_destroy_(old_face);
}

void hb_font_relpace_face(void* blob, void* face, void* font, void* parent_font)
{
    LOG("hb_font_relpace_face");
    void* new_face = hb_face_create_(blob, hb_face_get_index_(face));

    hb_font_set_face_(parent_font, new_face);
    hb_ot_font_set_funcs_(parent_font);

    unsigned int upem = hb_face_get_upem_(new_face);
    hb_font_set_scale_(parent_font, upem, upem);

    hb_font_set_face_(font, new_face);

    hb_face_destroy_(new_face);
}

static void hb_font_relpace_face(void* font)
{
    void* parent_font = hb_font_get_parent_(font);

    void* parent_font_user_data = HB_FONT_GET_USER_DATA(parent_font);

    LOG("hb_font_relpace_face parent_font_user_data:%p", parent_font_user_data);

    if (parent_font_user_data == nullptr)
    {
        void* face = hb_font_get_face_(font);

        void* blob = hb_face_reference_blob_(face);

        void* blob_user_data = HB_BLOB_GET_USER_DATA(blob);

        LOG("hb_font_relpace_face blob_user_data:%p", blob_user_data);

        if (blob_user_data != nullptr)
        {
            hb_font_relpace_face(blob, face, font, parent_font);

            HB_FONT_SET_USER_DATA(parent_font);
        }
        else
        {
            const char** blob_data_ptr = GET_OFFSET_PTR(blob, get_blob_data_offset(), const char**);
            LOG("hb_font_relpace_face blob_data:%p, blob:%p", *blob_data_ptr, blob);

            const char* new_base = find_self_maps(*blob_data_ptr);

            if (new_base != nullptr)
            {
                *blob_data_ptr = new_base;

                hb_font_relpace_face(blob, face, font, parent_font);

                HB_BLOB_SET_USER_DATA(blob);

                HB_FONT_SET_USER_DATA(parent_font);
            }
        }

        hb_blob_destroy_(blob);
    }
}

static void* hb_blob_create_proxy(const char* data, unsigned int length, int mode, void* user_data, hb_destroy_func_t destroy)
{
    LOG("hb_blob_create_proxy begin");

    const char* new_base = find_self_maps(data);

    if (new_base != nullptr)
    {
        data = new_base;
    }

    BYTEHOOK_STACK_SCOPE();
    void* ret = BYTEHOOK_CALL_PREV(hb_blob_create_proxy, data, length, mode, user_data, destroy);
    LOG("hb_blob_create_proxy blob_data:%p, blob:%p", data, ret);

    if (ret != nullptr && new_base != nullptr)
    {
        HB_BLOB_SET_USER_DATA(ret);
    }

    LOG("hb_blob_create_proxy end");

    return ret;
}

static void* hb_font_reference_proxy(void* font)
{
    LOG("hb_font_reference_proxy begin");

    hb_font_relpace_face(font);

    BYTEHOOK_STACK_SCOPE();
    void* ret = BYTEHOOK_CALL_PREV(hb_font_reference_proxy, font);

    LOG("hb_font_reference_proxy end");
    return ret;
}

static void* hb_font_get_face_proxy(void* parent)
{
    LOG("hb_font_get_face_proxy begin");

    hb_font_relpace_face(parent);

    BYTEHOOK_STACK_SCOPE();
    void* ret = BYTEHOOK_CALL_PREV(hb_font_get_face_proxy, parent);

    LOG("hb_font_get_face_proxy end");
    return ret;
}

#define LIBHARFBUZZ_NG "libharfbuzz_ng.so"
#define LIBFT2 "libft2.so"

#define GET_HANDLE(handle, filename, flags) \
    if (handle == nullptr) \
    { \
        handle = xdl_open("/system/lib/" filename, flags); \
        if (handle == nullptr) \
        { \
            LOG(#handle " == nullptr"); \
            return false; \
        } \
    }

#define CLOSE_HANDLE(handle) \
    if (handle != nullptr) \
    { \
        xdl_close(handle); \
        handle = nullptr; \
    }

#define GET_FUN(handle, name) \
    if (name##_ == nullptr) \
    { \
        name##_ = reinterpret_cast<name##_t>(xdl_sym(handle, #name, nullptr)); \
        if (name##_ == nullptr) \
        { \
            LOG(#name " == nullptr"); \
            return false; \
        } \
    }

static bool check_blob_data_offset()
{
    bool ret = false;

    const char* ptr = nullptr;
    void* blob = hb_blob_create_(ptr, sizeof(ptr), 1, nullptr, nullptr);
    const char** blob_data_ptr = GET_OFFSET_PTR(blob, get_blob_data_offset(), const char**);

    if (ptr == *blob_data_ptr)
    {
        LOG("get_blob_data_offset good");
        ret = true;
    }

    hb_blob_destroy_(blob);

    return ret;
}

#define CHECK_BLOB_DATA_OFFSET() \
    if (!check_blob_data_offset()) \
    { \
        LOG("check blob data offset fail"); \
        return false; \
    }

static void hooked_callback(bytehook_stub_t task_stub, int status_code, const char* caller_path_name, const char* sym_name, void* new_func, void* prev_func, void* arg)
{
    LOG("hooked_callback task_stub:%p, status_code:%d, caller_path_name:%s, sym_name:%s, new_func:%p, prev_func:%p, arg:%p", task_stub, status_code, caller_path_name, sym_name, new_func, prev_func, arg);
}


static void munmap_fonts_maps()
{
    IterateMaps([&](uintptr_t start, uintptr_t end, char perms[4], const char* path, void* args) -> bool
    {
        if (::strstr(path, "ttf") != nullptr || ::strstr(path, "otf") != nullptr || ::strstr(path, "ttc") != nullptr)
        {
            LOG("IterateMaps 0x%x-0x%x:%zu %s", start, end, end - start, path);

#if 0       // 使用hook FT_New_Size代替

            if (::strstr(path, "/system/fonts/Roboto-Regular.ttf") != nullptr)
            {
                return false;
            }

#endif
            fontsMaps_.insert({start, {0, path}});

            ::munmap(reinterpret_cast<void*>(start), end - start);
        }
        return false;
    });
}

#define HOOK_SINGLE(libname, symbol) \
    bytehook_hook_single(libname, nullptr, #symbol, reinterpret_cast<void*>(symbol##_proxy), hooked_callback, nullptr)

bool AndroidFontsExt_Install()
{
    sdk_ver_ = android_get_device_api_level();
    LOG("sdk_ver_ : %d", sdk_ver_);

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_R__)
    {
        bytehook_init(BYTEHOOK_MODE_AUTOMATIC, false);

        void* libft2_handle = nullptr;
        GET_HANDLE(libft2_handle, LIBFT2, XDL_DEFAULT);

        GET_FUN(libft2_handle, FT_Done_Face);
        GET_FUN(libft2_handle, FT_New_Memory_Face);

        CLOSE_HANDLE(libft2_handle); // 只是减少引用计数，并不会真正unload

        if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_O_MR1__)
        {
            HOOK_SINGLE("libskia.so", FT_Open_Face);
            HOOK_SINGLE("libskia.so", FT_New_Size);
        }
        else
        {
            HOOK_SINGLE("libhwui.so", FT_Open_Face);
            HOOK_SINGLE("libhwui.so", FT_New_Size);
        }

        if (sdk_ver_ >= __ANDROID_API_N__)
        {
            void* libharfbuzz_ng_handle = nullptr;
            GET_HANDLE(libharfbuzz_ng_handle, LIBHARFBUZZ_NG, XDL_DEFAULT);

            GET_FUN(libharfbuzz_ng_handle, hb_blob_create);
            GET_FUN(libharfbuzz_ng_handle, hb_blob_destroy);
            CHECK_BLOB_DATA_OFFSET();

            GET_FUN(libharfbuzz_ng_handle, hb_face_reference_blob);
            GET_FUN(libharfbuzz_ng_handle, hb_blob_set_user_data);
            GET_FUN(libharfbuzz_ng_handle, hb_blob_get_user_data);

            GET_FUN(libharfbuzz_ng_handle, hb_font_get_face);
            GET_FUN(libharfbuzz_ng_handle, hb_ot_font_set_funcs);
            GET_FUN(libharfbuzz_ng_handle, hb_font_get_parent);
            GET_FUN(libharfbuzz_ng_handle, hb_font_set_user_data);
            GET_FUN(libharfbuzz_ng_handle, hb_font_get_user_data);

            GET_FUN(libharfbuzz_ng_handle, hb_face_create);
            GET_FUN(libharfbuzz_ng_handle, hb_face_get_index);
            GET_FUN(libharfbuzz_ng_handle, hb_face_get_upem);
            GET_FUN(libharfbuzz_ng_handle, hb_font_set_scale);
            GET_FUN(libharfbuzz_ng_handle, hb_face_destroy);

            if (sdk_ver_ >= __ANDROID_API_P__)
            {
                GET_FUN(libharfbuzz_ng_handle, hb_font_set_face);
            }
            else
            {
                GET_FUN(libharfbuzz_ng_handle, hb_face_make_immutable);
                GET_FUN(libharfbuzz_ng_handle, hb_face_reference);

                hb_font_set_face_ = hb_font_set_face_self;
            }

            HOOK_SINGLE("libminikin.so", hb_blob_create);

            if (sdk_ver_ <= __ANDROID_API_O_MR1__)
            {
                HOOK_SINGLE("libminikin.so", hb_font_reference);
            }
            else
            {
                HOOK_SINGLE("libminikin.so", hb_font_get_face);
            }

            CLOSE_HANDLE(libharfbuzz_ng_handle); // 只是减少引用计数，并不会真正unload
        }

        munmap_fonts_maps();
    }

    return true;
}