package com.example.myapplication.domain;


public class Score {

    private int score;

    public void increase() {
        score = score + 10;
    }

    public int print() {
        return score;
    }
}
