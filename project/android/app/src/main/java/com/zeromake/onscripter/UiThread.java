package com.zeromake.onscripter;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class UiThread {
    private final Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            if (msg.obj instanceof Runnable) {
                ((Runnable) msg.obj).run();
            }
        }

    };

    private final static UiThread instance = new UiThread();


    public static void runOnUiThread(Runnable r) {
        Message msg = new Message();
        msg.obj = r;
        instance.mHandler.sendMessage(msg);
    }

    public static void runOnUiThreadDelayed(Runnable r, long delayMillis) {
        Message msg = new Message();
        msg.obj = r;
        instance.mHandler.sendMessageDelayed(msg, delayMillis);
    }
}
