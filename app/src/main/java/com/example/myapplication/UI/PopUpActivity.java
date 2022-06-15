package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;

import com.example.myapplication.R;
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

        textLCD.print("### Ranking ###","Register Page");

        EditText editText = findViewById(R.id.nickEdtTxt);


        Button regBtn = findViewById(R.id.registerBtn);
        regBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.d("nickname", editText.getText().toString());
                textLCD.stop();
                startActivity(new Intent(PopUpActivity.this, MainActivity.class));
                finish();
            }
        });
    }
}