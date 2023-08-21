package com.amap.android.lantern.sample;

public class Lantern
{
    public static boolean install()
    {
        return native_install();
    }

    private static native boolean native_install();
}
