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
        printDotMatrix("", 20);
        thread.interrupt();
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

