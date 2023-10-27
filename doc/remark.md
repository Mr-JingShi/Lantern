# 特殊事项

****************************************************************************************************

加载asset-App级别自定义字体库

memory_base:0xeb142000

I/lantern: FT_Open_Face_proxy begin

I/lantern: FT_Open_Face_proxy face_index:0

I/lantern: FT_Open_Face_proxy flags:1

I/lantern: FT_Open_Face_proxy memory_base:0xeb142000 memory_size:130480

I/lantern: FT_Open_Face_proxy ret:0 face:0xf34cc980 extensions:0xf33b3ced

I/lantern: FT_Open_Face_proxy end

0xeb142000位于[anon:libc_malloc]中

I/lantern: e97e5000-eade5000 r--s 00000000 b3:1d 3073                               /system/usr/icu/icudt55l.dat

I/lantern: eb140000-eb180000 rw-p 00000000 00:00 0                                  [anon:libc_malloc]

I/lantern: eb4b0000-eb834000 rw-s 00000000 00:08 8012                               anon_inode:dmabuf

memory_base:0xc3b84900

I/lantern: FT_Open_Face_proxy begin

I/lantern: FT_Open_Face_proxy face_index:0

I/lantern: FT_Open_Face_proxy flags:1

I/lantern: FT_Open_Face_proxy memory_base:0xc3b84900 memory_size:130480

I/lantern: FT_Open_Face_proxy ret:0 face:0xc3799380 extensions:0xc3296ced

I/lantern: FT_Open_Face_proxy end

0xc3b84900位于[anon:libc_malloc]中

I/lantern: c3584000-c3585000 ---p 00000000 00:00 0

I/lantern: c3585000-c3d05000 rw-p 00000000 00:00 0                                  [anon:libc_malloc]

