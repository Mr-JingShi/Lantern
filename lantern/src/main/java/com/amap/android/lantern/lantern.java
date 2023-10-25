/**
 * COPYRIGHT NOTICE
 * Copyright (C), 2022-2032, AutoNavi. All Rights Reserved
 * @file        PLTHook.java
 * @brief       PLTHook库--PLTHook
 * @version     1.0
 * @author      经十
 * @date        修订说明：
 *              v1.0 - 14/07/2022-经十-创建
 */

package com.amap.android.lantern;

import android.util.Log;
import android.util.Xml;
import org.xmlpull.v1.XmlPullParser;

import java.lang.reflect.Method;
import java.util.Set;
import java.nio.ByteBuffer;
import java.io.FileInputStream;
import java.util.regex.Pattern;


public class lantern {

    private static String TAG = "lantern";
    public static void get_font_data() {
        try {
            Class<?> SystemFonts = Class.forName("android.graphics.fonts.SystemFonts");

            Method getAvailableFonts = SystemFonts.getMethod("getAvailableFonts");

            Set<Object> sAvailableFonts = (Set<Object>)getAvailableFonts.invoke(null);

            Class<?> Font = Class.forName("android.graphics.fonts.Font");

            Method getBuffer = Font.getMethod("getBuffer");

            for (Object font : sAvailableFonts) {
                ByteBuffer buffer = (ByteBuffer)getBuffer.invoke(font);
            }

        } catch (Exception e) {
            Log.i(TAG, e.toString());
        }
    }

    private static final Pattern FILENAME_WHITESPACE_PATTERN =
            Pattern.compile("^[ \\n\\r\\t]+|[ \\n\\r\\t]+$");
    public static void get_font_name() {
        try {
            Log.i(TAG, "111----");
            FileInputStream fontsIn = new FileInputStream("/system/etc/fonts.xml");

            XmlPullParser parser = Xml.newPullParser();
            parser.setInput(fontsIn, null);
            parser.nextTag();

            parser.require(XmlPullParser.START_TAG, null, "familyset");
            while (parser.next() != XmlPullParser.END_TAG) {
                if (parser.getEventType() != XmlPullParser.START_TAG) continue;

                if (parser.getName().equals("family")) {
                    while (parser.next() != XmlPullParser.END_TAG) {
                        if (parser.getEventType() != XmlPullParser.START_TAG) continue;
                        String tag = parser.getName();
                        if (tag.equals("font")) {
                            StringBuilder filename = new StringBuilder();
                            while (parser.next() != XmlPullParser.END_TAG) {
                                if (parser.getEventType() == XmlPullParser.TEXT) {
                                    filename.append(parser.getText());
                                } else if (parser.getEventType() == XmlPullParser.START_TAG) {
                                    skip(parser);
                                }
                            }
                            String sanitizedName = FILENAME_WHITESPACE_PATTERN.matcher(filename).replaceAll("");

                            Log.i(TAG, "filename:" + sanitizedName);
                        } else {
                            skip(parser);
                        }
                    }
                } else {
                    skip(parser);
                }
            }

            fontsIn.close();

            Log.i(TAG, "111----");
        } catch (Exception e) {
            Log.i(TAG, e.toString());
        }
    }

    private static void skip(XmlPullParser parser) throws Exception {
        int depth = 1;
        while (depth > 0) {
            switch (parser.next()) {
                case XmlPullParser.START_TAG:
                    depth++;
                    break;
                case XmlPullParser.END_TAG:
                    depth--;
                    break;
            }
        }
    }
}
