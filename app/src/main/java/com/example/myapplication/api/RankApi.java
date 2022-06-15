package com.example.myapplication.api;

import android.os.AsyncTask;
import android.util.JsonReader;

import com.example.myapplication.domain.Rank;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class RankApi extends AsyncTask<String, Void, List<Rank>> {

    private static final String END_POINT = "http://3.214.64.150:8080";





    @Override
    protected List<Rank> doInBackground(String... params) {
        try {
            final List<Rank> ranks = new ArrayList<>();
            final URL endPoint = new URL(END_POINT + "/score");
            final HttpURLConnection conn = (HttpURLConnection) endPoint.openConnection();

            if (conn.getResponseCode() == 200) {
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
            } else {
                throw new IllegalArgumentException("Server connection error");
            }
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return Collections.EMPTY_LIST;
    }
}
