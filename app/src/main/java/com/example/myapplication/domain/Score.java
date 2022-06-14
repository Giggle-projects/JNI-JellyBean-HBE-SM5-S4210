package com.example.myapplication.domain;

import com.example.myapplication.hw.HwContainer;
import com.example.myapplication.hw.Segment;

public class Score {

    private static final Segment segment = HwContainer.segment;

    private int value;

    public Score() {
        this(0);
    }

    public Score(int value) {
        this.value = value;
    }

    public void increase(Level level) {
        value += (10 * level.value());
        printSegment();
    }

    public int value() {
        return value;
    }

    public void printSegment() {
        segment.print(value);
    }
}
