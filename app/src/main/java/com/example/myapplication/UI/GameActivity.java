package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;


import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.domain.Life;
import com.example.myapplication.domain.Score;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.Segment;
import com.example.myapplication.hw.TextLCD;

public class GameActivity extends AppCompatActivity {

    private final int[] answerFromServer = new int[]{1,2,3,4,5};
    private int stageScore = 0;
    private int stage = 5;

    private final TextLCD textLCD = HwContainer.textLcd;
    private final Keypad keypad = HwContainer.keypad;
    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Segment segment = HwContainer.segment;
    private final Life life = InMemoryDB.getLife();
    private final Level level = InMemoryDB.getLevel();
    private final Score score = InMemoryDB.getScore();

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (answerFromServer[stageScore] == keypad.getPositionOfKeyEvent(event)) {
            dotMatrix.print("Correct", 5, 1);
            stageScore++;
            score.increase();
            segment.print(score.print());

            if (stageScore == level.print()) {
                level.next();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
            }
        } else {
            dotMatrix.print("Wrong", 5, 1);
            if (life.isOver()) {
                startActivity(new Intent(GameActivity.this, GameOverActivity.class));
                segment.stop();
            } else {
                life.lose();
                life.printLed();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
            }
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
        textLCD.print("### level " + level.print() + " ###", "Game Start");
    }
}