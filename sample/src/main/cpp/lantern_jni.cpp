#include <jni.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <android/log.h>
#include <fstream>

#include "lantern.h"

#define HOOKER_JNI_VERSION    JNI_VERSION_1_6
#define HOOKER_JNI_CLASS_NAME "com/amap/android/lantern/sample/Lantern"
#define HOOKER_TAG            "lantern_jni"

#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_INFO, HOOKER_TAG, fmt, ##__VA_ARGS__)

static jboolean install(JNIEnv* env, jobject thiz, jint sdk_ver, jintArray black_list)
{
    (void)thiz;
    return AndroidFontsExt_Install(env, sdk_ver, black_list);
}

static void check(JNIEnv* env, jobject thiz)
{
    (void)env;
    (void)thiz;

    size_t count = 0;
    size_t sum = 0;

    FILE* fp = nullptr;
    char line[PATH_MAX] = {0};

    if ((fp = ::fopen("/proc/self/maps", "r")) == nullptr)
    {
        return;
    }

    while (::fgets(line, sizeof(line), fp) != nullptr)
    {
        if (::strcasestr(line, ".ttf") == nullptr
            && ::strcasestr(line, ".otf") == nullptr
            && ::strcasestr(line, ".ttc") == nullptr)
        {
            continue;
        }

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

        LOG("line:%s", line);

        ++count;
        sum += (end - start);
    }

    ::fclose(fp);

    LOG("count:%zu", count);
    LOG("sum:%zuBytes-%zuM", sum, sum / 1024 / 1024);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    if (nullptr == vm)
    {
        return JNI_ERR;
    }

    JNIEnv* env;

    if (JNI_OK != vm->GetEnv((void**)&env, HOOKER_JNI_VERSION))
    {
        return JNI_ERR;
    }

    if (nullptr == env)
    {
        return JNI_ERR;
    }

    jclass cls;

    if (nullptr == (cls = env->FindClass(HOOKER_JNI_CLASS_NAME)))
    {
        return JNI_ERR;
    }

    JNINativeMethod m[] =
    {
        {"native_install", "(I[I)Z", (void*)install},
        {"native_check", "()V", (void*)check},
    };

    if (0 != env->RegisterNatives(cls, m, sizeof(m) / sizeof(m[0])))
    {
        return JNI_ERR;
    }

    return HOOKER_JNI_VERSION;
}
