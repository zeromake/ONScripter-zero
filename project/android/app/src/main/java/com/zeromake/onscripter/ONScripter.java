package com.zeromake.onscripter;

import org.libsdl.app.FileProvider;
import org.libsdl.app.SDLActivity;

import android.content.Intent;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;


import java.io.File;

public class ONScripter extends SDLActivity {
    public static String ARGS_KEY = "args";
    private String[]  arguments;
    private String rootPath = null;

    // @Override
    // protected boolean initWindowStyle() {
    //     return false;
    // }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "sdl2",
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
        nativeInitJavaCallbacks();
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            String[] args = extras.getStringArray(ARGS_KEY);
            if (args != null && args.length > 0) {
                Globals.CurrentGameRunning = true;
                arguments = args;
                boolean hasRoot = false;
                for (String arg : args) {
                    if (arg.equals("-r")) {
                        hasRoot = true;
                    } else if (hasRoot) {
                        rootPath = arg;
                        break;
                    }
                }
            }
        }
    }

    public void playVideo(char[] name) {
        Log.d("ONScripter", "playVideo: " + rootPath + "/" + String.copyValueOf(name));
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        Uri uri = null;
        String file = rootPath + "/" + String.copyValueOf(name);
        if(Build.VERSION.SDK_INT>= Build.VERSION_CODES.N){
            uri = FileProvider.getUriForFile(this,"com.zeromake.onscripter", new File(file));
        } else {
            Uri.parse("file://" + file);
        }
        intent.setDataAndType(uri, "video/*");
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(intent);
    }

    public void onNativeCrashed() {
        new RuntimeException("crashed here (native trace should follow after the Java trace)").printStackTrace();
        Application.getInstance().startActivity(new Intent(Application.getInstance(), CrashHandler.class));
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
