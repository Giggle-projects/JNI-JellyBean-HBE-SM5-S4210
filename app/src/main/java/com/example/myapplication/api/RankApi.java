package com.example.myapplication.api;

import android.os.AsyncTask;
import android.util.JsonReader;
import android.util.Log;

import com.example.myapplication.domain.Rank;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ExecutionException;

public class RankApi {

    private static final String END_POINT = "http://3.214.64.150:8080";

    public static void uploadRank(String name, int score) {
        AsyncTask<String, Void, Void> asyncTask = new AsyncTask<String, Void, Void>() {
            @Override
            protected Void doInBackground(String... params) {
                try {
                    final URL endPoint = new URL(END_POINT + "/score/rank?score="+score+"&name="+name);
                    final HttpURLConnection conn = (HttpURLConnection) endPoint.openConnection();
                    conn.setRequestMethod("POST");
                    final OutputStream os = conn.getOutputStream();
                    os.flush();
                    os.close();
                    if (conn.getResponseCode() != 200) {
                        throw new IllegalArgumentException("Server connection error");
                    }
                    conn.disconnect();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                return null;
            }
        };
        try {
            asyncTask.execute().get();
        } catch (Exception e) {
            Log.d("ERROR", e.getMessage());
        }
    }

    public static Integer getRankOf(int score) {
        AsyncTask<String, Void, Integer> asyncTask = new AsyncTask<String, Void, Integer>() {
            @Override
            protected Integer doInBackground(String... params) {
                try {
                    final URL endPoint = new URL(END_POINT + "/score/rank?score="+score);
                    final HttpURLConnection conn = (HttpURLConnection) endPoint.openConnection();
                    if (conn.getResponseCode() != 200) {
                        throw new IllegalArgumentException("Server connection error");
                    }
                    final InputStreamReader responseBodyReader = new InputStreamReader(conn.getInputStream(), "UTF-8");
                    final BufferedReader bufferedReader = new BufferedReader(responseBodyReader);
                    final int valRank = Integer.parseInt(bufferedReader.readLine());
                    conn.disconnect();
                    return valRank;
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (Exception e) {
                    e.printStackTrace();
                } catch (Throwable a) {
                    a.printStackTrace();
                }
                Log.d("ERROR", "RRR");
                return Integer.MAX_VALUE;
            }
        };
        try {
            return asyncTask.execute().get();
        } catch (Exception e) {
            Log.d("ERROR", e.getMessage());
        } catch (Throwable a) {
            a.printStackTrace();
        }
        return Integer.MAX_VALUE;
    }

    public static List<Rank> findAll() throws ExecutionException, InterruptedException {
        AsyncTask<String, Void, List<Rank>> asyncTask = new AsyncTask<String, Void, List<Rank>>() {
            @Override
            protected List<Rank> doInBackground(String... params) {
                try {
                    final List<Rank> ranks = new ArrayList<>();
                    final URL endPoint = new URL(END_POINT + "/score");
                    final HttpURLConnection conn = (HttpURLConnection) endPoint.openConnection();
                    if (conn.getResponseCode() != 200) {
                        throw new IllegalArgumentException("Server connection error");
                    }
                    final InputStreamReader responseBodyReader = new InputStreamReader(conn.getInputStream(), "UTF-8");
                    final JsonReader jsonReader = new JsonReader(responseBodyReader);
                    jsonReader.beginArray();
                    while (jsonReader.hasNext()) {
                        jsonReader.beginObject();
                        while (jsonReader.hasNext()) {
                            jsonReader.nextName();
                            final int valRank = jsonReader.nextInt();
                            jsonReader.nextName();
                            final String valName = jsonReader.nextString();
                            jsonReader.nextName();
                            final int valScore = jsonReader.nextInt();
                            final Rank rank = new Rank(valRank, valName, valScore);
                            ranks.add(rank);
                        }
                        jsonReader.endObject();
                    }
                    jsonReader.endArray();
                    conn.disconnect();
                    jsonReader.close();
                    return ranks;
                } catch (IOException e) {
                    Log.d("ERROR", "Server connection error");
                    return Collections.EMPTY_LIST;
                }
            }
        };
        return asyncTask.execute().get();
    }
}
