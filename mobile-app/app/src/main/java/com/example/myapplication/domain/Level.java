package com.example.myapplication.domain;

public class Level {

    private int value;

    public Level() {
        this(1);
    }

    public Level(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    public void next() {
        this.value++;
    }

    public boolean done(int stageScore) {
        return value == stageScore;
    }

    public void reset() {
        this.value = 1;
    }
}
