package com.zeromake.onscripter;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Environment;

import com.nostra13.universalimageloader.core.DisplayImageOptions;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.TreeSet;

public class Settings {

    private static boolean globalsSettingsLoaded = false;

    private Settings() {
    }

    public static void LoadGlobals(Activity activity,  boolean reload) {
        if (globalsSettingsLoaded && !reload) {
            return;
        }
        SharedPreferences sharedPreferences = activity.getSharedPreferences("pref", 0);
        Globals.CurrentDirectoryPathForLauncher = sharedPreferences.getString("CurrentDirectoryPathForLauncher", Globals.CurrentDirectoryPathForLauncher);
        setupCurrentDirectory();
        globalsSettingsLoaded = true;
    }

    public static void SaveGlobals(Activity activity) {
        SharedPreferences.Editor edit = activity.getSharedPreferences("pref", 0).edit();
        edit.clear();
        if (Globals.CurrentDirectoryPathForLauncher != null) {
            edit.putString("CurrentDirectoryPathForLauncher", Globals.CurrentDirectoryPathForLauncher);
        }
        edit.apply();
    }

    private static void setupCurrentDirectory() {
        if (Globals.CurrentDirectoryPathForLauncher != null) {
            if (Globals.CurrentDirectoryPathForLauncher.isEmpty()) {
                Globals.CurrentDirectoryPathForLauncher = null;
            } else {
                File file = new File(Globals.CurrentDirectoryPathForLauncher);
                if (!file.exists() || !file.isDirectory() || !file.canRead()) {
                    Globals.CurrentDirectoryPathForLauncher = null;
                }
            }
        }
        Globals.CurrentDirectoryPath = null;
        ArrayList<String> arrayList = new ArrayList<>();
        TreeSet<String> treeSet = new TreeSet<>();
        TreeSet<String> validDirectory = new TreeSet<>();
        validDirectory.add(Environment.getExternalStorageDirectory().getAbsolutePath());
        for (String inlineSd: Globals.FallbackDirectoryPathArray) {
            File inlineSdF = new File(inlineSd);
            if (inlineSdF.exists() && inlineSdF.isDirectory() && inlineSdF.canRead()) {
                validDirectory.add(inlineSd);
            }
        }
        TreeSet<String> validEnvDirectory = new TreeSet<>();
        String str2 = System.getenv("EXTERNAL_STORAGE");
        if (str2 != null) {
            validEnvDirectory.add(str2);
        }
        String str3 = System.getenv("EXTERNAL_STORAGE2");
        if (str3 != null) {
            validEnvDirectory.add(str3);
        }
        String str4 = System.getenv("EXTERNAL_ALT_STORAGE");
        if (str4 != null) {
            validEnvDirectory.add(str4);
        }
        String str5 = System.getenv("SECOND_VOLUME_STORAGE");
        if (str5 != null) {
            validEnvDirectory.add(str5);
        }
        String str6 = System.getenv("THIRD_VOLUME_STORAGE");
        if (str6 != null) {
            validEnvDirectory.add(str6);
        }
        String str7 = System.getenv("SECONDARY_STORAGE");
        if (str7 != null) {
            validEnvDirectory.add(str7);
        }
        String str8 = System.getenv("EXTERNAL_STORAGE_ALL");
        if (str8 != null) {
            validEnvDirectory.addAll(Arrays.asList(str8.split(";")));
        }

        for (String str : Globals.CURRENT_DIRECTORY_PATH_TEMPLATE_ARRAY) {
            if (!str.contains("${SDCARD}")) {
                treeSet.add(str);
            } else {
                treeSet.add(str.replace("${SDCARD}", Environment.getExternalStorageDirectory().getAbsolutePath()));
                for (String inlineSd : validDirectory) {
                    treeSet.add(str.replace("${SDCARD}", inlineSd));
                }
                for (String envSd: validEnvDirectory) {
                    treeSet.add(str.replace("${SDCARD}", envSd));
                }
            }
            for (String s : treeSet) {
                File file2 = new File(s);
                if (file2.exists() && file2.isDirectory() && file2.canRead() && file2.canWrite()) {
                    String absolutePath = file2.getAbsolutePath();
                    if (Globals.CurrentDirectoryPathForLauncher == null || Globals.CurrentDirectoryPathForLauncher.isEmpty()) {
                        Globals.CurrentDirectoryPathForLauncher = absolutePath;
                    }
                    if (Globals.CurrentDirectoryPath == null || Globals.CurrentDirectoryPath.isEmpty()) {
                        Globals.CurrentDirectoryPath = absolutePath;
                    }
                    arrayList.add(absolutePath);
                }
            }
        }
        Globals.CurrentDirectoryPathArray = treeSet.toArray(new String[0]);
        Globals.CurrentDirectoryValidPathArray = arrayList.toArray(new String[0]);
        Globals.FallbackDirectoryValidPathArray = validDirectory.toArray(new String[0]);
    }

    public static DisplayImageOptions getDisplayImageOptions() {
        return new DisplayImageOptions.Builder()
                .cacheInMemory(true)//是否进行内存缓存
                .build();
    }
}
