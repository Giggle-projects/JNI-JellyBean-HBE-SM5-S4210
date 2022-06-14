package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.widget.TextView;

import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.domain.Life;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.LED;
import com.example.myapplication.hw.TextLCD;

public class ProblemActivity extends AppCompatActivity {

    final DotMatrix dotMatrix = HwContainer.dotMatrix;
    final TextLCD textLCD = HwContainer.textLcd;
    final Life life = InMemoryDB.getLife();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_problem);

        dotMatrix.print("3  2  1", 10, 10);
        textLCD.print("Problem Page", "Focus Please!!");
        life.printLed();

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