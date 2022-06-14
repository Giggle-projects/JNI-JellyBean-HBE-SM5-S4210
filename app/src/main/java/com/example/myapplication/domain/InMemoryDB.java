package com.example.myapplication.domain;

public class InMemoryDB {

    private static Level level;
    private static Life life;
    private static Score score;
    private static Answer answer;

    public static void init() {
        level = new Level();
        life = new Life();
        score = new Score();
        answer = new Answer();
    }

    public static Level getLevel() {
        return level;
    }

    public static Life getLife() {
        return life;
    }

    public static Score getScore() { return score;}

    public static Answer getAnswer() {
        return answer;
    }
}
