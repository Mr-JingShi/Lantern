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

剩余大量字体库未munmap机型

Android 10 OnePlus IN2010

Accessing hidden field Landroid/graphics/Typeface;->sSlateFontMap:Ljava/util/HashMap; (blacklist, JNI, denied)
Accessing hidden field Landroid/graphics/Typeface;->sOriginFontMap:Ljava/util/HashMap; (blacklist, JNI, denied)

```java
public final class SystemFonts {
    private static final String DEFAULT_FAMILY = "sans-serif";
    private static final String TAG = "SystemFonts";
    private static FontConfig.Alias[] sAliases;
    private static List<Font> sAvailableFonts;
    private static final FontConfig.Alias[] sOriginAliases;
    private static List<Font> sOriginAvailableFonts;
    private static Map<String, FontFamily[]> sOriginFallbackMap;
    private static final FontConfig.Alias[] sSlateAliases;
    private static List<Font> sSlateAvailableFonts;
    private static Map<String, FontFamily[]> sSlateFallbackMap;
    private static Map<String, FontFamily[]> sSystemFallbackMap;

    static {
        ArrayMap<String, FontFamily[]> originFallbackMap = new ArrayMap<>();
        ArrayMap<String, FontFamily[]> slateFallbackMap = new ArrayMap<>();
        ArrayList<Font> availableOriginFonts = new ArrayList<>();
        ArrayList<Font> availableSlateFonts = new ArrayList<>();
        FontCustomizationParser.Result oemCustomization = readFontCustomization("/product/etc/fonts_customization.xml", "/product/fonts/");
        if (!OpFontHelperInjector.sFeatureEnable) {
            sOriginAliases = buildSystemFallback("/system/etc/fonts.xml", "/system/fonts/", oemCustomization, originFallbackMap, availableOriginFonts);
        } else {
            sOriginAliases = buildSystemFallback(OpFontHelperInjector.getSystemFontsXmlPath("/system/etc/fonts.xml"), "/system/fonts/", oemCustomization, originFallbackMap, availableOriginFonts);
        }
        sAliases = (FontConfig.Alias[]) sOriginAliases.clone();
        sOriginFallbackMap = Collections.unmodifiableMap(originFallbackMap);
        sSystemFallbackMap = Collections.unmodifiableMap(originFallbackMap);
        sAvailableFonts = Collections.unmodifiableList(availableOriginFonts);
        sOriginAvailableFonts = Collections.unmodifiableList(availableOriginFonts);
        sSlateAliases = buildSystemFallback("/system/etc/fonts_slate.xml", "/system/fonts/", oemCustomization, slateFallbackMap, availableSlateFonts);
        sSlateFallbackMap = Collections.unmodifiableMap(slateFallbackMap);
        sSlateAvailableFonts = Collections.unmodifiableList(availableSlateFonts);
    }
}


public class Typeface {
    public static final int BOLD = 1;
    public static final int BOLD_ITALIC = 3;
    public static final Typeface DEFAULT;
    public static final Typeface DEFAULT_BOLD;
    private static final String DEFAULT_FAMILY = "sans-serif";
    static final String FONTS_ORIGIN_CONFIG = "fonts.xml";
    static final String FONTS_SLATE_CONFIG = "fonts_slate.xml";
    public static final int ITALIC = 2;
    public static final Typeface MONOSPACE;
    public static final int NORMAL = 0;
    public static final int ONEPLUS_FONT_ORIGIN = 1;
    public static final int ONEPLUS_FONT_SLATE = 2;
    public static final Typeface OP_TYPEFACE_MEDIUM;
    public static final int RESOLVE_BY_FONT_TABLE = -1;
    public static final Typeface SANS_SERIF;
    public static final Typeface SERIF;
    private static final int STYLE_ITALIC = 1;
    public static final int STYLE_MASK = 3;
    private static final int STYLE_NORMAL = 0;
    static Typeface sDefaultTypeface;
    @UnsupportedAppUsage(trackingBug = 123769446)
    static Typeface[] sDefaults;
    private static final boolean sOpDynamicFontV2;
    static HashMap<String, Typeface> sOriginFontMap;
    static HashMap<String, Typeface> sSlateFontMap;
    @UnsupportedAppUsage(trackingBug = 123769347)
    static Map<String, Typeface> sSystemFontMap;
    public boolean isLikeDefault;
    public boolean mOpIsThirdTypeface;
    @UnsupportedAppUsage
    private int mStyle;
    private int[] mSupportedAxes;
    private int mWeight;
    @UnsupportedAppUsage
    public long native_instance;
    private static String TAG = "Typeface";
    private static final NativeAllocationRegistry sRegistry = NativeAllocationRegistry.createMalloced(Typeface.class.getClassLoader(), nativeGetReleaseFunc());
    private static String TAG_FONT = "Font";
    @GuardedBy({"sStyledCacheLock"})
    private static final LongSparseArray<SparseArray<Typeface>> sStyledTypefaceCache = new LongSparseArray<>(3);
    private static final Object sStyledCacheLock = new Object();
    @GuardedBy({"sWeightCacheLock"})
    private static final LongSparseArray<SparseArray<Typeface>> sWeightTypefaceCache = new LongSparseArray<>(3);
    private static final Object sWeightCacheLock = new Object();
    @GuardedBy({"sDynamicCacheLock"})
    private static final LruCache<String, Typeface> sDynamicTypefaceCache = new LruCache<>(16);
    private static final Object sDynamicCacheLock = new Object();
    @UnsupportedAppUsage(trackingBug = 123768928)
    @Deprecated
    static final Map<String, FontFamily[]> sSystemFallbackMap = Collections.emptyMap();
    private static final int[] EMPTY_AXES = new int[0];

    static {
        HashMap<String, Typeface> systemFontMap = new HashMap<>();
        OpFontHelperInjector.resetToDefaultIfFeatureOff();
        SystemFonts.changeFallbackFont(2);
        initSystemDefaultTypefaces(systemFontMap, SystemFonts.getRawSystemFallbackMap(), SystemFonts.getAliases());
        sSlateFontMap = new HashMap<>(Collections.unmodifiableMap(systemFontMap));
        systemFontMap.clear();
        SystemFonts.changeFallbackFont(1);
        initSystemDefaultTypefaces(systemFontMap, SystemFonts.getRawSystemFallbackMap(), SystemFonts.getAliases());
        sOriginFontMap = new HashMap<>(Collections.unmodifiableMap(systemFontMap));
        sSystemFontMap = (HashMap) sOriginFontMap.clone();
        if (sSystemFontMap.containsKey(DEFAULT_FAMILY)) {
            setDefault(sSystemFontMap.get(DEFAULT_FAMILY));
        }
        String str = null;
        DEFAULT = create(str, 0);
        DEFAULT_BOLD = create(str, 1);
        SANS_SERIF = create(DEFAULT_FAMILY, 0);
        SERIF = create("serif", 0);
        MONOSPACE = create("monospace", 0);
        sDefaults = new Typeface[]{DEFAULT, DEFAULT_BOLD, create(str, 2), create(str, 3)};
        OP_TYPEFACE_MEDIUM = create("sans-serif-medium", 0);
        String[] genericFamilies = {"serif", DEFAULT_FAMILY, "cursive", "fantasy", "monospace", "system-ui"};
        for (String genericFamily : genericFamilies) {
            registerGenericFamilyNative(genericFamily, systemFontMap.get(genericFamily));
        }
        sOpDynamicFontV2 = OpFontHelperInjector.sFeatureEnable;
    }
}
```

