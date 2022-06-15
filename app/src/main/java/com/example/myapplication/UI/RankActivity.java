package com.example.myapplication.UI;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.example.myapplication.api.RankApi;
import com.example.myapplication.databinding.ActivityRankBinding;
import com.example.myapplication.domain.Rank;
import com.example.myapplication.hw.DotMatrix;
import com.example.myapplication.hw.FullColorLed;
import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Segment;
import com.example.myapplication.hw.TextLCD;

import java.util.List;
import java.util.concurrent.ExecutionException;


public class RankActivity extends AppCompatActivity {

    private ActivityRankBinding binding;
    TextLCD textLCD = HwContainer.textLcd;
    DotMatrix dotMatrix = HwContainer.dotMatrix;
    Segment segment = HwContainer.segment;
    FullColorLed fullColorLed = HwContainer.fullColorLed;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityRankBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        textLCD.print("# Ranking Page #", "the Hall of Fame");
        fullColorLed.on(9,10,0,0);
        fullColorLed.on(8,0,10,0);
        fullColorLed.on(7,0,0,10);
        fullColorLed.on(6,10,10,10);

        try {
            final List<Rank> ranks = RankApi.findAll();
            if(ranks.isEmpty()) {
                return;
            }
            dotMatrix.print(ranks.get(0).name(),10, Integer.MAX_VALUE);
            segment.print(ranks.get(0).score());
            binding.R1.setText(ranks.get(0).toString());
            binding.R2.setText(ranks.get(1).toString());
            binding.R3.setText(ranks.get(2).toString());
            binding.R4.setText(ranks.get(3).toString());
            binding.R5.setText(ranks.get(4).toString());
            binding.R6.setText(ranks.get(5).toString());
            binding.R7.setText(ranks.get(6).toString());
            binding.R8.setText(ranks.get(7).toString());
            binding.R9.setText(ranks.get(8).toString());
            binding.R10.setText(ranks.get(9).toString());
        } catch (IndexOutOfBoundsException e) {
           e.printStackTrace();
            Log.d("error", e.getMessage());
        } catch (ExecutionException e) {
            e.printStackTrace();
            Log.d("error", e.getMessage());
        } catch (InterruptedException e) {
            e.printStackTrace();
            Log.d("error", e.getMessage());
        }
    }
}