﻿#include "lantern.h"


#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include "lantern_inner.spp"
#include "lantern_java.spp"
#include "lantern_offset.spp"
#include "lantern_sig.spp"
#include "lantern_plthook.spp"
#include "lantern_maps.spp"
#include "lantern_android.spp"
#include "lantern_xml.spp"
#include "lantern_log.spp"

static void munmap_fonts_maps(JNIEnv* __env, const std::unordered_map<void*, size_t>& __fontData, const std::unordered_set<size_t>& __black_lists)
{
#if 0

    if (0)
#else
    if (!__fontData.empty())
#endif
    {
        for (auto&& iter = fontsMaps_.begin(); iter != fontsMaps_.end(); /* ++iter */)
        {
            if (__fontData.find(reinterpret_cast<void*>(iter->first)) == __fontData.end())
            {
                LOG("munmap_fonts_maps 0x%x not in fontData", iter->first);
                iter = fontsMaps_.erase(iter);
                continue;
            }

            ++iter;
        }
    }
    else
    {
        char device[92] = {0};

        if (__system_property_get("ro.product.model", device) <= 0)
        {
            LOG("get ro.product.brand falied");
            return;
        }

        bool original_android = true;

        std::set<std::string> names;

        get_fontnames_from_xml(__env, names);
        LOG("names size %zu", names.size());

        std::unordered_map<std::string, uintptr_t> fontname2addr;

        for (auto iter = fontsMaps_.begin(); iter != fontsMaps_.end(); /* ++iter */)
        {
            if (names.count(iter->second.pathname) == 0)
            {
                LOG("munmap_fonts_maps %s not in xml", iter->second.pathname.c_str());
                iter = fontsMaps_.erase(iter);
                continue;
            }

            auto&& fontname2addrIter = fontname2addr.find(iter->second.pathname);

            if (fontname2addrIter != fontname2addr.end())
            {
                LOG("munmap_fonts_maps found multi font");
                fontsMaps_.erase(fontname2addrIter->second);
                iter = fontsMaps_.erase(iter);
                continue;
            }
            else
            {
                fontname2addr.insert({iter->second.pathname, iter->first});
            }

            ++iter;
        }
    }

    LOG("munmap_fonts_maps fontsMaps_.size:%zu", fontsMaps_.size());

    for (auto&& iter = fontsMaps_.begin(); iter != fontsMaps_.end(); /* ++iter */)
    {
        struct stat st = {};

        if (::stat(iter->second.pathname.c_str(), &st) != 0)
        {
            LOG("munmap_fonts_maps stat failed start:0x%x path:%s", iter->first, iter->second.pathname.c_str());

            iter = fontsMaps_.erase(iter);
            continue;
        }

        size_t file_size = static_cast<size_t>(st.st_size);

        if (__black_lists.count(file_size) != 0)
        {
            LOG("munmap_fonts_maps filesize hit start:0x%x size:%zu path:%s", iter->first, file_size, iter->second.pathname.c_str());

            iter = fontsMaps_.erase(iter);
            continue;
        }

        iter->second.file_size = file_size;
        ::munmap(reinterpret_cast<void*>(iter->first), iter->second.mmap_size);

        ++iter;
    }
}

