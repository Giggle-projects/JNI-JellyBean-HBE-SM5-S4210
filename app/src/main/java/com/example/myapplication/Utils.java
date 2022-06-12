package com.example.myapplication;

import android.util.Log;

public class Utils {

    private static final String SUCCEED_MESSAGE = "SUCCEED";

    public static void sleep(int l) {
        try {
            Thread.sleep(l);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public static void assertEqual(String tester, String expected) {
        if(!tester.equals(expected)) {
            Log.d("ERROR", "error in using component");
            throw new IllegalArgumentException("error in using component");
        }
    }

    public static void assertSucceed(String tester) {
        assertEqual(tester, SUCCEED_MESSAGE);
    }
}
