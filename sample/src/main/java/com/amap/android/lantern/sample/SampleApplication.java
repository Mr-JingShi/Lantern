package com.amap.android.lantern.sample;

import android.app.Application;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import com.amap.android.lantern.lantern;

public class SampleApplication extends Application {
    private static String TAG = "lantern";

    public SampleApplication() {
        super();
        Log.i(TAG, "SampleApplication");

        int count = 0;
        try {
            FileInputStream statusFileStream = new FileInputStream("/proc/self/maps");
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(statusFileStream));

            String content;
            while ((content = bufferedReader.readLine()) != null) {
                if (content.contains(".ttf")
                    || content.contains(".otf")
                    || content.contains(".ttc")
                    || content.contains(".TTF")
                    || content.contains(".OTF")
                    || content.contains(".TTC")) {
                    Log.i(TAG, content);
                    ++count;
                }
            }
            Log.i(TAG, "count:"+ count);
            statusFileStream.close();
            bufferedReader.close();
        } catch (Exception ex) { }
    }

    public void onCreate() {
        Log.i(TAG, "SampleApplication onCreate");
        super.onCreate();

        Lantern.check();

        ArrayList<Integer> black_list = new ArrayList<Integer>();

        try {
            AssetManager manager = getBaseContext().getAssets();
            InputStream stream = manager.open("fonts/DancingScript.ttf");
            Log.i(TAG, "SampleApplication " + stream.available());

            black_list.add(stream.available());

            stream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        // black_list.add(14904);

        int[] array = new int[black_list.size()];
        for(int i = 0; i < black_list.size(); i ++) {
            array[i] = black_list.get(i).intValue();

            Log.i(TAG, "SampleApplication " + array[i]);
        }

        // lantern.get_font_data();
        // lantern.get_font_name();

        Lantern.install(array);
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        Log.i(TAG, "attachBaseContext");

        System.loadLibrary("lantern_jni");
    }
}