bool get_font_data_from_fontfamily(std::unordered_map<void*, size_t>& __fontData,
                                   void* __fontFamily,
                                   GetSkTypeface_t __GetSkTypeface,
                                   openStream_t __openStream,
                                   getMemoryBase_t __getMemoryBase,
                                   getLength_t __getLength,
                                   getNumFonts_t __getNumFonts,
                                   getFont_t __getFont,
                                   GetFontData_t __GetFontData,
                                   GetFontSize_t __GetFontSize,
                                   size_t __font_vector_step,
                                   size_t __font_vector_offset,
                                   std::set<void*>& __fontFamily_cache,
                                   std::set<void*>& __font_cache)
{
    if (__fontFamily_cache.find(__fontFamily) != __fontFamily_cache.end())
    {
        return true;
    }

    __fontFamily_cache.insert(__fontFamily);

    void* font_vector_begin = GET_OFFSET_DATA(__fontFamily, __font_vector_offset, void**)NULLPTR_RETURN(font_vector_begin, false);
    void* font_vector_end = GET_OFFSET_DATA(__fontFamily, __font_vector_offset + 0x04, void**)NULLPTR_RETURN(font_vector_end, false);

    size_t fonts_length = (reinterpret_cast<size_t>(font_vector_end) - reinterpret_cast<size_t>(font_vector_begin))EXPRESSION_RETURN(fonts_length, <, 0, false);
    size_t fonts_remainder = (fonts_length % __font_vector_step)EXPRESSION_RETURN(fonts_remainder, !=, 0, false);
    size_t fonts_size = (fonts_length / __font_vector_step)EXPRESSION_RETURN(fonts_size, <, 0, false);

    if (__getNumFonts != nullptr)
    {
        size_t size = __getNumFonts(__fontFamily)EXPRESSION_RETURN(size, !=, fonts_size, false);
    }

    for (size_t j = 0; j < fonts_size; ++j)
    {
        void* font = GET_OFFSET_DATA(font_vector_begin, j * __font_vector_step, void**)NULLPTR_RETURN(font, false);

        if (__getFont != nullptr)
        {
            void* minikinfont = __getFont(__fontFamily, j)EXPRESSION_RETURN(minikinfont, !=, font, false);
        }

        if (__font_cache.find(font) != __font_cache.end())
        {
            continue;
        }

        __font_cache.insert(font);

        const void* memory_data = nullptr;
        size_t memory_size = 0;

        if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_M__)
        {
            void* typeface = __GetSkTypeface(font)NULLPTR_RETURN(typeface, false);
            void* stream = __openStream(typeface, nullptr)NULLPTR_RETURN(stream, false);
            memory_data = __getMemoryBase(stream)NULLPTR_RETURN(memory_data, false);
            memory_size = __getLength(stream)EXPRESSION_RETURN(memory_size, <=, 0, false);
        }
        else
        {
            memory_data = __GetFontData(font)NULLPTR_RETURN(memory_data, false);
            memory_size = __GetFontSize(font)EXPRESSION_RETURN(memory_size, <=, 0, false);
        }

        if (!check_maps(memory_data, memory_size, __fontData))
        {
            LOG("check_maps failed!");
            return false;
        }
    }

    return true;
}

bool get_font_data_from_fontfamily_vector(std::unordered_map<void*, size_t>& __fontData,
        void* __typeface_native,
        GetSkTypeface_t __GetSkTypeface,
        openStream_t __openStream,
        getMemoryBase_t __getMemoryBase,
        getLength_t __getLength,
        getNumFonts_t __getNumFonts,
        getFont_t __getFont,
        GetFontData_t __GetFontData,
        GetFontSize_t __GetFontSize,
        size_t __fontFamily_vector_step,
        size_t __font_vector_step,
        size_t __fontFamily_vector_offset,
        size_t __fontFamily_offset,
        size_t __font_vector_offset,
        std::set<void*>& __fontFamily_cache,
        std::set<void*>& __font_cache)
{
    void* fontCollection = GET_OFFSET_DATA(__typeface_native, 0x00, void**)NULLPTR_RETURN(fontCollection, false);

    void* fontFamily_vector_begin = GET_OFFSET_DATA(fontCollection, __fontFamily_vector_offset, void**)NULLPTR_RETURN(fontFamily_vector_begin, false);
    void* fontFamily_vector_end = GET_OFFSET_DATA(fontCollection, __fontFamily_vector_offset + 0x04, void**)NULLPTR_RETURN(fontFamily_vector_end, false);

    size_t fontFamily_length = (reinterpret_cast<size_t>(fontFamily_vector_end) - reinterpret_cast<size_t>(fontFamily_vector_begin))EXPRESSION_RETURN(fontFamily_length, <, 0, false);
    size_t fontFamily_remainder = (fontFamily_length % __fontFamily_vector_step)EXPRESSION_RETURN(fontFamily_remainder, !=, 0, false);
    size_t fontFamily_size = (fontFamily_length / __fontFamily_vector_step)EXPRESSION_RETURN(fontFamily_size, <, 0, false);

    for (size_t i = 0; i < fontFamily_size; ++i)
    {
        void* fontFamily = GET_OFFSET_DATA(fontFamily_vector_begin, i * __fontFamily_vector_step + __fontFamily_offset, void**)NULLPTR_RETURN(fontFamily, false);

        bool result = get_font_data_from_fontfamily(__fontData,
                      fontFamily,
                      __GetSkTypeface,
                      __openStream,
                      __getMemoryBase,
                      __getLength,
                      __getNumFonts,
                      __getFont,
                      __GetFontData,
                      __GetFontSize,
                      __font_vector_step,
                      __font_vector_offset,
                      __fontFamily_cache,
                      __font_cache)EXPRESSION_RETURN(result, ==, false, false);
    }

    return true;
}

