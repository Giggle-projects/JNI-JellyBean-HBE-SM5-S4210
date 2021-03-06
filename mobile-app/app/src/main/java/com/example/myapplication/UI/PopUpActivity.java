package com.example.myapplication.UI;

import android.content.Intent;
import android.os.Bundle;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;

import com.example.myapplication.R;
import com.example.myapplication.api.RankApi;
import com.example.myapplication.domain.InMemoryDB;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.TextLCD;

public class PopUpActivity extends AppCompatActivity {

    TextLCD textLCD = HwContainer.textLcd;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pop_up);

        textLCD.print("### Ranking ###", "Register Page");

        final EditText editText = findViewById(R.id.nickEdtTxt);
        final Button regBtn = findViewById(R.id.registerBtn);
        regBtn.setOnClickListener(view -> {
            RankApi.uploadRank(editText.getText().toString(), InMemoryDB.getScore().value());
            textLCD.stop();
            startActivity(new Intent(PopUpActivity.this, MainActivity.class));
            finish();
        });
    }
}