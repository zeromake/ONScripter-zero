package com.zeromake.onscripter;

import org.libsdl.app.SDLActivity;

import android.content.Intent;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.file.FileSystem;

public class ONScripter extends SDLActivity {
    public static String ARGS_KEY = "args";
    private String[]  arguments;

    @Override
    protected boolean initWindowStyle() {
        return false;
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
//                "sdl2",
                "onscripter"
        };
    }

    @Override
    protected String[] getArguments() {
        return arguments;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        nativeInitJavaCallbacks();
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            String[] args = extras.getStringArray(ARGS_KEY);
            if (args != null && args.length > 0) {
                Globals.CurrentGameRunning = true;
                arguments = args;
            }
        }
    }

    public void playVideo(char[] name) {
        Log.d("ONScripter", "playVideo");
    }

//    public static void onNativeCrashed() {
//        new RuntimeException("crashed here (native trace should follow after the Java trace)").printStackTrace();
//        Application.getInstance().startActivity(new Intent(Application.getInstance(), CrashHandler.class));
//    }

    public static int mkdir(char[] name) {
        File f = new File(new String(name));
        return f.mkdirs() ? 0 : 1;
    }

    public native int nativeInitJavaCallbacks();
    public static native int nativeGetWidth();
    public static native int nativeGetHeight();

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Globals.CurrentGameRunning = false;
    }

}