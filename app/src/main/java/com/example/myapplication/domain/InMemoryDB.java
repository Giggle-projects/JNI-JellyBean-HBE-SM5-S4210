package com.example.myapplication.domain;

public class InMemoryDB {

    private static Level level = new Level(1);
    private static Life life = new Life(3);

    public static Level getLevel() {
        return level;
    }

    public static Life getLife() {
        return life;
    }
}
