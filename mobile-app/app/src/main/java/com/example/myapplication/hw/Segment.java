package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.segmentControl;

import android.os.SystemClock;

public class Segment {

    private Thread thread;
    private int printData;

    public Segment() {
        this.thread = new Thread(() -> {
            while (true) {
                segmentControl(printData);
                SystemClock.sleep(0);
            }
        });
        this.thread.start();
    }

    public void print(int i) {
        this.printData = i;
    }

    public void stop() {
        this.printData = 0;
    }
}
