package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;

import com.example.myapplication.R;
import com.example.myapplication.domain.Answer;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.domain.Life;
import com.example.myapplication.domain.Score;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.TextLCD;

public class GameActivity extends AppCompatActivity {

    private final Answer answer = InMemoryDB.getAnswer();
    private int stageScore = 0;

    private final TextLCD textLCD = HwContainer.textLcd;
    private final Keypad keypad = HwContainer.keypad;
    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Life life = InMemoryDB.getLife();
    private final Level level = InMemoryDB.getLevel();
    private final Score score = InMemoryDB.getScore();

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        final int userInput = keypad.getPositionOfKeyEvent(event);
        if(answer.isCorrect(life,stageScore, userInput)) {
            dotMatrix.print("Correct", 5, 1);
            stageScore++;
            score.increase(level);
            if (level.done(stageScore)) {
                level.next();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
                finish();
            }
        } else {
            dotMatrix.print("Wrong", 5, 1);
            life.lose();
            level.reset();
            if (life.isOver()) {
                startActivity(new Intent(GameActivity.this, GameOverAcitivity.class));
                finish();
            } else {
                life.printLed();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
                finish();
            }
        }
        return true;
    }

    private void getNewAnswerFormServer() {
        // get new answer from server
        answer.add(life, 1);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
        textLCD.print("### level " + level.value() + " ###", "Game Start");
        getNewAnswerFormServer();
    }
}