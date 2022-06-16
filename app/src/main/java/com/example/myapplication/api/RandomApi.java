package com.example.myapplication.api;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Random;

public class RandomApi {

    private static final String END_POINT = "http://localhost:8080/";
    private static final Random random = new Random();

    public static Integer getRand() {
        AsyncTask<String, Void, Integer> asyncTask = new AsyncTask<String, Void, Integer>() {
            @Override
            protected Integer doInBackground(String... params) {
                try {
                    final URL endPoint = new URL(END_POINT);
                    final HttpURLConnection conn = (HttpURLConnection) endPoint.openConnection();
                    if (conn.getResponseCode() != 200) {
                        throw new IllegalArgumentException("Server connection error");
                    }
                    final InputStreamReader responseBodyReader = new InputStreamReader(conn.getInputStream(), "UTF-8");
                    final BufferedReader bufferedReader = new BufferedReader(responseBodyReader);
                    final String response = bufferedReader.readLine();
                    conn.disconnect();
                    int randomNum = Integer.parseInt(response);
                    Log.d("api", "random number from local server : " + randomNum);
                    return randomNum;
                } catch (IOException e) {
                    e.printStackTrace();
                    int randomNum = random.nextInt(16);
                    Log.d("api", "error in local server : " + randomNum);
                    return randomNum;
                }
            }
        };
        try {
            return asyncTask.execute().get();
        } catch (Exception e) {
            Log.d("ERROR", e.getMessage());
            return random.nextInt(16);
        }
    }
}
