package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.fullColorLedControl;

public class FullColorLed {

    public void on(int red, int green, int blue) {
        fullColorLedControl(9, red, green, blue);
        fullColorLedControl(8, red, green, blue);
        fullColorLedControl(7, red, green, blue);
        fullColorLedControl(6, red, green, blue);
        fullColorLedControl(5, red, green, blue);
    }

    public void on(int ledNum, int red, int green, int blue) {
        fullColorLedControl(9, red, green, blue);
    }

    public void off(int ledNum) {
        fullColorLedControl(ledNum, 0, 0, 0);
    }
}
