package com.example.hbesm5s4210;

import com.example.hbesm5s4210.domain.HighScore;
import com.example.hbesm5s4210.domain.HighScoreRepository;
import com.example.hbesm5s4210.domain.InmemoryDB;
import com.example.hbesm5s4210.dto.ScoreResponse;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.data.domain.Sort;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/*
GET :: /level/{level}?key={key}
POST :: /level/{level}?key={key} {
  answer : [answers]
}
GET :: /score
POST :: /score?key={key}&name={name}
 */

@RestController
public class GameController {

    private final HighScoreRepository highScoreRepository;

    public GameController(HighScoreRepository highScoreRepository) {
        this.highScoreRepository = highScoreRepository;
    }

    @GetMapping("/score")
    public ResponseEntity<List<ScoreResponse>> getHighScores() {
        final List<HighScore> scores = getHighScores(10);
        return ResponseEntity.ok(ScoreResponse.listOf(scores));
    }

    @PostMapping("/score")
    public ResponseEntity<Void> enrollHighScore(int key, String name) {
        final int score = InmemoryDB.getScore(key);
        highScoreRepository.save(new HighScore(name, score));
        deleteUnderRankFrom(11);
        return ResponseEntity.ok().build();
    }

    @PostMapping("/score/rank")
    public ResponseEntity<Void> enrollRank(String name, int score) {
        highScoreRepository.save(new HighScore(name, score));
        deleteUnderRankFrom(11);
        return ResponseEntity.ok().build();
    }

    @GetMapping("/score/rank")
    public ResponseEntity<Integer> rank(int score) {
        final int rankIndex = findRankOfScore(score);
        return ResponseEntity.ok(rankIndex + 1);
    }

    private int findRankOfScore(int score) {
        final List<Integer> highScores = getHighScores(10).stream()
                .map(HighScore::getScore)
                .collect(Collectors.toList());
        return findRankIndex(highScores, score);
    }

    private int findRankIndex(List<Integer> highScores, int targetScore) {
        highScores.sort((o1, o2) -> o2 - o1);
        int rank = 0;
        for (int highScore : highScores) {
            if (targetScore > highScore) {
                return rank;
            }
            rank++;
        }
        return rank;
    }

    private List<HighScore> getHighScores(int size) {
        final Sort sort = Sort.by("score").descending();
        final Pageable pageable = PageRequest.of(0, size, sort);
        return highScoreRepository.findAll(pageable).getContent();
    }

    private void deleteUnderRankFrom(int minimumRank) {
        final List<HighScore> ranks = new ArrayList<>(highScoreRepository.findAll());
        ranks.sort((o1, o2) -> o2.getScore() - o1.getScore());
        for (int i = minimumRank - 1; i < ranks.size(); i++) {
            highScoreRepository.delete(ranks.get(i));
        }
    }
}
