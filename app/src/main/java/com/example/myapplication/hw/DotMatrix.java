package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.printDotMatrix;

public class DotMatrix {

    private Thread thread;
    private String msg = "";
    private int speed = 20;
    private int printCount = 3;

    public DotMatrix() {
        thread = new Thread(() -> {
            while (true) {
                if (printCount > 0) {
                    printCount--;
                    printDotMatrix(msg, speed);
                }
            }
        });
        thread.start();
    }

    public void print(String msg, int speed, int printCount) {
        this.printCount = printCount;
        this.msg = msg;
        this.speed = speed;
    }

    public void print(String msg) {
        print(msg, 20, 1);
    }

    public void stop() {
        this.printCount = 0;
        this.msg = "";
    }
}

