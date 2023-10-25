package com.amap.android.lantern.sample;

import android.os.Build;

public class Lantern
{
    private static String TAG = "Lantern";
    public static boolean install(int[] black_list) {
        return native_install(Build.VERSION.SDK_INT, black_list);
    }
    public static void check()
    {
        native_check();
    }

    private static native boolean native_install(int sdk_ver, int[] black_list);
    private static native void native_check();
}
