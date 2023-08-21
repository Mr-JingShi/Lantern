#include <jni.h>
#include <android/log.h>

#include "lantern.h"

#define HOOKER_JNI_VERSION    JNI_VERSION_1_6
#define HOOKER_JNI_CLASS_NAME "com/amap/android/lantern/sample/Lantern"
#define HOOKER_TAG            "lantern"

#define LOG(fmt, ...) __android_log_print(ANDROID_LOG_INFO, HOOKER_TAG, fmt, ##__VA_ARGS__)

static jboolean install(JNIEnv* env, jobject thiz)
{
    return AndroidFontsExt_Install();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    if (NULL == vm)
    {
        return JNI_ERR;
    }

    JNIEnv* env;

    if (JNI_OK != vm->GetEnv((void**)&env, HOOKER_JNI_VERSION))
    {
        return JNI_ERR;
    }

    if (NULL == env)
    {
        return JNI_ERR;
    }

    jclass cls;

    if (NULL == (cls = env->FindClass(HOOKER_JNI_CLASS_NAME)))
    {
        return JNI_ERR;
    }

    JNINativeMethod m[] =
    {
        {"native_install", "()Z", (void*)install},
    };

    if (0 != env->RegisterNatives(cls, m, sizeof(m) / sizeof(m[0])))
    {
        return JNI_ERR;
    }

    return HOOKER_JNI_VERSION;
}
