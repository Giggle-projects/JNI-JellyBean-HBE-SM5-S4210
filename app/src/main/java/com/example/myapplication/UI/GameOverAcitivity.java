package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Dialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import com.example.myapplication.R;
import com.example.myapplication.api.RankApi;
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
        textLCD.print("### Game Over ###", "f-score : " + score.value());
        fullColorLed.on(10, 0, 0);
        score.printSegment();

        Button yesBtn = findViewById(R.id.yesBtn);
        Button noBtn = findViewById(R.id.noBtn);
        TextView textView = findViewById(R.id.rktextView);

        if (RankApi.getRankOf(score.value()) > 10) {
            textView.setText("Do you want a restart\nthe Memory Game?");
            yesBtn.setOnClickListener(view -> {
                dotMatrix.stop();
                textLCD.stop();
                InMemoryDB.init();
                startActivity(new Intent(GameOverAcitivity.this, ProblemActivity.class));
                finish();
            });
        } else {
            textView.setText("Do you want to register\nfor the Rank?");
            yesBtn.setOnClickListener(view -> {
                dotMatrix.stop();
                textLCD.stop();
                startActivity(new Intent(GameOverAcitivity.this, PopUpActivity.class));
                finish();
            });
        }

        noBtn.setOnClickListener(view -> {
            dotMatrix.stop();
            textLCD.stop();
            startActivity(new Intent(GameOverAcitivity.this, MainActivity.class));
            finish();
        });
    }
}
