package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Vibrator;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Button;
import android.widget.Toast;

import com.example.myapplication.R;
import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.LED;
import com.example.myapplication.hw.Piezo;
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
    private final Piezo piezo = HwContainer.piezo;
    private final Keypad keypad = HwContainer.keypad;

    private long backKeyPressedTime = 0;
    private Toast toast;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        Log.d("key down event", keypad.getPositionOfCode(event.getKeyCode()) + "");
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        HwContainer.vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
        InMemoryDB.init();

        textLcd.print("Let's play a", "Memory Game");
        dotMatrix.print("WelCome", 5, Integer.MAX_VALUE);
        led.printLinear();
        segment.print(220613);

        piezo.out(50, 1000);
        piezo.out(0, 100);
        piezo.out(52, 1000);
        piezo.out(0, 100);
        piezo.out(50, 1000);
        piezo.out(0, 100);

        Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
            segment.stop();
            dotMatrix.stop();
            textLcd.stop();
            led.print(0);
            startActivity(new Intent(MainActivity.this, ProblemActivity.class));
            finish();
        });
    }

    @Override
    public void onBackPressed() {
        if (System.currentTimeMillis() > backKeyPressedTime + 2500) {
            backKeyPressedTime = System.currentTimeMillis();
            toast = Toast.makeText(this, "뒤로 가기 버튼을 한 번 더 누르시면 종료됩니다.", Toast.LENGTH_LONG);
            toast.show();
            return;
        }
        if (System.currentTimeMillis() <= backKeyPressedTime + 2500) {
            finish();
            toast.cancel();
            toast = Toast.makeText(this,"이용해 주셔서 감사합니다.",Toast.LENGTH_LONG);
            toast.show();
        }
    }
}
