package com.example.myapplication.domain;

public class InMemoryDB {

    private static Level level = new Level(1);
    private static Life life = new Life(3);
    private static Score score = new Score(0);

    public static void init() {
        level = new Level(1);
        life = new Life(3);
        score = new Score(0);
    }

    public static Level getLevel() {
        return level;
    }

    public static Life getLife() {
        return life;
    }

    public static Score getScore() { return score;}
}
