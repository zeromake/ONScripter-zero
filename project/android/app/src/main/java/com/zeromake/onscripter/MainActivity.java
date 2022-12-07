package com.zeromake.onscripter;

import android.os.Bundle;
import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity
{
	@Override
    protected boolean initWindowStyle() {
        return true;
    }

	@Override
	protected void onCreate(Bundle state) {
		super.onCreate(state);
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "main"
        };
    }
}