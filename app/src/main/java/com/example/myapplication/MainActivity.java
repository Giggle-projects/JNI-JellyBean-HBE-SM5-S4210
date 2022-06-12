package com.example.myapplication;

import static android.os.SystemClock.sleep;

import static com.example.myapplication.GlobalNative.piezoControl;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Button;

import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.hw.Buzzer;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.LED;
import com.example.myapplication.hw.Segment;
import com.example.myapplication.hw.TextLCD;

import java.time.LocalDate;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    private final DotMatrix dotMatrix = HwContainer.dotMatrix;
    private final Segment segment = HwContainer.segment;
    private final TextLCD textLcd = HwContainer.textLcd;
    private final LED led = HwContainer.led;
    private final Buzzer buzzer = HwContainer.buzzer;

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
        dotMatrix.changeMessage("WelCome");
        led.printLinear();
        segment.print(220613);
        buzzer.print(1, 10);
        buzzer.print(100, 10);
        buzzer.print(0, 10);

        Button startBtn = findViewById(R.id.startBtn);
        startBtn.setOnClickListener(view -> {
                dotMatrix.changeMessage("");
                segment.stop();
                buzzer.print(0, 0);
                Intent intent = new Intent(MainActivity.this, ProblemActivity.class);
                startActivity(intent);
        });
    }
}

