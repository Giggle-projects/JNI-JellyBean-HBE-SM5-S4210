package com.example.myapplication.hw;

import static com.example.myapplication.GlobalNative.piezoControl;

import android.util.Log;

import java.util.LinkedList;
import java.util.List;

public class Piezo {

    private final List<Integer> sounds = new LinkedList<>();
    private final List<Integer> beats = new LinkedList<>();

    public Piezo() {
        sounds.clear();
        beats.clear();

        new Thread(() -> {
            while (true) {
                try {
                    if (sounds.isEmpty() || beats.isEmpty()) {
                        piezoControl((char) 0);
                        continue;
                    }

                    final int sound = sounds.get(0);
                    final int beat = beats.get(0);
                    piezoControl((char) sound);
                    Thread.sleep(beat);
                    sounds.remove(0);
                    beats.remove(0);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public void out(int sound, int beat) {
        this.sounds.add(sound);
        this.beats.add(beat);
    }
}
