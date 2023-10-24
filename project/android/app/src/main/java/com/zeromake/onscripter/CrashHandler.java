package com.zeromake.onscripter;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class CrashHandler extends Activity {
    private EditText textView = null;
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (process != null) {
            process.destroy();
        }
        if (executor != null) {
            executor.shutdown();
            executor = null;
        }
    }

    private ExecutorService executor = null;

    private Process process;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.crash_handler);
        textView = findViewById(R.id.content);
        Button button = findViewById(R.id.task_button);
        executor = Executors.newCachedThreadPool();
        button.setOnClickListener(v -> executor.execute(() -> {
            try {
                process = Runtime.getRuntime().exec(new String[]{
                        "logcat",
                        "-d",
                        "-t",
                        "500",
                        "-v",
                        "threadtime"
                });
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    String finalLine = line;
                    UiThread.runOnUiThread(() -> this.textView.append(finalLine));
                }
                process = null;
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }));
    }
}
