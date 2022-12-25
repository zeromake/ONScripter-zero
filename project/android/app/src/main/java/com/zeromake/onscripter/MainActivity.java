package com.zeromake.onscripter;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Environment;
import android.os.Looper;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import com.hjq.permissions.OnPermissionCallback;
import com.hjq.permissions.Permission;
import com.hjq.permissions.XXPermissions;
import com.nostra13.universalimageloader.core.ImageLoader;

import java.io.File;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;

public class MainActivity extends Activity implements
        OnPermissionCallback,
        DialogInterface.OnClickListener,
        AdapterView.OnItemClickListener,
        View.OnClickListener{
    private ImageView background;
    private ImageView btnSettings;
    private ImageView cover;
    private ListView games;
    private TextView gameTitle;
    private AlertDialog mDirBrowserDialog;
    private String mDirBrowserCurDirPath;
    private File[] mDirBrowserDirFileArray;
    private File[] mDirFileArray;
    private GameAdapter items;

    private PopupWindow settingPopupWindow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (Globals.CurrentGameRunning) {
            Intent intent = new Intent(this, ONScripter.class);
            startActivity(intent);
            return;
        }
        Settings.LoadGlobals(this, false);
        DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
        ONSVariable.dw = displayMetrics.widthPixels;
        ONSVariable.dh = displayMetrics.heightPixels;
        int phoneType = ((TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE)).getPhoneType();
        double doubleValue = new BigDecimal(
                ONSVariable.dw / ONSVariable.dh
        ).setScale(
                2,
                RoundingMode.HALF_UP
        ).doubleValue();
        if ((float)ONSVariable.dw / (float)ONSVariable.dh == 1.5d || doubleValue == 1.33d) {
            setContentView(R.layout.activity_ft);
        } else if (phoneType == 0) {
            setContentView(R.layout.activity_pad);
        } else {
            setContentView(R.layout.activity_main);
        }
        initViews();
        externalStoragePermission();
        this.items = new GameAdapter(this, new ArrayList<>());
        this.games.setAdapter((ListAdapter) this.items);
        this.games.setOnItemClickListener(this);
        this.btnSettings.setOnClickListener(view -> {
            setting();
        });
        loadCurrentDirectory();
//        this.items.setOnConfigClickListener(this);
        this.items.setOnPlayClickListener(this);
    }

    private<T extends View> T bindView(int i) {
        return findViewById(i);
    }

    private void initViews() {
        games = bindView(R.id.games);
        cover = bindView(R.id.cover);
        background = bindView(R.id.background);
        gameTitle = bindView(R.id.gametitle);
        btnSettings = bindView(R.id.btn_settings);
    }

    private void externalStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                XXPermissions.with(this).permission(
                        Permission.MANAGE_EXTERNAL_STORAGE
                ).request(this);
            }
        } else {
            XXPermissions.with(this).permission(
                    Permission.READ_EXTERNAL_STORAGE,
                    Permission.WRITE_EXTERNAL_STORAGE
            ).request(this);
        }
    }
    @Override
    public void onGranted(List<String> permissions, boolean all) {
        Toast.makeText(MainActivity.this, "权限申请成功", Toast.LENGTH_SHORT).show();
        Settings.LoadGlobals(this, true);
    }

    @Override
    public void onDenied(List<String> permissions, boolean never) {
        Toast.makeText(MainActivity.this, "权限申请失败", Toast.LENGTH_SHORT).show();
    }

    public void setting() {
        LinearLayout linearLayout = new LinearLayout(this);
        linearLayout.setOrientation(LinearLayout.VERTICAL);
        linearLayout.setGravity(17);
        if (settingPopupWindow != null && settingPopupWindow.isShowing()) {
            settingPopupWindow.dismiss();
        } else {
            settingPopupWindow = new PopupWindow((View) linearLayout, -2, -2, true);
            settingPopupWindow.setAnimationStyle(R.style.Animation_ConfigPanelAnimation);
            settingPopupWindow.setTouchable(true);
            settingPopupWindow.setOutsideTouchable(true);
            settingPopupWindow.setBackgroundDrawable(getResources().getDrawable(R.drawable.config_upper));
            settingPopupWindow.showAtLocation(this.cover, 5, 0, 0);
        }
        Button okCancelButton3 = new Button(this);
        okCancelButton3.setBackgroundColor(Color.argb(0, 0, 0, 0));
        okCancelButton3.setTextSize(21.0f);
        okCancelButton3.setText("游戏路径设置");
        okCancelButton3.setOnClickListener(view -> {
            this.chooseDir();
        });
        linearLayout.addView(okCancelButton3);
    }

    private void chooseDir() {
        CharSequence[] strArr = new CharSequence[Globals.CurrentDirectoryValidPathArray.length + 1];
        for (int i = 0; i < Globals.CurrentDirectoryValidPathArray.length; i++) {
            strArr[i] = Globals.CurrentDirectoryValidPathArray[i];
        }
        strArr[strArr.length - 1] = getString(R.string.Other) + "...";
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setInverseBackgroundForced(true);
        builder.setTitle(getString(R.string.Launch_ChooseDirectory));
        builder.setItems(strArr, this);
        builder.setNegativeButton(getString(R.string.Cancel), (DialogInterface.OnClickListener) null);
        builder.setCancelable(true);
        builder.create().show();
    }

    public void loadCurrentDirectory() {
        if (Globals.CurrentDirectoryPathForLauncher == null || Globals.CurrentDirectoryPathForLauncher.equals("")) {
            return;
        }
        try {
            this.items.clear();
            this.items.notifyDataSetChanged();
            this.mDirFileArray = new File(Globals.CurrentDirectoryPathForLauncher).listFiles(file -> {
                return !file.isHidden() && file.isDirectory() && file.canRead() && file.canWrite();
            });
            assert this.mDirFileArray != null;
            Arrays.sort(this.mDirFileArray, (file, file2) -> file.getName().compareTo(file2.getName()));
            new Thread() {
                @Override
                public void run() {
                    Looper.prepare();
                    for (File file : MainActivity.this.mDirFileArray) {
                        Game scanGameDir = null;
                        if (!file.isHidden() && file.isDirectory()) {
                            scanGameDir = Game.scanGameDir(file);
                        }
                        if (scanGameDir != null) {
                            Game finalScanGameDir = scanGameDir;
                            UiThread.runOnUiThread(() -> items.add(finalScanGameDir));
                        }
                    }
                }
            }.start();
        } catch (Exception unused) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(getString(R.string.Error));
            builder.setMessage(getString(R.string.Launch_CouldNotOpenDirectory) + "\n" + Globals.CurrentDirectoryPathForLauncher);
            builder.setPositiveButton(getString(R.string.OK), (DialogInterface.OnClickListener) null);
            builder.create().show();
            this.games.setAdapter((ListAdapter) new ArrayAdapter<>(this, R.layout.gamelist_item, new String[0]));
        }
    }

    @Override
    public void onClick(DialogInterface dialogInterface, int i) {
        if (dialogInterface == this.mDirBrowserDialog) {
            this.mDirBrowserCurDirPath = this.mDirBrowserDirFileArray[i].getAbsolutePath();
            this.mDirBrowserDialog = null;
            this.mDirBrowserDirFileArray = null;
        } else if (i < Globals.CurrentDirectoryValidPathArray.length) {
            Globals.CurrentDirectoryPathForLauncher = Globals.CurrentDirectoryValidPathArray[i];
            Settings.SaveGlobals(this);
            loadCurrentDirectory();
            return;
        } else if (Globals.CurrentDirectoryPathForLauncher == null || Globals.CurrentDirectoryPathForLauncher.equals("")) {
            this.mDirBrowserCurDirPath = "/";
        } else {
            this.mDirBrowserCurDirPath = Globals.CurrentDirectoryPathForLauncher;
        }
        File file = new File(this.mDirBrowserCurDirPath);
        File parentFile = file.getParentFile();
        ArrayList<File> dirs = new ArrayList<>();
        if (parentFile != null && parentFile.canRead()) {
            dirs.add(parentFile);
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            try (Stream<Path> stream = Files.list(Paths.get(this.mDirBrowserCurDirPath))) {
                stream.forEach(path -> {
                    if (Files.isDirectory(path)
                            && Files.isReadable(path)) {
                        dirs.add(new File(path.toAbsolutePath().toString()));
                    }
                });
            } catch (IOException ignore) {
                AlertDialog.Builder builder2 = new AlertDialog.Builder(this);
                builder2.setTitle(getString(R.string.Error));
                builder2.setMessage(getString(R.string.Launch_CouldNotOpenDirectory) + "\n" + this.mDirBrowserCurDirPath);
                builder2.setPositiveButton(getString(R.string.OK), (DialogInterface.OnClickListener) null);
                builder2.create().show();
                return;
            }
        } else {
            File[] arr = file.listFiles(file2 -> file2.isDirectory() && file2.canRead());
            assert arr != null;
            dirs.addAll(Arrays.asList(arr));
        }
        this.mDirBrowserDirFileArray = new File[dirs.size()];
        String[] strArr = new String[this.mDirBrowserDirFileArray.length];
        for (int index = 0; index < dirs.size(); index++) {
            this.mDirBrowserDirFileArray[index] = dirs.get(index);
            strArr[index] = dirs.get(index).getName();
        }
        if (parentFile != null && strArr.length > 0) {
            strArr[0] = "..";
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(this.mDirBrowserCurDirPath);
        builder.setItems(strArr, this);
        builder.setPositiveButton(getString(R.string.Launch_SetDirectory), (dialogInterface2, i5) -> {
            Globals.CurrentDirectoryPathForLauncher = this.mDirBrowserCurDirPath;
            Settings.SaveGlobals(this);
            this.loadCurrentDirectory();
        });
        builder.setNegativeButton(getString(R.string.Cancel), null);
        builder.setCancelable(true);
        this.mDirBrowserDialog = builder.create();
        this.mDirBrowserDialog.show();
    }


    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        U.scrollViewToCenter(view, this.games);
        if (this.items.getSelectedPosition() != i) {
            this.items.setSelectedPosition(i);
            loadGameItem(this.items.getSelectedItem());
            Globals.CurrentDirectoryPath = this.mDirFileArray[i].getAbsolutePath();
        }
    }

    private void loadGameItem(Game game) {
        this.gameTitle.setText(game.title);
        if (game.background != null) {
            updateBackground(game.background);
        }
        if (game.cover != null) {
            updateCover(game.cover, game.background == null);
        } else {
            this.cover.setImageDrawable(getResources().getDrawable(R.drawable.dbkg_und));
            if (game.background == null) {
                this.background.setImageDrawable(getResources().getDrawable(R.drawable.dbkg_und_blur));
            }
        }
    }

    private void updateCover(String cover, boolean b) {
        ImageLoader.getInstance().displayImage("file:/"+cover, this.cover, Settings.getDisplayImageOptions());
    }

    private void updateBackground(String background) {
        ImageLoader.getInstance().displayImage("file:/"+background, this.background, Settings.getDisplayImageOptions());
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.btn_play) {
            Game selectedItem = items.getSelectedItem();
            Intent intent = new Intent(this, ONScripter.class);
            Bundle extras = new Bundle();
            // types(0:全局文字,1:普通文字, 2:精灵文字, 3: 菜单文字, 4: 弹窗文字, 5: ruby 文字),font size1,font size2,color,render_outline,outline_size,outline_color
            // 大部分游戏只有 1, 2 生效，后面的文字都是用精灵文字去做的，还有不少弹框是用图片做的。
            extras.putStringArray(ONScripter.ARGS_KEY, new String[]{
                    "-r",
                    selectedItem.path,
                    "--scale-window",
                    "--fontcache",
                    "--font-config",
                    "1:3,2,,1,5,#03a9f4",
                    // "--sharpness",
                    // "1.0",
            });
            intent.putExtras(extras);
            startActivity(intent);
        }
    }
}