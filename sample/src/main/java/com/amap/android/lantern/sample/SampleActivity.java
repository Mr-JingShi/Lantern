package com.amap.android.lantern.sample;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Typeface;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;

public class SampleActivity extends AppCompatActivity {
    private String TAG = "lantern";

    private Typeface[] typefaceVar = {Typeface.DEFAULT, Typeface.DEFAULT_BOLD, Typeface.SANS_SERIF, Typeface.SERIF, Typeface.MONOSPACE};
    private int[] styleVar = {Typeface.NORMAL, Typeface.BOLD, Typeface.ITALIC, Typeface.BOLD_ITALIC};

    private int[] flagVar = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x100 | 0x400 | 0x02};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "SampleActivity onCreate");
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_sample);
    }

    public void on_check_clicked(View view) {
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

    public void on_test_gc_clicked(View view) {
        // Code taken from AOSP FinalizationTest:
        // https://android.googlesource.com/platform/libcore/+/master/support/src/test/java/libcore/java/lang/ref/FinalizationTester.java
        Runtime.getRuntime().gc();
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {}
        System.runFinalization();
    }

    private int lang = 0;
    private String langs[] = {
            "日语 高徳の地図",
            "俄语 Карта Гаоде",
            "法语 Carte de Gaodé",
            "泰语 แผนที่",
            "卡纳达语 ಒಂದು ನಕ್ಷೆ",
            "乌克兰语 Карта Гаоде",
            "蒙古语 Гаод газрын зураг",
            "老挝语 ແຜນທີ່ Gaode",
            "缅甸语 Gaode မြေပုံ",
            "高棉语 ផែនទី Gaode",
            "哈萨克语 Гаод картасы",
            "梵语 गाओडे मानचित्र",
            "西班牙语 mapa de Gaodé",
            "信德语 گاديءَ جو نقشو"
    };
    public void on_test_ja_clicked(View view) {
        Button button = findViewById(R.id.test_ja);
        button.setText("继续点击，" + langs[lang++%langs.length]);
    }

    private int tf_ = 0;
    private int style_ = 0;
    private int flag_ = -1;
    private boolean next = false;
    public void on_test_style_clicked(View view) {
        Button button = findViewById(R.id.test_style);

        flag_++;
        if (flag_ == 14) {
            style_++;
            flag_ = 0;
            if (style_ == 3) {
                tf_++;
                style_ = 0;
                if (tf_ == 5) {
                    flag_ = 0;
                    style_ = 0;
                    tf_ = 0;

                    next = true;
                }
            }
        }

//        tf_ = 1; // 2 2 2 2 0 0
//        style_ = 0; // 0 1 0 0 0 0
//        flag_ = 13; // 3 4 2 9 11 12

        Log.i(TAG, "index:" + tf_ + " tf:" + typefaceVar[tf_]);
        Log.i(TAG, "index:" + style_ + " style:" + styleVar[style_]);
        Log.i(TAG, "index:" + flag_ + " flag:" + flagVar[flag_]);

        button.setTypeface(typefaceVar[tf_], styleVar[style_]);
        button.getPaint().setFlags(flagVar[flag_]);

        if (next) {
            button.setText("继续点击，再次加载下一个style组合\n123abcDEF中国-_-");
        } else {
            button.setText("继续点击，加载下一个style组合\n123abcDEF中国-_-");
        }
    }
    public void on_test_emojj_clicked(View view)
    {
        Button button = findViewById(R.id.test_emojj);
        button.setText("\uD83D\uDC4F\uD83D\uDE18");
    }
    public void on_test_symbol_clicked(View view)
    {
        Button button = findViewById(R.id.test_symbol);
        button.setText("®❡■⚫囧☝✍貳♉㍼﹄㊉㈩ༀད");
    }
    public void on_test_unknow_clicked(View view)
    {
        Button button = findViewById(R.id.test_unknow);
        button.setText("犇猋麤鎏顕燊琞");
    }

    public void on_test_asset_font_clicked(View view)
    {
        Typeface tf = Typeface.createFromAsset(getAssets(), "fonts/DancingScript.ttf");

        Button button = findViewById(R.id.test_asset_font);

        button.setTypeface(tf);
        button.setText("Load DancingScript.ttf");
    }

    public void on_test_path_font_clicked(View view)
    {
        try {
            Typeface tf = Typeface.createFromFile("/system/fonts/Roboto-Medium.ttf");

            Button button = findViewById(R.id.test_path_font);

            button.setTypeface(tf);
            button.setText("Load /system/fonts/Roboto-Medium.ttf");
        } catch (Exception e) {}
    }
}
