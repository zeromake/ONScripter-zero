package com.zeromake.onscripter;

import android.os.Bundle;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.util.Objects;

public class Game {
    public String path;
    public String audio;
    public String background;
    public String cover;
    public String description;
    public String icon;
    public String title;
    public String video;

    public void readJSON(JSONObject jSONObject) throws JSONException {
        readJSON(jSONObject, false);
    }

    public void readJSON(JSONObject jSONObject, boolean z) throws JSONException {
        if (jSONObject.has("title") && (z || this.title == null)) {
            this.title = jSONObject.getString("title");
        }
        if (jSONObject.has("cover") && (z || this.cover == null)) {
            this.cover = jSONObject.getString("cover");
        }
        if (jSONObject.has("description") && (z || this.description == null)) {
            this.description = jSONObject.getString("description");
        }
        if (jSONObject.has("background") && (z || this.background == null)) {
            this.background = jSONObject.getString("background");
        }
        if (jSONObject.has("icon") && (z || this.icon == null)) {
            this.icon = jSONObject.getString("icon");
        }
        if (jSONObject.has("video") && (z || this.video == null)) {
            this.video = jSONObject.getString("video");
        }
        if (jSONObject.has("audio")) {
            if (!z && this.audio != null) {
                return;
            }
            this.audio = jSONObject.getString("audio");
        }
    }

    public static Game fromBundle(Bundle bundle) {
        Game game = new Game();
        game.path = bundle.getString("path");
        game.title = bundle.getString("title");
        game.cover = bundle.getString("cover");
        game.description = bundle.getString("description");
        game.background = bundle.getString("background");
        game.icon = bundle.getString("icon");
        game.video = bundle.getString("video");
        game.audio = bundle.getString("audio");
        return game;
    }

    public Bundle toBundle() {
        Bundle bundle = new Bundle();
        bundle.putString("path", this.path);
        bundle.putString("title", this.title);
        bundle.putString("cover", this.cover);
        bundle.putString("description", this.description);
        bundle.putString("background", this.background);
        bundle.putString("icon", this.icon);
        bundle.putString("video", this.video);
        bundle.putString("audio", this.audio);
        return bundle;
    }

    public static Game scanGameDir(File file) {
        Game game = new Game();
        game.path = file.getAbsolutePath();
        game.title = file.getName();
        File file2 = new File(file, "media.json");
        if (file2.exists()) {
            try {
                game.readJSON(new JSONObject(Objects.requireNonNull(U.read(file2))), true);
            } catch (Exception ignored) {
            }
        }
        if (game.cover == null || game.background == null || game.video == null || game.icon == null || game.audio == null) {
            String[] list = file.list();
            assert list != null;
            for (String str : list) {
                String lowerCase = str.toLowerCase();
                if ((lowerCase.equals("cover.jpg") || lowerCase.equals("cover.png")) && game.cover == null) {
                    game.cover = new File(file, str).getAbsolutePath();
                }
                if ((lowerCase.equals("background.jpg") || lowerCase.equals("background.png") || lowerCase.equals("bkg.jpg") || lowerCase.equals("bkg.png")) && game.background == null) {
                    game.background = new File(file, str).getAbsolutePath();
                }
                if ((lowerCase.equals("preview.mp4") || lowerCase.equals("preview.avi") || lowerCase.equals("preview.mpg") || lowerCase.equals("pv.mp4") || lowerCase.equals("pv.avi") || lowerCase.equals("pv.mpg") || lowerCase.equals("op.mp4") || lowerCase.equals("op.avi") || lowerCase.equals("op.mpg")) && game.video == null) {
                    game.video = new File(file, str).getAbsolutePath();
                }
                if ((lowerCase.equals("theme.mp3") || lowerCase.equals("theme.flac") || lowerCase.equals("theme.ogg") || lowerCase.equals("theme.wma") || lowerCase.equals("track.mp3") || lowerCase.equals("track.flac") || lowerCase.equals("track.ogg") || lowerCase.equals("track.wma")) && game.audio == null) {
                    game.audio = new File(file, str).getAbsolutePath();
                }
                if ((lowerCase.equals("icon.jpg") || lowerCase.equals("icon.png")) && game.icon == null) {
                    game.icon = new File(file, str).getAbsolutePath();
                }
            }
            if (game.cover != null && game.video != null && game.audio != null) {
                return game;
            }
            for (String str2 : list) {
                String lowerCase2 = str2.toLowerCase();
                if ((lowerCase2.endsWith(".jpg") || lowerCase2.endsWith(".png")) && game.cover == null) {
                    game.cover = new File(file, str2).getAbsolutePath();
                }
                if (U.supportAudioMedia(lowerCase2) && game.audio == null) {
                    game.audio = new File(file, str2).getAbsolutePath();
                }
                if (lowerCase2.startsWith("preview.") && U.supportVideoMedia(lowerCase2) && game.video == null) {
                    game.video = new File(file, str2).getAbsolutePath();
                }
            }
            if (game.video != null) {
                return game;
            }
            for (String str3 : list) {
                if (U.supportVideoMedia(str3.toLowerCase()) && game.video == null) {
                    game.video = new File(file, str3).getAbsolutePath();
                }
            }
            return game;
        }
        return game;
    }
}
