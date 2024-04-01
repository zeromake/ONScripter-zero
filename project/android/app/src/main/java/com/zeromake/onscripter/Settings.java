package com.zeromake.onscripter;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Environment;

import com.nostra13.universalimageloader.core.DisplayImageOptions;

import java.io.File;
import java.util.ArrayList;
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
            if (Globals.CurrentDirectoryPathForLauncher.equals("")) {
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
        for (String str : Globals.CURRENT_DIRECTORY_PATH_TEMPLATE_ARRAY) {
            if (!str.contains("${SDCARD}")) {
                treeSet.add(str);
            } else {
                treeSet.add(str.replace("${SDCARD}", Environment.getExternalStorageDirectory().getAbsolutePath()));
                treeSet.add(str.replace("${SDCARD}", "/mnt/ext_card"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/flash"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/sdcard"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/sdcard/external_sd"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/sdcard-ext"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/storage/sdcard"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/udisk"));
                treeSet.add(str.replace("${SDCARD}", "/mnt/usbdisk"));
                treeSet.add(str.replace("${SDCARD}", "/sdcard"));
                treeSet.add(str.replace("${SDCARD}", "/sdcard/sd"));
                treeSet.add(str.replace("${SDCARD}", "/storage/sdcard"));
                treeSet.add(str.replace("${SDCARD}", "/storage/emulated/0"));
                String str2 = System.getenv("EXTERNAL_STORAGE");
                if (str2 != null) {
                    treeSet.add(str.replace("${SDCARD}", str2));
                }
                String str3 = System.getenv("EXTERNAL_STORAGE2");
                if (str3 != null) {
                    treeSet.add(str.replace("${SDCARD}", str3));
                }
                String str4 = System.getenv("EXTERNAL_ALT_STORAGE");
                if (str4 != null) {
                    treeSet.add(str.replace("${SDCARD}", str4));
                }
                String str5 = System.getenv("SECOND_VOLUME_STORAGE");
                if (str5 != null) {
                    treeSet.add(str.replace("${SDCARD}", str5));
                }
                String str6 = System.getenv("THIRD_VOLUME_STORAGE");
                if (str6 != null) {
                    treeSet.add(str.replace("${SDCARD}", str6));
                }
                String str7 = System.getenv("SECONDARY_STORAGE");
                if (str7 != null) {
                    treeSet.add(str.replace("${SDCARD}", str7));
                }
                String str8 = System.getenv("EXTERNAL_STORAGE_ALL");
                if (str8 != null) {
                    for (String str9 : str8.split(";")) {
                        treeSet.add(str.replace("${SDCARD}", str9));
                    }
                }
            }
            for (String s : treeSet) {
                File file2 = new File(s);
                if (file2.exists() && file2.isDirectory() && file2.canRead() && file2.canWrite()) {
                    String absolutePath = file2.getAbsolutePath();
                    if (Globals.CurrentDirectoryPathForLauncher == null || Globals.CurrentDirectoryPathForLauncher.equals("")) {
                        Globals.CurrentDirectoryPathForLauncher = absolutePath;
                    }
                    if (Globals.CurrentDirectoryPath == null || Globals.CurrentDirectoryPath.equals("")) {
                        Globals.CurrentDirectoryPath = absolutePath;
                    }
                    arrayList.add(absolutePath);
                }
            }
        }
        Globals.CurrentDirectoryPathArray = (String[]) treeSet.toArray(new String[0]);
        Globals.CurrentDirectoryValidPathArray = (String[]) arrayList.toArray(new String[0]);
    }

    public static DisplayImageOptions getDisplayImageOptions() {
        return new DisplayImageOptions.Builder()
                .cacheInMemory(true)//是否进行内存缓存
                .build();
    }
}
