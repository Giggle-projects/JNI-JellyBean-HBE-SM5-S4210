package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;

import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.TextLCD;

import java.util.concurrent.atomic.AtomicInteger;

public class GameActivity extends AppCompatActivity {

    private int[] answerFromServer = new int[]{1, 3, 4, 6, 7};
    private int score = 0;
    private int stage = 5;

    TextLCD textLCD = HwContainer.textLcd;

    final private Level level = InMemoryDB.getLevel();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);

        textLCD.print("### level " + level.print() + " ###", "Game Start");

        Button btn1 = findViewById(R.id.but1);
        btn1.setOnClickListener(view -> {
            level.next();
            Intent intent = new Intent(GameActivity.this, ProblemActivity.class);
            startActivity(intent);
        });




//
//        btn.setOnClickListener(view -> {
//            if(btn.getId()-100007 == answerFromServer[score]) {
//                score++;
//            } else {
//                // 하트 감소
//            }
//
//            if(score == stage) {
//                // next stage
//            }
//        });
    }
}