package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.segmentControl;

public class Segment {

    private Thread thread;
    private int printData;

    public void print(int i) {
        thread = new Thread(new PrintSegmentRunnable());
        thread.start();
        this.printData = i;
    }

    public void stop() {
        this.printData = 0;
        thread.interrupt();
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

