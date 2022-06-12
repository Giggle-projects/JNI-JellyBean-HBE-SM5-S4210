package com.example.myapplication;

import static com.example.myapplication.Utils.assertEqual;
import static com.example.myapplication.Utils.assertSucceed;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.TextView;

import com.example.myapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (event.getKeyCode() == 145) {
            Log.d("KeyUP Event", event.getKeyCode() + "");
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        TextView tv = binding.sampleText;
        tv.setText("yongjin");

        ledControl(0b1011_0100);
        for (int count = 1000; count >= 0; count--) {
            segmentControl(count);
        }
        lcdClear();
        lcdPrint(0, "hi");
        lcdPrint(1, "hello");
    }

    public native String ledControl(int bitCount);
    public native String piezoControl(int i);
    public native String segmentControl(int data);
    public native String lcdClear();
    public native String lcdPrint(int lineIndex, String msg);
}
