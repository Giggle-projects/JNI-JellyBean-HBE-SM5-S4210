package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.segmentControl;

public class Segment {

    private final Thread thread;
    private int printData;

    public Segment() {
        thread = new Thread(new PrintSegmentRunnable());
        thread.start();
    }

    public void print(int i) {
        this.printData = i;
    }

    public void stop() {
        this.printData = 0;
    }

    private class PrintSegmentRunnable implements Runnable {

        @Override
        public void run() {
            while (true) {
                segmentControl(printData);
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {

                }
            }
        }
    }
}

