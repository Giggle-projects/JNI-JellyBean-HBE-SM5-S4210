package com.example.myapplication.domain;

import static com.example.myapplication.GlobalNative.ledControl;

public class Life {

    public static final int INITIAL_LIFE_POINT = 3;

    private int value;

    public Life() {
        this(INITIAL_LIFE_POINT);
    }

    public Life(int value) {
        this.value = value;
    }

    public boolean isOver() {
        return value < 1;
    }

    public void lose() {
        value--;
        printLed();
    }

    public int value() {
        return value;
    }

    public void printLed() {
        ledControl(bitCount());
    }

    private int bitCount() {
        return (0b11111111 >> (8 - value)) << (8 - value);
    }
}
