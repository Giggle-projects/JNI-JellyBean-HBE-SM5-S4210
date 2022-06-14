package com.example.myapplication.domain;

public class InMemoryDB {

    private static final Level level = new Level(1);
    private static final Life life = new Life(3);
    private static final Score score = new Score();

    public static Level getLevel() {
        return level;
    }

    public static Life getLife() {
        return life;
    }

    public static Score getScore() { return score;}
}
