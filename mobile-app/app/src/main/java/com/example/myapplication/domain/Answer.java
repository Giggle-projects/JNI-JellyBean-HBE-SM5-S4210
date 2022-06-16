package com.example.myapplication.domain;

import java.util.LinkedList;
import java.util.List;

public class Answer {

    private List<Integer>[] answersList = new List[Life.INITIAL_LIFE_POINT + 1];

    public Answer() {
        for (int i = 0; i < answersList.length; i++) {
            answersList[i] = new LinkedList<>();
        }
    }

    public void add(Life life, int answer) {
        final int lifeValue = life.value();
        if (lifeValue > Life.INITIAL_LIFE_POINT) {
            throw new IllegalArgumentException("Maximum life is " + Life.INITIAL_LIFE_POINT);
        }
        answersList[lifeValue].add(answer);
    }

    public boolean isCorrect(Life life, int stageScore, int userInput) {
        final int lifeValue = life.value();
        if (lifeValue > Life.INITIAL_LIFE_POINT) {
            throw new IllegalArgumentException("Maximum life is " + Life.INITIAL_LIFE_POINT);
        }
        if (stageScore > answersList[lifeValue].size()) {
            throw new IllegalArgumentException("Maximum stageScore is " + answersList[lifeValue].size());
        }
        return answersList[lifeValue].get(stageScore).equals(userInput);
    }
}
