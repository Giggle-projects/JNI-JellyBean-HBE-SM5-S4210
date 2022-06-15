package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;

import com.example.myapplication.R;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.domain.Score;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.FullColorLed;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.TextLCD;

public class GameOverAcitivity extends AppCompatActivity {

    DotMatrix dotMatrix = HwContainer.dotMatrix;
    TextLCD textLCD = HwContainer.textLcd;
    FullColorLed fullColorLed = HwContainer.fullColorLed;
    Score score = InMemoryDB.getScore();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gameover);

        dotMatrix.print("Finish", 10, Integer.MAX_VALUE);
        textLCD.print("### Game Over ###", "final score : " + score.value());
        fullColorLed.on(10, 0, 0);
        score.printSegment();

        Button yesBtn = findViewById(R.id.yesBtn);
        Button noBtn = findViewById(R.id.noBtn);

        yesBtn.setOnClickListener(view -> {
            dotMatrix.stop();
            textLCD.stop();
            startActivity(new Intent(GameOverAcitivity.this, MainActivity.class));
            finish();
        });

        noBtn.setOnClickListener(view -> {
            dotMatrix.stop();
            textLCD.stop();
            startActivity(new Intent(GameOverAcitivity.this, MainActivity.class));
            finish();
        });
    }
}
