package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import com.example.myapplication.R;
import com.example.myapplication.api.RandomApi;
import com.example.myapplication.domain.Answer;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Life;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.FullColorLed;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.TextLCD;

import java.util.Random;

public class ProblemActivity extends AppCompatActivity {

    private static final Random random = new Random();
    private final Answer answer = InMemoryDB.getAnswer();
    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final TextLCD textLCD = HwContainer.textLcd;
    private final FullColorLed fullColorLed = HwContainer.fullColorLed;
    private final Life life = InMemoryDB.getLife();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_problem);

        dotMatrix.print("Wait!", 5, Integer.MAX_VALUE);
        textLCD.print("Problem Page", "Focus Please!!");
        life.printLed();
        fullColorLed.on(5, 10, 0);
        fullColorLed.off(8);

        final ButtonTable buttonTable = ButtonTable.initFromActivity(this);
        final int newAnswer = getNewAnswer();
        buttonTable.setColor(newAnswer, 0);

        final TextView timer = findViewById(R.id.timerText);
        new CountDownTimer(4000, 1000) {
            @Override
            public void onTick(long millisUntilFinished) {
                timer.setText("Timer " + millisUntilFinished / 1000);
            }
            @Override
            public void onFinish() {
                timer.setText("done!");
            }
        }.start();

        new Handler().postDelayed(() -> {
            textLCD.stop();
            dotMatrix.stop();
            startActivity(new Intent(ProblemActivity.this, GameActivity.class));
            finish();
        },3000);
    }

    private int getNewAnswer() {
        final int num = RandomApi.getRand();
        answer.add(life, num);
        return num;
    }
}
