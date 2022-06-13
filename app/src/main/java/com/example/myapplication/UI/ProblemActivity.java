package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.widget.Button;
import android.widget.TextView;

import com.example.myapplication.R;

public class ProblemActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_problem);

        MainActivity.dotMatrix.stop();

        TextView timer = findViewById(R.id.timerText);

        new CountDownTimer(4000, 1000) {

            public void onTick(long millisUntilFinished) {
                timer.setText("Timer " + millisUntilFinished / 1000);
            }

            public void onFinish() {
                timer.setText("done!");
            }
        }.start();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(ProblemActivity.this, GameOverActivity.class);
                startActivity(intent);
            }
        },3000);


    }
}