Android 10 samsung SM-G988U

buildSystemFallback 方法被调用了两次， sSystemFallbackMap 只保留了一份 /system/etc/fonts.xml

```java
public final class SystemFonts {
    static {
        ArrayMap<String, FontFamily[]> systemFallbackMap = new ArrayMap<>();
        ArrayList<Font> availableFonts = new ArrayList<>();
        FontCustomizationParser.Result oemCustomization = readFontCustomization("/product/etc/fonts_customization.xml", "/product/fonts/");
        sAliases = buildSystemFallback("/system/etc/fonts.xml", "/system/fonts/", oemCustomization, systemFallbackMap, availableFonts);
        sAvailableFonts = Collections.unmodifiableList(availableFonts);
        ArrayList<Font> availableOmcFonts = new ArrayList<>();
        String fontXmlPath = getOmcEtcPath();
        sOmcAliases = buildSystemFallback(fontXmlPath, "/system/fonts/", oemCustomization, systemFallbackMap, availableOmcFonts);
        sSystemFallbackMap = Collections.unmodifiableMap(systemFallbackMap);
    }
}
```

Android 11 nubia NX629J

Typeface类静态块中systemFontMap因为registerGenericFamilyNative(genericFamily, systemFontMap.get(genericFamily));语句的缘故，持续被native层引用，始终并没有被GC回收