I/lantern: c3d05000-c4273000 r--p 00000000 00:00 0                                  [anon:dalvik-classes.dex extracted in memory from /data/app/com.amap.android.l

****************************************************************************************************

vivi 从Android 5.0开始已经支持系统级自定义字体库

LGE 从Android 5.0开始已经支持系统级自定义字体库

SAMSUNG 从Android 5.0开始已经支持系统级自定义字体库

****************************************************************************************************

oppo 7.1.1 libskia.so --> libskia_mtk.so

****************************************************************************************************
vivo Android 7.1.2 加载之前使用当次未使用的系统级自定义字体库，后续会GC调之前使用当次未使用的系统级自定义字体库

vivo Android 7.1.1 加载之前使用当次未使用的系统级自定义字体库，后续会GC调之前使用当次未使用的系统级自定义字体库

vivo Android 8.1 加载之前使用当次未使用的系统级自定义字体库，后续会GC调之前使用当次未使用的系统级自定义字体库

oppo Android 11 从系统级自定义字体库切换回默认字体库时，会多加载一个/system/fonts/Roboto-Regular.ttf，后续此ttf会被GC掉

c9ce8000-ca6f4000 r--s 00000000 08:0a 1053947                            /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf

ca6f4000-cb100000 r--s 00000000 08:0a 1053947                            /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf

cbad1000-ccfe3000 r--s 00000000 fd:00 1183                               /system/fonts/SysSans-Hans-Regular.ttf

e6a1f000-e6a6a000 r--s 00000000 fd:00 1152                               /system/fonts/Roboto-Regular.ttf

e7cbe000-e7cc0000 r--s 00000000 fd:00 1142                               /system/fonts/OplusOSUI-XThin.ttf

e9a8b000-e9ad6000 r--s 00000000 fd:00 1152                               /system/fonts/Roboto-Regular.ttf

ebb02000-ebb4d000 r--s 00000000 fd:00 1150                               /system/fonts/Roboto-Medium.ttf

Explicit concurrent copying GC freed 9050(559KB) AllocSpace objects, 3(60KB) LOS objects, 67% free, 2897KB/9041KB, paused 115us total 31.362ms

oneway function results will be dropped but finished with status OK and parcel size 4

c9ce8000-ca6f4000 r--s 00000000 08:0a 1053947                            /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf

ca6f4000-cb100000 r--s 00000000 08:0a 1053947                            /data/data/com.google.android.gms/files/fonts/opentype/Noto_Color_Emoji_Compat-400-100_0-0_0.ttf

cbad1000-ccfe3000 r--s 00000000 fd:00 1183                               /system/fonts/SysSans-Hans-Regular.ttf

e6a1f000-e6a6a000 r--s 00000000 fd:00 1152                               /system/fonts/Roboto-Regular.ttf

e7cbe000-e7cc0000 r--s 00000000 fd:00 1142                               /system/fonts/OplusOSUI-XThin.ttf

ebb02000-ebb4d000 r--s 00000000 fd:00 1150                               /system/fonts/Roboto-Medium.ttf

****************************************************************************************************

切换默认字体库时，有些机型重启App，有些机型不重启App。

小米 5X Android 8.1 切换默认字体，需要重启手机。

****************************************************************************************************

红魔3 Android 11 会把fonts.xml加载三遍，高达774个376M，后续触发GC时，会munmap调1/3

****************************************************************************************************

(delete) 标识混乱

切换默认字体库时，不重启App的机型，大多数会把之前使用当次未使用的字体库打上 (delete) 标识，且触发GC时会释放掉部分之前使用当次未使用的字体库。

****************************************************************************************************

vivi Android 10 

/system/fonts/DroidSansFallbackMonster.ttf为默认字体库时

oldface->face_flags:0x8b39

newface->face_flags:0x0b39

需要 swap face->face_flags

****************************************************************************************************

SONY D6503 Android 6.0.1 出现隐藏字体

/system/fonts/.SST-CondensedBd.ttf

/system/fonts/.SST-Condensed.ttf

/system/fonts/.SST-HeavyItalic.ttf

/system/fonts/.SST-Heavy.ttf

/system/fonts/.SST-MediumItalic.ttf

/system/fonts/.SST-Medium.ttf

/system/fonts/.SSTVietnamese-Bold.ttf

/system/fonts/.SSTVietnamese-Roman.ttf

/system/fonts/.SST-LightItalic.ttf

/system/fonts/.SST-Light.ttf

/system/fonts/.SST-UltraLightItalic.ttf

/system/fonts/.SST-UltraLight.ttf

****************************************************************************************************

xml里的字体库是软连接时，maps里映射的是实体文件，因此必须找到实体文件才行

如 SAMSUNG SM-G965F Android 8.0.0

SECRobotoLight-Bold.ttf -> Roboto-Medium.ttf

****************************************************************************************************

HUAWEI HONOR KIW-AL10  Android 6.0

在执行 SkTypeface::openStream 时会 munmap 旧的 DroidSansChinese.ttf ，然后 mmap 新的 DroidSansChinese.ttf
不执行lantern的任何操作，KIW-AL10机型额会进行上述替换操作。

使用默认 DroidSansChinese.ttf 时

e7bfe000-e84b6000 r--p 00000000 b3:18 1895 /system/fonts/DroidSansChinese.ttf

ea54e000-eae06000 r--p 00000000 b3:18 1895 /system/fonts/DroidSansChinese.ttf

munmap_proxy addr:0xe7bfe000 size:9139596

mmap_proxy ret:0xe7bfe000 size:9139596, fd:28

mmap_proxy readlink fd:/proc/self/fd/28

mmap_proxy readlink file:/system/fonts/DroidSansChinese.ttf

mmap_proxy ret:0xe7bfe000

使用自定义 DroidSansChinese.ttf 时

e7bfe000-e84b6000 r--p 00000000 b3:18 1895 /system/fonts/DroidSansChinese.ttf

ea54e000-eae06000 r--p 00000000 b3:18 1895 /system/fonts/DroidSansChinese.ttf

munmap_proxy addr:0xe7bfe000 size:9139596

mmap_proxy ret:0xe7fb0000 size:5264484, fd:28

mmap_proxy readlink fd:/proc/self/fd/28

mmap_proxy readlink file:/data/themes/0/fonts/DroidSansChinese.ttf

mmap_proxy ret:0xe7fb0000

****************************************************************************************************