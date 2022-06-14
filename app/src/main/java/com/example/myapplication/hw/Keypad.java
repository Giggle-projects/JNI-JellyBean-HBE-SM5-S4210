package com.example.myapplication.hw;

import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.Button;

import com.example.myapplication.R;

import java.util.HashMap;
import java.util.Map;

public class Keypad {

    public static int[][] KEY_CODES = new int[][]{
            {44, 43, 37, 33},
            {45, 46, 48, 51},
            {53, 7, 8, 9},
            {10, 11, 12, 13}
    };

    private static Map<Integer, Integer> codeTable = new HashMap<>();

    static {
        for (int i = 0; i < KEY_CODES.length; i++) {
            for (int j = 0; j < KEY_CODES[0].length; j++) {
                codeTable.put(KEY_CODES[i][j], i * 4 + j);
            }
        }
    }

    public int getPositionOfCode(int keycode) {
        return codeTable.get(keycode);
    }


    public int getPositionOfKeyEvent(KeyEvent keyEvent) {
        return getPositionOfCode(keyEvent.getKeyCode());
    }
}