```java
public final class SystemFonts {
    private static final String DEFAULT_FAMILY = "sans-serif";
    private static final String TAG = "SystemFonts";
    private static FontConfig.Alias[] sAliases;
    private static Map<String, FontFamily[]> sSystemFallbackMap = new ArrayMap();
    private static List<Font> sAvailableFonts = new ArrayList();
    
    static {
        ArrayMap<String, FontFamily[]> systemFallbackMap = new ArrayMap<>();
        ArrayList<Font> availableFonts = new ArrayList<>();
        FontCustomizationParser.Result oemCustomization = readFontCustomization("/product/etc/fonts_customization.xml", "/product/fonts/");
        sAliases = buildSystemFallback("/system/etc/fonts.xml", "/system/fonts/", oemCustomization, systemFallbackMap, availableFonts);
        sSystemFallbackMap.putAll(systemFallbackMap);
        sAvailableFonts.addAll(availableFonts);
    }
    
    public static Map<String, FontFamily[]> getRawSystemFallbackMap() {
        return sSystemFallbackMap;
    }
    
    public static void deinit() {
        if (sAliases != null) {
            sAliases = null;
        }
        Map<String, FontFamily[]> map = sSystemFallbackMap;
        if (map != null) {
            map.clear();
        }
        List<Font> list = sAvailableFonts;
        if (list != null) {
            list.clear();
        }
    }

    public static void reload() {
        ArrayMap<String, FontFamily[]> systemFallbackMap = new ArrayMap<>();
        ArrayList<Font> availableFonts = new ArrayList<>();
        FontCustomizationParser.Result oemCustomization = readFontCustomization("/product/etc/fonts_customization.xml", "/product/fonts/");
        sAliases = buildSystemFallback("/system/etc/fonts.xml", "/system/fonts/", oemCustomization, systemFallbackMap, availableFonts);
        sSystemFallbackMap.putAll(systemFallbackMap);
        sAvailableFonts.addAll(availableFonts);
    }
}

public class Typeface {
    public static final int BOLD = 1;
    public static final int BOLD_ITALIC = 3;
    public static final Typeface DEFAULT;
    public static final Typeface DEFAULT_BOLD;
    private static final String DEFAULT_FAMILY = "sans-serif";
    public static final int ITALIC = 2;
    public static final Typeface MONOSPACE;
    public static final int NORMAL = 0;
    public static Typeface NUBIA_DEFAULT = null;
    public static Typeface NUBIA_DEFAULT_BOLD = null;
    public static Typeface NUBIA_SANS_SERIF = null;
    public static final int RESOLVE_BY_FONT_TABLE = -1;
    public static final Typeface SANS_SERIF;
    public static final Typeface SERIF;
    private static final int STYLE_ITALIC = 1;
    public static final int STYLE_MASK = 3;
    private static final int STYLE_NORMAL = 0;
    static Typeface sDefaultTypeface;
    static Typeface[] sDefaults;
    private int mStyle;
    private int[] mSupportedAxes;
    private int mWeight;
    public long native_instance;
    private static String TAG = "Typeface";
    private static final NativeAllocationRegistry sRegistry = NativeAllocationRegistry.createMalloced(Typeface.class.getClassLoader(), nativeGetReleaseFunc());
    private static final LongSparseArray<SparseArray<Typeface>> sStyledTypefaceCache = new LongSparseArray<>(3);
    private static final Object sStyledCacheLock = new Object();
    private static final LongSparseArray<SparseArray<Typeface>> sWeightTypefaceCache = new LongSparseArray<>(3);
    private static final Object sWeightCacheLock = new Object();
    private static final LruCache<String, Typeface> sDynamicTypefaceCache = new LruCache<>(16);
    private static final Object sDynamicCacheLock = new Object();
    static Map<String, Typeface> sSystemFontMap = new ArrayMap();
    @Deprecated
    static final Map<String, FontFamily[]> sSystemFallbackMap = Collections.emptyMap();
    private static final int[] EMPTY_AXES = new int[0];

    static {
        HashMap<String, Typeface> systemFontMap = new HashMap<>();
        initSystemDefaultTypefaces(systemFontMap, SystemFonts.getRawSystemFallbackMap(), SystemFonts.getAliases());
        sSystemFontMap.putAll(systemFontMap);
        if (sSystemFontMap.get(DEFAULT_FAMILY) == null) {
            Log.w(TAG, "DEFAULT_FAMILY is null, reload again");
            FontListParser.sReloadWithoutRedirect = true;
            reload();
            FontListParser.sReloadWithoutRedirect = false;
        } else {
            setDefault(sSystemFontMap.get(DEFAULT_FAMILY));
        }
        String str = null;
        DEFAULT = create(str, 0);
        DEFAULT_BOLD = create(str, 1);
        SANS_SERIF = create(DEFAULT_FAMILY, 0);
        SERIF = create("serif", 0);
        MONOSPACE = create("monospace", 0);
        NUBIA_DEFAULT = create(DEFAULT_FAMILY, 0);
        NUBIA_DEFAULT_BOLD = create(DEFAULT_FAMILY, 1);
        NUBIA_SANS_SERIF = create(DEFAULT_FAMILY, 0);
        sDefaults = new Typeface[]{DEFAULT, DEFAULT_BOLD, create(str, 2), create(str, 3)};
        String[] genericFamilies = {"serif", DEFAULT_FAMILY, "cursive", "fantasy", "monospace", "system-ui"};
        for (String genericFamily : genericFamilies) {
            registerGenericFamilyNative(genericFamily, systemFontMap.get(genericFamily));
        }
    }
    
    private static void deinit() {
        sStyledTypefaceCache.clear();
        Map<String, Typeface> map = sSystemFontMap;
        if (map != null) {
            map.clear();
        }
        SystemFonts.deinit();
    }

    public static void reload() {
        deinit();
        SystemFonts.reload();
        HashMap<String, Typeface> systemFontMap = new HashMap<>();
        initSystemDefaultTypefaces(systemFontMap, SystemFonts.getRawSystemFallbackMap(), SystemFonts.getAliases());
        sSystemFontMap.putAll(systemFontMap);
        setDefault(sSystemFontMap.get(DEFAULT_FAMILY));
        NUBIA_DEFAULT = create(DEFAULT_FAMILY, 0);
        NUBIA_DEFAULT_BOLD = create(DEFAULT_FAMILY, 1);
        NUBIA_SANS_SERIF = create(DEFAULT_FAMILY, 0);
        String str = null;
        sDefaults = new Typeface[]{NUBIA_DEFAULT, NUBIA_DEFAULT_BOLD, create(str, 2), create(str, 3)};
    }
}

```

****************************************************************************************************