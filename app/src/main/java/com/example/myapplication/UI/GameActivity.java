package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;

import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Level;
import com.example.myapplication.domain.Life;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.TextLCD;

public class GameActivity extends AppCompatActivity {

    private int[] answerFromServer = new int[]{1};
    private int stageScore = 0;
    private int stage = 5;

    private TextLCD textLCD = HwContainer.textLcd;
    private Life life = InMemoryDB.getLife();
    private Level level = InMemoryDB.getLevel();
    private Keypad keypad = HwContainer.keypad;
    private DotMatrix dotMatrix = HwContainer.dotMatrix;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (answerFromServer[stageScore] == keypad.getPositionOfKeyEvent(event)) {
            dotMatrix.print("Correct", 5, 1);
            stageScore++;
            if (stageScore == level.print()) {
                level.next();
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
            }
        } else {
            dotMatrix.print("Wrong", 5, 1);
            if (life.isOver()) {
                startActivity(new Intent(GameActivity.this, ProblemActivity.class));
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