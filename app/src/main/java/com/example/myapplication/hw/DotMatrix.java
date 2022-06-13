package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.printDotMatrix;

public class DotMatrix {

    private Thread thread;
    private String msg = "";
    private int speed = 20;

    public DotMatrix() {
        thread = new Thread(() -> {
            while (true) {
                printDotMatrix(msg, speed);
            }
        });
        thread.start();
    }

    public void print(String msg, int speed) {
        this.msg = msg;
        this.speed = speed;
    }

    public void print(String msg) {
        print(msg,20);
    }

    public void stop() {
        this.msg = "";
    }
}

