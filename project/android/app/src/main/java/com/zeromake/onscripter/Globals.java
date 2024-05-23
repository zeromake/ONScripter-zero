package com.zeromake.onscripter;

public class Globals {
    public static String CurrentDirectoryPathForLauncher = null;
    public static String CurrentDirectoryPath = null;
    public static String[] CurrentDirectoryPathArray = null;
    public static String[] CurrentDirectoryValidPathArray = null;
    public static final String[] CURRENT_DIRECTORY_PATH_TEMPLATE_ARRAY = new String[]{"${SDCARD}/mine", "${SDCARD}/ons"};
    public static boolean CurrentGameRunning = false;

    public static String[] FallbackDirectoryPathArray = new String[]{
        "/mnt/ext_card",
        "/mnt/flash",
        "/mnt/sdcard",
        "/mnt/sdcard/external_sd",
        "/mnt/sdcard-ext",
        "/mnt/storage/sdcard",
        "/mnt/udisk",
        "/mnt/usbdisk",
        "/sdcard",
        "/sdcard/sd",
        "/storage/sdcard",
        "/storage/sdcard0",
        "/storage/sdcard1",
        "/storage/emulated/0",
        "/storage/emulated/legacy",
        "/storage/usb0",
    };
    public static String[] FallbackDirectoryValidPathArray = null;
}
