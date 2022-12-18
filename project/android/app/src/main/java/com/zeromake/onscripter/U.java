package com.zeromake.onscripter;

import android.view.View;
import android.widget.ListAdapter;
import android.widget.ListView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

public class U {

    public static String read(File file) {
        try {
            StringBuilder stringBuffer = new StringBuilder();
            BufferedReader bufferedReader = new BufferedReader(new FileReader(file));
            while (true) {
                String readLine = bufferedReader.readLine();
                if (readLine != null) {
                    stringBuffer.append(readLine);
                    stringBuffer.append("\n");
                } else {
                    bufferedReader.close();
                    return stringBuffer.toString();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static boolean supportVideoMedia(String str) {
        return str.endsWith(".avi") || str.endsWith(".mp4") || str.endsWith(".mpg") || str.endsWith(".rmvb") || str.endsWith(".mpeg") || str.endsWith(".flv") || str.endsWith(".rm") || str.endsWith(".f4v") || str.endsWith(".hlv") || str.endsWith(".wmv") || str.endsWith(".3gp") || str.endsWith(".mkv");
    }

    public static boolean supportAudioMedia(String str) {
        return str.endsWith(".mp3") || str.endsWith(".wma") || str.endsWith(".flac") || str.endsWith(".ape") || str.endsWith(".ogg") || str.endsWith(".m4a") || str.endsWith(".aac");
    }

    public static void SCROLL_LIST_FOR_DISTANCE_IN_ANY_MILLIS(ListView listView, int i, int i2) {
        listView.smoothScrollBy(i, i2);
    }

    public static void scrollViewToCenter(View view, ListView listView) {
        ListAdapter adapter = listView.getAdapter();
        int top = (view.getTop() + (view.getHeight() / 2)) - (listView.getHeight() / 2);
        if (top < 0 && listView.getFirstVisiblePosition() == 0) {
            listView.smoothScrollToPosition(0);
        } else if (top <= 0 || listView.getLastVisiblePosition() != adapter.getCount() - 1) {
            SCROLL_LIST_FOR_DISTANCE_IN_ANY_MILLIS(listView, top, 300);
        } else {
            listView.smoothScrollToPosition(adapter.getCount() - 1);
        }
    }
}
