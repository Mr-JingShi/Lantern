# Lantern概述

## 字体库配置文件

### 原生配置文件

Android 5.0 ~ 9

/system/etc/fonts.xml ------ /system/fonts/

Android 10 ~ 11

/system/etc/fonts.xml ------ /system/fonts/

/product/etc/fonts_customization.xml ------ /product/fonts/

### 厂商配置文件

高 API 一加手机

/system/etc/fonts.xml ------ /system/fonts/

/system/etc/fonst_base.xml ------ /system/fonts/ 

/system/etc/fonts_slate.xml ------ /system/fonts/

使用 fonts.xml 、 fonts_slate.xml 的某种组合形式

## 字体库加载

Android 5.0 ~ 6.0 字体库是在 Native 层 libskia.so 中通过mmap建立ttf-memory的映射，
 fonts.xml 同一个 ttf 存在多份时， ttf-memory 映射关系就存在多份。
如，LG-H422 Android 5.0.1机型：

adb44000-ae3c4000 r--p 00000000 b3:09 1042       /system/fonts/NotoColorEmoji.ttf

ae3c4000-aec44000 r--p 00000000 b3:09 1042       /system/fonts/NotoColorEmoji.ttf

Android 7.0 ~ 11 字体库是在 Java 层通过 fileChannel.map 建立ttf-memory的映射，
 fonts.xml 同一个 ttf 存在多份时，因为有 HashMap<String, ByteBuffer> bufferCache 缓存的原因，
 ttf-memory 映射关系只映射一份（在存在软连接的情况下，java读到的是软连接名称，而 maps 映射的是实体名称，
fonts.xml 中同时配置了软连接和实体，可能会出现多份，多个软连接指向同一个实体文件，fonts.xml 中同时配置了
指向同一个实体文件的多个软连接时，可能会出现多份）。 在随后的处理中 ByteBuffer 被提升成 Global ，
在 Global 减到0时，交给了 GC 。
```c++
    jobject fontRef = MakeGlobalRefOrDie(env, font);
```
```c++
    static void release_global_ref(const void* /*data*/, void* context) {
        JNIEnv* env = AndroidRuntime::getJNIEnv();
        bool needToAttach = (env == NULL);
        if (needToAttach) {
            JavaVMAttachArgs args;
            args.version = JNI_VERSION_1_4;
            args.name = "release_font_data";
            args.group = NULL;
            jint result = AndroidRuntime::getJavaVM()->AttachCurrentThread(&env, &args);
            if (result != JNI_OK) {
                ALOGE("failed to attach to thread to release global ref.");
                return;
            }
        }
    
        jobject obj = reinterpret_cast<jobject>(context);
        env->DeleteGlobalRef(obj);
    
        if (needToAttach) {
           AndroidRuntime::getJavaVM()->DetachCurrentThread();
        }
    }
```

Android 12 ～ x 似乎 Google 已经意识到该把手术刀刀向字体库了， Google 工程师基本上已经把字体库修改成按需加载了。

## 常驻字体库

阅读 Android 源码可以得知 */fonst.xml 中有效 ttf 均被 zygote 加载成常驻字体库
（字体库生命周期大于App生命周期）。 像部分 VIVO 、 OPPO 机型App启动后 GC 会回收的字体库成为非常驻字体库，
经过大量测试发现：

（1）谷歌：（完全按照原生 Android 架构）加载 fonts.xml 内有效 ttf 。

（2）小米、三星、魅族等修改默认字体库和支持系统级自定义字体库的机型：加载 */fonst.xml 内有效 ttf 和 几个有效系统级自定义字体库。

（3） VIVO 、 OPPO 部分机型：加载 */fonst.xml 内有效 ttf 、 几个有效系统级自定义字体库和上次使用当次未使用的系统级自定义字体库。
这其中包括在切换系统级自定义字体库时可能会夹杂着加载 /system/fonts/Roboto-Regular.ttf 等 */fonst.xml 中存在的字体库，
即 ttf-memory 映射关系变成了多份。

c5b70000-c6c5d000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

c6c5d000-c7d4a000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

c7d4a000-c8e37000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

ca7d1000-cb8be000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

cb8be000-cc9ab000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

cc9ab000-cda98000 r--s 00000000 fe:02 2859011                            /data/fonts/颜真卿经典行楷两万字.ttf

cfe2d000-d0662000 r--s 00000000 fe:00 2230                               /system/fonts/DroidSansBoldBBK.ttf

d0662000-d0ea2000 r--s 00000000 fe:00 2233                               /system/fonts/DroidSansMediumBBK.ttf

d0ea2000-d16ef000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

d16ef000-d2824000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d2824000-d3959000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d3959000-d4a8e000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d4a8e000-d52c3000 r--s 00000000 fe:00 2230                               /system/fonts/DroidSansBoldBBK.ttf

d52c3000-d5b03000 r--s 00000000 fe:00 2233                               /system/fonts/DroidSansMediumBBK.ttf

d5b03000-d6350000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

d6350000-d7485000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d7485000-d85ba000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d85ba000-d96ef000 r--s 00000000 fe:02 2859013                            /data/fonts/我的期盼是如愿以偿.ttf

