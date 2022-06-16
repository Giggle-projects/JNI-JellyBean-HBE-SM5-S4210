package com.example.hbesm5s4210;

import com.example.hbesm5s4210.domain.HighScoreRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;

@SpringBootApplication
public class HbeSm5S4210Application {

    public static void main(String[] args) {
        SpringApplication.run(HbeSm5S4210Application.class, args);
    }
}

@Component
class TestDummy {

    @Autowired
    private GameController gameController;

    @PostConstruct
    public void setUp() {
        gameController.enrollRank("A", 1);
        gameController.enrollRank("B", 2);
        gameController.enrollRank("C", 3);
        gameController.enrollRank("D", 4);
        gameController.enrollRank("E", 5);
        gameController.enrollRank("F", 61);
        gameController.enrollRank("G", 7);
        gameController.enrollRank("H", 8);
        gameController.enrollRank("I", 9);
        gameController.enrollRank("J", 10);
    }
}
