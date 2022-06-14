package com.example.myapplication.domain;

import static com.example.myapplication.GlobalNative.ledControl;

public class Life {

    private int value;

    public Life(int value) {
        this.value = value;
    }

    public boolean isOver() {
        return value <= 0;
    }

    public void lose() {
        value--;
    }

    public void gain() {
        value++;
    }

    public void printLed() {
        ledControl(bitCount());
    }

    private int bitCount() {
        return (0b11111111 >> (8 - value)) << (8 - value);
    }
}
