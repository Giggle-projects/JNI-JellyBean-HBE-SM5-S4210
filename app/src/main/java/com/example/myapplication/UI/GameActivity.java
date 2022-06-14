package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.Vibrator;
import android.util.Log;
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
import com.example.myapplication.hw.Piezo;
import com.example.myapplication.hw.TextLCD;

import java.util.Random;

public class GameActivity extends AppCompatActivity {

    private static final long MIN_CLICK_INTERVAL=600;
    private long mLastClickTime;

    private final Answer answer = InMemoryDB.getAnswer();
    private int stageScore = 0;

    private final TextLCD textLCD = HwContainer.textLcd;
    private final Keypad keypad = HwContainer.keypad;
    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Vibrator vibrator = HwContainer.vibrator;
    private final Piezo piezo = HwContainer.piezo;

    private final Life life = InMemoryDB.getLife();
    private final Level level = InMemoryDB.getLevel();
    private final Score score = InMemoryDB.getScore();
    private final Random random = new Random();

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        final long currentClickTime = SystemClock.uptimeMillis();
        final long elapsedTime = currentClickTime - mLastClickTime;
        mLastClickTime = currentClickTime;

        if (elapsedTime <= MIN_CLICK_INTERVAL) {
            return false;
        }

        if(checkCorrect(event)) {
            piezo.out(1, 100);
            dotMatrix.print("Correct", 5, 1);
            stageScore++;
            score.increase(level);
            if (level.done(stageScore)) {
                level.next();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
                finish();
            }
        } else {
            vibrator.vibrate(500);
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

    private boolean checkCorrect(KeyEvent event) {
        final int userInput = keypad.getPositionOfKeyEvent(event);
        return answer.isCorrect(life,stageScore, userInput);
    }

    private void getNewAnswer() {
        int num = random.nextInt(16);
        answer.add(life, num);
        Log.d("tag", String.valueOf(num));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
        textLCD.print("### level " + level.value() + " ###", "Game Start");
        getNewAnswer();

    }
}