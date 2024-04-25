package com.amap.android.lantern.sample;

import android.os.Build;

public class Lantern
{
    private static String TAG = "Lantern";
    public static boolean install() {
        return native_install(Build.VERSION.SDK_INT);
    }
    public static void check()
    {
        native_check();
    }

    private static native boolean native_install(int sdk_ver);
    private static native void native_check();
}