bool get_font_data(JNIEnv* __env, std::unordered_map<void*, size_t>& __fontData)
{
    GetSkTypeface_t GetSkTypeface = nullptr;
    openStream_t openStream = nullptr;
    getMemoryBase_t getMemoryBase = nullptr;
    getLength_t getLength = nullptr;
    getNumFonts_t getNumFonts = nullptr;
    getFont_t getFont = nullptr;
    GetFontData_t GetFontData = nullptr;
    GetFontSize_t GetFontSize = nullptr;

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_M__)
    {
        void* libandroid_runtime = nullptr;
        GET_HANDLE(libandroid_runtime, XDL_DEFAULT);
        GET_CPLUSPLUS_FUN(libandroid_runtime, GetSkTypeface, "_ZNK7android15MinikinFontSkia13GetSkTypefaceEv");
        CLOSE_HANDLE(libandroid_runtime); // 只是减少引用计数，并不会真正unload

        void* libskia = nullptr;
        GET_HANDLE(libskia, XDL_DEFAULT);
        GET_CPLUSPLUS_FUN(libskia, openStream, "_ZNK10SkTypeface10openStreamEPi");
        GET_CPLUSPLUS_FUN(libskia, getMemoryBase, "_ZN14SkMemoryStream13getMemoryBaseEv");
        GET_CPLUSPLUS_FUN(libskia, getLength, "_ZNK14SkMemoryStream9getLengthEv");
        CLOSE_HANDLE(libskia); // 只是减少引用计数，并不会真正unload
    }
    else
    {
        void* libhwui = nullptr;
        GET_HANDLE(libhwui, XDL_DEFAULT);
        GET_CPLUSPLUS_FUN(libhwui, GetFontData, "_ZNK7android15MinikinFontSkia11GetFontDataEv");
        GET_CPLUSPLUS_FUN(libhwui, GetFontSize, "_ZNK7android15MinikinFontSkia11GetFontSizeEv");
        CLOSE_HANDLE(libhwui); // 只是减少引用计数，并不会真正unload
    }

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_N_MR1__)
    {
        void* libminikin = nullptr;
        GET_HANDLE(libminikin, XDL_DEFAULT);
        TRY_GET_CPLUSPLUS_FUN(libminikin, getNumFonts, "_ZNK7android10FontFamily11getNumFontsEv");
        TRY_GET_CPLUSPLUS_FUN(libminikin, getFont, "_ZNK7android10FontFamily7getFontEj");
        CLOSE_HANDLE(libminikin); // 只是减少引用计数，并不会真正unload
    }

#if DEBUGGABLE

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_O_MR1__)
    {
        // gMinikinLock _ZN7android12gMinikinLockE
    }

#endif

    size_t fontFamily_vector_step = get_fontFamily_vector_step()EXPRESSION_RETURN(fontFamily_vector_step, <=, 0, false);
    size_t font_vector_step = get_font_vector_step()EXPRESSION_RETURN(font_vector_step, <=, 0, false);
    size_t fontFamily_vector_offset = get_fontFamily_vector_offset()EXPRESSION_RETURN(fontFamily_vector_offset, <, 0, false);
    size_t fontFamily_offset = get_fontFamily_offset()EXPRESSION_RETURN(fontFamily_offset, <, 0, false);
    size_t font_vector_offset = get_font_vector_offset()EXPRESSION_RETURN(font_vector_offset, <, 0, false);

    std::set<void*> fontFamily_cache, font_cache;

    std::set<void*> fontFamily_native_ptrs;

#if SPECIAL_MACHINE_ANDROID

    if (::strcasestr(brand_.c_str(), "motorola"))
    {
        if (sdk_ver_ == __ANDROID_API_N__ || sdk_ver_ == __ANDROID_API_N_MR1__)
        {
            std::unordered_map<std::string, std::string> fontFamilyfields;

            get_fallbackFonts_for_motorola_from_java(__env, fontFamilyfields);

            if (!fontFamilyfields.empty())
            {
                get_fontFamily_native_ptrs_from_java(__env, fontFamilyfields, fontFamily_native_ptrs);
            }
        }
    }

