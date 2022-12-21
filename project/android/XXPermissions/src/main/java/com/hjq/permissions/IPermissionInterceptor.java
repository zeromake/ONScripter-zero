package com.hjq.permissions;

import android.app.Activity;

import java.util.ArrayList;
import java.util.List;

/**
 *    author : Android 轮子哥
 *    github : https://github.com/getActivity/XXPermissions
 *    time   : 2020/12/26
 *    desc   : 权限请求拦截器
 */
public interface IPermissionInterceptor {

    /**
     * 发起权限申请（可在此处先弹 Dialog 再申请权限，如果用户已经授予权限，则不会触发此回调）
     *
     * @param allPermissions            申请的权限
     * @param callback                  权限申请回调
     */
    default void launchPermissionRequest(Activity activity, List<String> allPermissions,
                                         OnPermissionCallback callback) {
        PermissionFragment.launch(activity, new ArrayList<>(allPermissions), this, callback);
    }

    /**
     * 用户授予了权限（注意需要在此处回调 {@link OnPermissionCallback#onGranted(List, boolean)}）
     *
     * @param allPermissions             申请的权限
     * @param grantedPermissions         已授予的权限
     * @param all                        是否全部授予
     * @param callback                   权限申请回调
     */
    default void grantedPermissionRequest(Activity activity, List<String> allPermissions,
                                          List<String> grantedPermissions, boolean all,
                                          OnPermissionCallback callback) {
        if (callback == null) {
            return;
        }
        callback.onGranted(grantedPermissions, all);
    }

    /**
     * 用户拒绝了权限（注意需要在此处回调 {@link OnPermissionCallback#onDenied(List, boolean)}）
     *
     * @param allPermissions            申请的权限
     * @param deniedPermissions         已拒绝的权限
     * @param never                     是否勾选了不再询问选项
     * @param callback                  权限申请回调
     */
    default void deniedPermissionRequest(Activity activity, List<String> allPermissions,
                                         List<String> deniedPermissions, boolean never,
                                         OnPermissionCallback callback) {
        if (callback == null) {
            return;
        }
        callback.onDenied(deniedPermissions, never);
    }


    /**
     * 权限请求完成
     *
     * @param allPermissions            申请的权限
     * @param skipRequest               是否跳过了申请过程
     * @param callback                  权限申请回调
     */
    default void finishPermissionRequest(Activity activity, List<String> allPermissions,
                                         boolean skipRequest, OnPermissionCallback callback) {}
}