package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Vibrator;
import android.widget.Button;
import android.widget.Toast;

import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.FullColorLed;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.LED;
import com.example.myapplication.hw.Piezo;
import com.example.myapplication.hw.Segment;
import com.example.myapplication.hw.TextLCD;


public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("myapplication");
    }

    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Segment segment = HwContainer.segment;
    private final TextLCD textLcd = HwContainer.textLcd;
    private final LED led = HwContainer.led;
    private final Piezo piezo = HwContainer.piezo;
    private final FullColorLed fullColorLed = HwContainer.fullColorLed;

    private long backKeyPressedTime = 0;
    private Toast toast;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        HwContainer.vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        InMemoryDB.init();

        textLcd.print("Let's play a", "Memory Game");
        dotMatrix.print("WelCome", 5, Integer.MAX_VALUE);
        led.printLinear();
        fullColorLed.on(0, 0, 10);
        segment.print(220613);

        piezo.out(50, 100);
        piezo.out(0, 100);
        piezo.out(52, 100);
        piezo.out(0, 100);
        piezo.out(50, 100);
        piezo.out(0, 100);
        piezo.out(46, 100);
        piezo.out(0, 100);
        piezo.out(50, 100);
        piezo.out(52, 500);

        final Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
            segment.stop();
            dotMatrix.stop();
            textLcd.stop();
            led.print(0);
            startActivity(new Intent(MainActivity.this, ProblemActivity.class));
            finish();
        });

        final Button rankBtn = findViewById(R.id.rankBtn);
        rankBtn.setOnClickListener(view -> {
            startActivity(new Intent(MainActivity.this, RankActivity.class));
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
            toast = Toast.makeText(this, "이용해 주셔서 감사합니다.", Toast.LENGTH_LONG);
            toast.show();
        }
    }
}
