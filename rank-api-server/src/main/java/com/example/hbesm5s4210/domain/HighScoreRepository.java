package com.example.hbesm5s4210.domain;

import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface HighScoreRepository extends JpaRepository<HighScore, Long> {

    Page<HighScore> findAll(Pageable pageable);
}
