package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.piezoControl;
import static com.example.myapplication.GlobalNative.printDotMatrix;
import static com.example.myapplication.GlobalNative.segmentControl;

public class Buzzer {

    private final Thread thread;
    private int pitch = 0;
    private int time = 0;

    public Buzzer() {
        thread = new Thread(new PrintBuzzerRunnable());
        thread.start();
    }

    public void print(int pitch, int time) {
        this.pitch = pitch;
        this.time = time;
    }

    private class PrintBuzzerRunnable implements Runnable {

        @Override
        public void run() {
            while (true) {
                try {
                    piezoControl(pitch);
                    Thread.sleep(time);
                    pitch = 0;
                } catch (InterruptedException e) {

                }
            }
        }
    }
}
