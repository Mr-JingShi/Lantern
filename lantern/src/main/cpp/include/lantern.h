
#pragma once

#include <jni.h>

/**
 * @brief 安装字体扩展库
 * @param env : java env
 * @param sdk_ver : 运行环境 sdk 版本号
 * @param black_list : 字体库大小黑名单
 *
 * @remark App装载的且会创建文件映射到maps的字体库文件大小
 * @remark createFromFile方式加载字体库时会创建文件映射到maps
 * @remark createFromAsset方式加载字体库时会创建内存映射到maps
 */
bool AndroidFontsExt_Install(JNIEnv* env, jint sdk_ver, jintArray black_list);