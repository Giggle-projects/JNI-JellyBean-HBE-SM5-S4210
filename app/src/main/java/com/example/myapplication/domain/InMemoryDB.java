package com.example.myapplication.domain;

public class InMemoryDB {

    private static Level level = new Level(0);

    public static Level getLevel() {
        return level;
    }
}
