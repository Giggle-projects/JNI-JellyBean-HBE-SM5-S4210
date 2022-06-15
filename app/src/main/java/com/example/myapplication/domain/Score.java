package com.example.myapplication.domain;

import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Segment;

public class Score {
    private static final int MAX_SCORE= 999999;
    private static final Segment segment = HwContainer.segment;

    private int value;

    public Score() {
        this(0);
    }

    public Score(int value) {
        this.value = value;
        if(value > MAX_SCORE) {
            this.value = MAX_SCORE;
        }
    }

    public void increase(Level level) {
        value += (10 * level.value());
        if(value > MAX_SCORE) {
            value = MAX_SCORE;
        }
        printSegment();
    }

    public int value() {
        return value;
    }

    public void printSegment() {
        segment.print(value);
    }
}