#endif

    if (sdk_ver_ == __ANDROID_API_P__)
    {
        std::unordered_map<std::string, std::string> systemFallbackMapields;
        systemFallbackMapields.emplace("sSystemFallbackMap", "Ljava/util/Map;");
        get_fontFamily_native_ptrs_by_map_from_java(__env, systemFallbackMapields, fontFamily_native_ptrs);
    }

    bool get_font_data_from_fontfamily_result = !fontFamily_native_ptrs.empty();

    for (auto&& iter : fontFamily_native_ptrs)
    {
        void* fontFamily = iter;

        if (sdk_ver_ >= __ANDROID_API_O__)
        {
            fontFamily = GET_OFFSET_DATA(iter, 0x00, void**)NULLPTR_RETURN(fontFamily, true);
        }

        if (!get_font_data_from_fontfamily(__fontData,
                                           fontFamily,
                                           GetSkTypeface,
                                           openStream,
                                           getMemoryBase,
                                           getLength,
                                           getNumFonts,
                                           getFont,
                                           GetFontData,
                                           GetFontSize,
                                           font_vector_step,
                                           font_vector_offset,
                                           fontFamily_cache,
                                           font_cache))
        {
            get_font_data_from_fontfamily_result = false;
            break;
        }
    }

    std::set<void*> typeface_native_ptrs;

    if (sdk_ver_ != __ANDROID_API_P__ || !get_font_data_from_fontfamily_result)
    {
        std::unordered_map<std::string, std::string> systemFontMapFields;
        systemFontMapFields.emplace("sSystemFontMap", "Ljava/util/Map;");

#if SPECIAL_MACHINE_ANDROID

        if (::strcasestr(brand_.c_str(), "OnePlus"))
        {
            if (sdk_ver_ == __ANDROID_API_N_MR1__)
            {
                // systemFontMapFields.insert({"sOriginSystemFontMap", "Ljava/util/Map;"}); // 使用sSystemFontMap代替
                systemFontMapFields.emplace("sSlateSystemFontMap", "Ljava/util/Map;");
            }
        }
        else if (::strcasestr(brand_.c_str(), "OPPO"))
        {
            if (sdk_ver_ == __ANDROID_API_N__ || sdk_ver_ == __ANDROID_API_N_MR1__)
            {
                std::unordered_map<std::string, std::string> typefaceFields;
                typefaceFields.emplace("COLOROSUI_LIGHT", "Landroid/graphics/Typeface;");
                typefaceFields.emplace("COLOROSUI_THIN", "Landroid/graphics/Typeface;");
                typefaceFields.emplace("COLOROSUI_XLIGHT", "Landroid/graphics/Typeface;");
                typefaceFields.emplace("COLOROSUI_XTHIN", "Landroid/graphics/Typeface;");
                typefaceFields.emplace("DEFAULT_BOLD", "Landroid/graphics/Typeface;");

                get_typeface_native_ptrs_from_java(__env, typefaceFields, typeface_native_ptrs);
            }
        }

#endif

        get_typeface_native_ptrs_by_map_from_java(__env, systemFontMapFields, typeface_native_ptrs);
    }

    for (auto&& iter : typeface_native_ptrs)
    {
        bool result = get_font_data_from_fontfamily_vector(__fontData,
                      iter,
                      GetSkTypeface,
                      openStream,
                      getMemoryBase,
                      getLength,
                      getNumFonts,
                      getFont,
                      GetFontData,
                      GetFontSize,
                      fontFamily_vector_step,
                      font_vector_step,
                      fontFamily_vector_offset,
                      fontFamily_offset,
                      font_vector_offset,
                      fontFamily_cache,
                      font_cache)EXPRESSION_RETURN(result, ==, false, false);
    }

    return !__fontData.empty();
}

