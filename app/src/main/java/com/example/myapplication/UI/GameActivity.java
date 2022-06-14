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
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Keypad;
import com.example.myapplication.hw.TextLCD;

public class GameActivity extends AppCompatActivity {

    private int[] answerFromServer = new int[]{1, 3, 4, 6, 7};
    private int stageScore = 0;
    private int stage = 5;

    private TextLCD textLCD = HwContainer.textLcd;
    private Level level = InMemoryDB.getLevel();
    private Keypad keypad = HwContainer.keypad;
    private DotMatrix dotMatrix = HwContainer.dotMatrix;

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        Log.d("key down event", keypad.getPositionOfCode(event.getKeyCode()) + "");

        if (answerFromServer[stageScore] == keypad.getPositionOfKeyEvent(event)) {
            dotMatrix.print("Correct", 5, 1);
            Log.d("tag", "correct!!");
            stageScore++;
            if (stageScore == level.print()) {
                level.next();
                Intent intent = new Intent(GameActivity.this, ProblemActivity.class);
                startActivity(intent);
            }
        } else {
            dotMatrix.print("Wrong", 3, 1);
            Log.d("tag", "wrong!!");
            Intent intent = new Intent(GameActivity.this, ProblemActivity.class);
            startActivity(intent);
            //if(life ==0) {
            //    // 종료 페이지
            //} else {
            //    // 다음 페이지로
            //    life--
            //    Intent intent = new Intent(GameActivity.this, ProblemActivity.class);
            //    startActivity(intent);
            //}
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