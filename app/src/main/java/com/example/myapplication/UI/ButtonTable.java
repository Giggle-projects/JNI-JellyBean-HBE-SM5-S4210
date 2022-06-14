package com.example.myapplication.UI;

import android.graphics.Color;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

import com.example.myapplication.R;

import java.util.HashMap;
import java.util.Map;

public class ButtonTable {

    public static final int[] COLORS = {Color.BLACK, Color.BLUE, Color.CYAN, Color.DKGRAY, Color.GRAY, Color.GREEN, Color.LTGRAY, Color.MAGENTA, Color.RED, Color.YELLOW,};

    private final Map<Integer, Button> buttonTable;

    public static ButtonTable initFromActivity(AppCompatActivity appCompatActivity){
        final Map<Integer, Button> buttonTable = new HashMap<>();
        buttonTable.put(0, appCompatActivity.findViewById(R.id.but1));
        buttonTable.put(1, appCompatActivity.findViewById(R.id.but2));
        buttonTable.put(2, appCompatActivity.findViewById(R.id.but3));
        buttonTable.put(3, appCompatActivity.findViewById(R.id.but4));
        buttonTable.put(4, appCompatActivity.findViewById(R.id.but5));
        buttonTable.put(5, appCompatActivity.findViewById(R.id.but6));
        buttonTable.put(6, appCompatActivity.findViewById(R.id.but7));
        buttonTable.put(7, appCompatActivity.findViewById(R.id.but8));
        buttonTable.put(8, appCompatActivity.findViewById(R.id.but9));
        buttonTable.put(9, appCompatActivity.findViewById(R.id.but10));
        buttonTable.put(10, appCompatActivity.findViewById(R.id.but11));
        buttonTable.put(11, appCompatActivity.findViewById(R.id.but12));
        buttonTable.put(12, appCompatActivity.findViewById(R.id.but13));
        buttonTable.put(13, appCompatActivity.findViewById(R.id.but14));
        buttonTable.put(14, appCompatActivity.findViewById(R.id.but15));
        buttonTable.put(15, appCompatActivity.findViewById(R.id.but16));
        return new ButtonTable(buttonTable);
    }

    private ButtonTable(Map<Integer, Button> buttonTable) {
        this.buttonTable = buttonTable;
    }

    public void setColor(int position, int colorIndex) {
        final int colorCode = COLORS[colorIndex % COLORS.length];
        buttonTable.get(position).setBackgroundColor(colorCode);
    }

    public void setText(int position, String text) {
        buttonTable.get(position).setText(text);
    }
}
