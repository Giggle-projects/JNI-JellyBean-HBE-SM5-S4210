package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.printDotMatrix;

public class DotMatrix {

    private Thread thread;
    private String msg = "";

    public void print(String msg) {
        thread = new Thread(new PrintDotMatrixRunnable());
        thread.start();
        this.msg = msg;
    }

    public void stop() {
        this.msg = "";
        try {
            thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private class PrintDotMatrixRunnable implements Runnable {

        @Override
        public void run() {
            while (true) {
                printDotMatrix(msg, 20);
            }
        }
    }
}

