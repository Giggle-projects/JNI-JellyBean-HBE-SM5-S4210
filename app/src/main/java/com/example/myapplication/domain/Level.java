package com.example.myapplication.domain;

public class Level {

    private int value;

    public Level(int value) {
        this.value = value;
    }

    public int print() {
        return value;
    }

    public void next() {
        this.value++;
    }
}
