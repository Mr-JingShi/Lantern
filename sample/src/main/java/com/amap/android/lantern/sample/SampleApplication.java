package com.amap.android.lantern.sample;

import android.app.Application;
import android.content.Context;
import android.util.Log;

public class SampleApplication extends Application
{
    private String TAG = "lantern";

    public void onCreate()
    {
        super.onCreate();

        Lantern.install();

        Log.i(TAG, "onCreate");
    }

    @Override
    protected void attachBaseContext(Context base)
    {
        super.attachBaseContext(base);
        Log.i(TAG, "attachBaseContext");

        System.loadLibrary("lantern");
        System.loadLibrary("lantern_jni");
    }
}
