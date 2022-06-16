package com.example.myapplication.domain;

public class Rank {

    private final int rank;
    private final String name;
    private final int score;

    public Rank(int rank, String name, int score) {
        this.rank = rank;
        this.name = name;
        this.score = score;
    }

    public int rank() {
        return rank;
    }

    public String name() {
        return name;
    }

    public int score() {
        return score;
    }

    public String toString() {
        String rankMark;
        if (rank % 10 == 1) {
            rankMark = "ST";
        } else if (rank % 10 == 2) {
            rankMark = "ND";
        } else if (rank % 10 == 3) {
            rankMark = "RD";
        } else {
            rankMark = "TH";
        }
        return String.format("%2d%s \t\t\t %06d점 \t\t %3.3s  ", rank, rankMark, score, name);
    }
}