bool AndroidFontsExt_Install(JNIEnv* __env, jint __sdk_ver, jintArray __black_list)
{
    if (__sdk_ver < __ANDROID_API_L__ || __sdk_ver > __ANDROID_API_R__)
    {
        LOG("sdk_ver not match : %d", __sdk_ver);
        return false;
    }

    sdk_ver_ = __sdk_ver;
    LOG("sdk_ver_ : %d", sdk_ver_);

    char brand[92] = {0};

    if (__system_property_get("ro.product.brand", brand) <= 0)
    {
        LOG("get ro.product.brand falied");
        return false;
    }

    brand_ = brand;
    LOG("brand:%s", brand);

    found_ttf_maps([&](uintptr_t __start, uintptr_t __end, char __perms[4], const char* __path) -> bool
    {
        fontsMaps_.insert({__start, {__end - __start, __perms, __path}});
        return false;
    });

    std::unordered_map<void*, size_t> fontData;


#if DEBUGGABLE

    if (sdk_ver_ == __ANDROID_API_Q__ || sdk_ver_ == __ANDROID_API_R__)
    {
        get_font_data_from_java(__env, fontData);
    }

    if (fontData.empty())
    {
        if (!get_font_data(__env, fontData))
        {
            LOG("get_font_data failed!");
        }

    }

#else

    if (!signal_register(SIGSEGV))
    {
        LOG("signal handler reg failed.");
        return false;
    }

    signal_flag_ = 1;

    if (0 == sigsetjmp(time_machine_, 1))
    {
        if (!get_font_data(__env, fontData))
        {
            std::unordered_map<void*, size_t> ().swap(fontData);
            LOG("get_font_data failed!");
        }
    }
    else
    {
        std::unordered_map<void*, size_t> ().swap(fontData);
        LOG("native init failed, found exception signal.");
    }

    signal_flag_ = 0;

    if (!signal_unregister(SIGSEGV))
    {
        LOG("signal handler unreg failed.");
        return false;
    }

#endif

    LOG("fontData size:%zu", fontData.size());

    bytehook_init(BYTEHOOK_MODE_AUTOMATIC, false);

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_M__)
    {
        HOOK_PARTIAL(libskia_filter, mmap);
    }
    else
    {
        HOOK_SINGLE("libopenjdk.so", mmap64);
#if DEBUGGABLE
        HOOK_SINGLE("libopenjdk.so", munmap);
#endif
    }

    void* libft2 = nullptr;
    GET_HANDLE(libft2, XDL_DEFAULT);

    GET_C_FUN(libft2, FT_Done_Face);
    GET_C_FUN(libft2, FT_New_Memory_Face);

    CLOSE_HANDLE(libft2); // 只是减少引用计数，并不会真正unload

    if (sdk_ver_ >= __ANDROID_API_L__ && sdk_ver_ <= __ANDROID_API_O_MR1__)
    {
        HOOK_PARTIAL(libskia_filter, FT_Open_Face);
        HOOK_PARTIAL(libskia_filter, FT_New_Size);
    }
    else
    {
        HOOK_SINGLE("libhwui.so", FT_Open_Face);
        HOOK_SINGLE("libhwui.so", FT_New_Size);
    }

    if (sdk_ver_ >= __ANDROID_API_N__)
    {
        void* libharfbuzz_ng = nullptr;
        GET_HANDLE(libharfbuzz_ng, XDL_DEFAULT);

        GET_C_FUN(libharfbuzz_ng, hb_blob_create);
        GET_C_FUN(libharfbuzz_ng, hb_blob_destroy);
        CHECK_BLOB_DATA_OFFSET();

        GET_C_FUN(libharfbuzz_ng, hb_face_reference_blob);
        GET_C_FUN(libharfbuzz_ng, hb_blob_set_user_data);
        GET_C_FUN(libharfbuzz_ng, hb_blob_get_user_data);

        GET_C_FUN(libharfbuzz_ng, hb_font_get_face);
        GET_C_FUN(libharfbuzz_ng, hb_ot_font_set_funcs);
        GET_C_FUN(libharfbuzz_ng, hb_font_get_parent);
        GET_C_FUN(libharfbuzz_ng, hb_font_set_user_data);
        GET_C_FUN(libharfbuzz_ng, hb_font_get_user_data);

        GET_C_FUN(libharfbuzz_ng, hb_face_create);
        GET_C_FUN(libharfbuzz_ng, hb_face_get_index);
        GET_C_FUN(libharfbuzz_ng, hb_face_get_upem);
        GET_C_FUN(libharfbuzz_ng, hb_font_set_scale);
        GET_C_FUN(libharfbuzz_ng, hb_face_destroy);

        if (sdk_ver_ >= __ANDROID_API_P__)
        {
            GET_C_FUN(libharfbuzz_ng, hb_font_set_face);
        }
        else
        {
            GET_C_FUN(libharfbuzz_ng, hb_face_make_immutable);
            GET_C_FUN(libharfbuzz_ng, hb_face_reference);

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

        CLOSE_HANDLE(libharfbuzz_ng); // 只是减少引用计数，并不会真正unload
    }

    std::unordered_set<size_t> blacklist;

    get_blacklist_from_java(__env, __black_list, blacklist);

    munmap_fonts_maps(__env, fontData, blacklist);

    return true;
}