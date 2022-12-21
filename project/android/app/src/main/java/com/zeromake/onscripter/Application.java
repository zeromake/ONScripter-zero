package com.zeromake.onscripter;


import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;

public class Application extends android.app.Application {
    private static Application _app;
    public static android.app.Application getInstance() {
        return _app;
    }
    @Override
    public void onCreate() {
        super.onCreate();
        _app = this;
        ImageLoader.getInstance().init(ImageLoaderConfiguration.createDefault(getApplicationContext()));
    }
}