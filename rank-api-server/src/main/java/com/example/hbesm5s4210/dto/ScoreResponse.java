package com.example.hbesm5s4210.dto;

import com.example.hbesm5s4210.domain.HighScore;

import java.util.ArrayList;
import java.util.List;

public class ScoreResponse {

    private final int rank;
    private final String name;
    private final int score;

    public ScoreResponse(int rank, String name, int score) {
        this.rank = rank;
        this.name = name;
        this.score = score;
    }

    public static ScoreResponse of(int rank, HighScore score) {
        return new ScoreResponse(
                rank,
                score.getName(),
                score.getScore()
        );
    }

    public static List<ScoreResponse> listOf(List<HighScore> scores) {
        final List<ScoreResponse> responses = new ArrayList<>();
        int rank =1;
        for(HighScore highScore : scores) {
            responses.add(ScoreResponse.of(rank++, highScore));
        }
        return responses;
    }

    public int getRank() {
        return rank;
    }

    public String getName() {
        return name;
    }

    public int getScore() {
        return score;
    }
}
