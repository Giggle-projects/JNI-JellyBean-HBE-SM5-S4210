package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Button;

import com.example.myapplication.R;
import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
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
    private final Keypad keypad = HwContainer.keypad;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        Log.d("key down event", keypad.getPositionOfCode(event.getKeyCode())+"");
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textLcd.print("Let's play a", "Memory Game");
        dotMatrix.print("WelCome", 5);
        led.printLinear();
        segment.print(220613);

        //for(int i =0 ; i<Integer.MAX_VALUE; i++) {
        //    piezoControl(i);
        //    SystemClock.sleep(1);
        //}
//
        // for(int i =50 ; i>=0; i--) {
        //     piezoControl(i);
        //     SystemClock.sleep(5);
        // }

        Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
                segment.stop();
                dotMatrix.stop();
                textLcd.stop();
                Intent intent = new Intent(MainActivity.this, ProblemActivity.class);
                startActivity(intent);
        });
    }

    //public native String piezoControl(int i);
}
