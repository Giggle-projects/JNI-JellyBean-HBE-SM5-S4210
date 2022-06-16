package com.example.hbesm5s4210.domain;

import java.util.HashMap;
import java.util.Map;

public class InmemoryDB {

    public static final Map<Integer, Integer> scoreTable = new HashMap<>();

    public static int getScore(int key) {
        return scoreTable.get(key);
    }

    public static void setScore(int key, int score) {
        scoreTable.put(key, score);
    }
}
