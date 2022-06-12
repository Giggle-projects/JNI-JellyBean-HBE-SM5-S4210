package com.example.myapplication;


import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Button;

import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.LED;
import com.example.myapplication.hw.Segment;
import com.example.myapplication.hw.TextLCD;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Segment segment = HwContainer.segment;
    private final TextLCD textLcd = HwContainer.textLcd;
    private final LED led = HwContainer.led;

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

        textLcd.startTextLCD("hi", "hello");
        dotMatrix.print("WelCome");
        led.printLinear();
        segment.print(220613);

        for(int i =0 ; i<50; i++) {
            piezoControl(i);
            SystemClock.sleep(50);
        }

        for(int i =50 ; i>=0; i--) {
            piezoControl(i);
            SystemClock.sleep(50);
        }

        Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
                dotMatrix.stop();
                segment.stop();
                textLcd.startTextLCD("", "");
                piezoControl(0);
                Intent intent = new Intent(MainActivity.this, ProblemActivity.class);
                startActivity(intent);
        });
    }

    public native String piezoControl(int i);
}

