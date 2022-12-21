package com.zeromake.onscripter;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class CrashHandler extends Activity {
    public static final String TAG = "CrashHandler";
    private EditText textView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.crash_handler);
        textView = findViewById(R.id.content);
        Button button = findViewById(R.id.task_button);
        button.setOnClickListener(v -> {
            final AsyncTask<Void, Void, Void> task = new LogTask(CrashHandler.this).execute();
        });
    }

    static class LogTask extends AsyncTask<Void, Void, Void>
    {
        Process process;
        @SuppressLint("StaticFieldLeak")
        CrashHandler a;
  
        LogTask(CrashHandler a) {
            super();
            this.a = a;
        }

        @Override
        protected Void doInBackground(Void... v) {
            try {
                Log.e(CrashHandler.TAG, "doInBackground begin");
                process = Runtime.getRuntime().exec(new String[]{"logcat","-d","-t","500","-v","threadtime"});
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while((line=reader.readLine())!=null)
                {
                    String finalLine = line;
                    UiThread.runOnUiThread(() -> this.a.textView.append(finalLine));
                }
                Log.e(CrashHandler.TAG, "doInBackground end");
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
  
        @Override
        protected void onCancelled() {
            Log.e(CrashHandler.TAG, "onCancelled");
            process.destroy();
        }

        @Override
        protected void onPostExecute(Void v) {
            Log.e(CrashHandler.TAG, "onPostExecute");
        }
    }
}
