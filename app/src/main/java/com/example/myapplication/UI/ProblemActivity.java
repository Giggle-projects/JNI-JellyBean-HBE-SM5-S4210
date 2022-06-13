package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.widget.TextView;

import com.example.myapplication.R;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.TextLCD;

public class ProblemActivity extends AppCompatActivity {

    DotMatrix dotMatrix = HwContainer.dotMatrix;
    TextLCD textLCD = HwContainer.textLcd;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_problem);

        int level = 1;

        dotMatrix.print("3  2  1", 10);
        textLCD.print("Problem Page", "Focus Please!!");

        TextView timer = findViewById(R.id.timerText);

        CountDownTimer countDownTimer = new CountDownTimer(4000, 1000) {

            @Override
            public void onTick(long millisUntilFinished) {
                timer.setText("Timer " + millisUntilFinished / 1000);
            }

            @Override
            public void onFinish() {
                timer.setText("done!");
            }
        };
        countDownTimer.start();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                textLCD.stop();
                dotMatrix.stop();
                Intent intent = new Intent(ProblemActivity.this, GameActivity.class);
                startActivity(intent);
            }
        },3000);


    }
}