package com.example.myapplication;

import static com.example.myapplication.GlobalNative.*;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;

import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.hw.DotMatrix;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (event.getKeyCode() == 145) {
            Log.d("KeyUP Event", event.getKeyCode() + "");
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        DotMatrix.changeMessage("HelloCome");
        DotMatrix.run();

        Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
            DotMatrix.stop();
            Intent intent = new Intent(MainActivity.this, ProblemActivity.class);
            startActivity(intent);
        });
    }
}

