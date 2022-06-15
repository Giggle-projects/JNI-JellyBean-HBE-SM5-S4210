package com.example.myapplication.UI;

import static com.example.myapplication.GlobalNative.segmentControl;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.Vibrator;
import android.view.KeyEvent;

import com.example.myapplication.R;
import com.example.myapplication.domain.Answer;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.domain.Life;
import com.example.myapplication.domain.Score;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.FullColorLed;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.Piezo;
import com.example.myapplication.hw.TextLCD;

public class GameActivity extends AppCompatActivity {

    private static final long MIN_CLICK_INTERVAL = 300;
    private long mLastClickTime;

    private final Answer answer = InMemoryDB.getAnswer();
    private int stageScore = 0;

    private final TextLCD textLCD = HwContainer.textLcd;
    private final Keypad keypad = HwContainer.keypad;
    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Vibrator vibrator = HwContainer.vibrator;
    private final Piezo piezo = HwContainer.piezo;
    private final FullColorLed fullColorLed = HwContainer.fullColorLed;

    private final Life life = InMemoryDB.getLife();
    private final Level level = InMemoryDB.getLevel();
    private final Score score = InMemoryDB.getScore();
    private ButtonTable buttonTable;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        final long currentClickTime = SystemClock.uptimeMillis();
        final long elapsedTime = currentClickTime - mLastClickTime;
        mLastClickTime = currentClickTime;
        if (elapsedTime <= MIN_CLICK_INTERVAL) {
            return false;
        }

        final int bottonPosition = keypad.getPositionOfKeyEvent(event);
        if (checkCorrect(event)) {
            buttonTable.setColor(bottonPosition, stageScore);;
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
            this.buttonTable.setText(bottonPosition, "X");
            new Thread(()-> vibrator.vibrate(500)).start();
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
        return answer.isCorrect(life, stageScore, userInput);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
        textLCD.print("### level " + level.value() + " ###", "Game Start");
        fullColorLed.on(5, 0, 10);
        fullColorLed.off(7);
        buttonTable = ButtonTable.initFromActivity(this);
    }
}
