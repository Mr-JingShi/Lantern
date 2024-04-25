
#pragma once

#include <jni.h>

/**
 * @brief 安装字体扩展库
 * @param env : java env
 * @param sdk_ver : 运行环境 sdk 版本号
 */
bool AndroidFontsExt_Install(JNIEnv* env, jint sdk_ver);