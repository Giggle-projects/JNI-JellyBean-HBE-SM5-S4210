package com.example.myapplication.hw;

import android.os.Vibrator;

public class HwContainer {
    public static final DotMatrix dotMatrix = new DotMatrix();
    public static final Segment segment = new Segment();
    public static final TextLCD textLcd = new TextLCD();
    public static final LED led = new LED();
    public static final Keypad keypad = new Keypad();
    public static final Piezo piezo = new Piezo();
    public static final FullColorLed fullColorLed = new FullColorLed();
    public static Vibrator vibrator;
}
