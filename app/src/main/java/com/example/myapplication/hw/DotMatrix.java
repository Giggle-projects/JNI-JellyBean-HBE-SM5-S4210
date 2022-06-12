package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.printDotMatrix;

public class DotMatrix {

    private final Thread thread;
    private String msg = "";

    public DotMatrix() {
        thread = new Thread(new PrintDotMatrixRunnable());
        thread.start();
    }

    public void changeMessage(String msg) {
        this.msg = msg;
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

