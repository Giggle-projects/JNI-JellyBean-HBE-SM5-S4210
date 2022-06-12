package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.printDotMatrix;

import com.example.myapplication.MainActivity;

public class DotMatrix {

    private static boolean dotMatrixPrinting = false;
    private static String msg = "";
    private static Thread thread = new Thread(new PrintDotMatrixRunnable());

    static {
        thread.start();
    }

    public static void stop() {
        dotMatrixPrinting = false;
    }

    public static void run() {
        dotMatrixPrinting = true;
    }

    public static void changeMessage(String newMsg) {
        msg = newMsg;
    }

    private static class PrintDotMatrixRunnable implements Runnable {

        @Override
        public void run() {
            while (dotMatrixPrinting) {
                printDotMatrix(msg, 20);
            }
        }
    }
}

