package com.example.myapplication.domain;


public class Score {

    private int value;

    public Score(int value) {
        this.value = value;
    }

    public void increase() {
        value += 10;
    }

    public int print() {
        return value;
    }
}