d96ef000-d9f24000 r--s 00000000 fe:00 2230                               /system/fonts/DroidSansBoldBBK.ttf

d9f24000-da764000 r--s 00000000 fe:00 2233                               /system/fonts/DroidSansMediumBBK.ttf

e863e000-e8e73000 r--s 00000000 fe:00 2230                               /system/fonts/DroidSansBoldBBK.ttf

e8e73000-e96b3000 r--s 00000000 fe:00 2233                               /system/fonts/DroidSansMediumBBK.ttf

e96b3000-e9f00000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

f2043000-f2890000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

I/art: Starting a blocking GC Explicit

I/art: Explicit concurrent mark sweep GC freed 25912(1877KB) AllocSpace objects, 2(40KB) LOS objects, 57% free, 5MB/13MB, paused 574us total 46.584ms

cfe2d000-d0662000 r--s 00000000 fe:00 2230                               /system/fonts/DroidSansBoldBBK.ttf

d0662000-d0ea2000 r--s 00000000 fe:00 2233                               /system/fonts/DroidSansMediumBBK.ttf

d0ea2000-d16ef000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

e96b3000-e9f00000 r--s 00000000 fe:00 2232                               /system/fonts/DroidSansFallbackBBK.ttf

（4）部分红魔机型：将 /system/etc/fonts.xml 加载了三遍，即每个 ttf/ttc/otf 存在三份映射关系，触发GC后，
会 munmap 调1/3， ，剩于2/3（每个 ttf/ttc/otf 存在两份份映射关系）。

（5）部分一加机型：/system/etc/路径下存在 fonst.xml 、 fonst_base.xml 、 fonts_slate.xml 等字体配置，
貌似把 fonst.xml 、 fonts_slate.xml都给加载了进来， 且并未约束 ttf-memory 映射关系为一份，其加载字体库个数多达500多个，
触发GC，并不会 munmap 。

 (6) 三星 SM-G988U 、 
 SM-N975F 机型，将 /system/etc/fonts.xml 加载了两遍，即每个 ttf/ttc/otf 存在两份映射关系，触发GC，并不会 munmap 。

对于三星SM-G988U和一加等触发GC，并不会 munmap 的机型，或许可以采取激进做法，将其字体库全部 munmap 掉。

（7）大部分手机在 App 启动后会加载自更新相关的字体库

b9a74000-ba480000 r--s 00000000 fd:0c 179235                             /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf

cf8b4000-d02c0000 r--s 00000000 fd:0c 179235                             /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf


 App 启动时从 zygote 继承来的，*/fonts.xml 中配置的且 single ttf-memory 映射关系的字体库我们统称为常驻字体库，
 App 启动时不在 */fonts.xml 中或在 */fonts.xml 中但不是 single ttf-memory 映射关系的字体库我们称为非常驻字体库，
 App 启动后通过Typeface.createFromFile映射的字体库或者【设置/个性化】切换的字体我们称为非常驻字体库。
我们的方案就是要munmap调常驻字体库， 如果munmap掉 非常驻字体库极有可能造成崩溃。

## 多份 ttf-memory 映射关系

（1）Android 原生系统引起，Android 5 ～ 6 未做去重处理

LG-H422 Android 5.0.1机型：

adb44000-ae3c4000 r--p 00000000 b3:09 1042       /system/fonts/NotoColorEmoji.ttf

ae3c4000-aec44000 r--p 00000000 b3:09 1042       /system/fonts/NotoColorEmoji.ttf

 (2) 手机厂商引起

小米8 Android 10机型：

afe01000-b0b34000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b0b34000-b1867000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b1867000-b259a000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b259a000-b32cd000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b32cd000-b4000000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b409b000-b4dce000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b4dce000-b5b01000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b5b01000-b6834000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b6834000-b7567000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b7567000-b829a000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b829a000-b8fcd000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b8fcd000-b9d00000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

b9d00000-baa33000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

baa33000-bb766000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

bb766000-bc499000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

bc499000-bd1cc000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

bd1cc000-bdeff000 r--s 00000000 103:05 527751                            /data/system/theme/fonts/Roboto-Regular.ttf

（3）App引起

```java
    Typeface tf = Typeface.createFromFile("/system/fonts/Roboto-Medium.ttf");

    Button button = findViewById(R.id.test_path_font);

    button.setTypeface(tf);
    button.setText("Load /system/fonts/Roboto-Medium.ttf");
        
    ...
        
    Typeface tf1 = Typeface.createFromFile("/system/fonts/Roboto-Medium.ttf");
    
    ...
```

f2632000-f267d000 r--s 00000000 fe:00 2359                               /system/fonts/Roboto-Medium.ttf

f2731000-f277c000 r--s 00000000 fe:00 2359                               /system/fonts/Roboto-Medium.ttf

## munmap时机

最好放到 Application.onCreate 中，太早 maps 文件可能加载不全

不要放到 Activity.onCreate 中，对于切换系统字体库不重启的机型，在切换系统字体库时会触发 Activity.onCreate 操作，这样会造成重复操作。
