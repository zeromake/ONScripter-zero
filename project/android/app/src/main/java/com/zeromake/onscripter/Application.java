package com.zeromake.onscripter;


import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();
        ImageLoader.getInstance().init(ImageLoaderConfiguration.createDefault(getApplicationContext()));
    }
}