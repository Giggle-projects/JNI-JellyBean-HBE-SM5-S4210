package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.*;

public class TextLCD {

    public void startTextLCD(String line1, String line2) {
        lcdClear();
        lcdPrint(0, line1);
        lcdPrint(1, line2);
    }
}
