package com.hjq.permissions;

import android.app.Activity;
import android.content.Context;

/**
 *    author : Android 轮子哥
 *    github : https://github.com/getActivity/XXPermissions
 *    time   : 2022/07/03
 *    desc   : Android 9.0 权限委托实现
 */
class PermissionDelegateImplV28 extends PermissionDelegateImplV26 {

   @Override
   public boolean isGrantedPermission(Context context, String permission) {
      if (PermissionUtils.equalsPermission(permission, Permission.ACCEPT_HANDOVER)) {
         return PermissionUtils.checkSelfPermission(context, permission);
      }
      return super.isGrantedPermission(context, permission);
   }

   @Override
   public boolean isPermissionPermanentDenied(Activity activity, String permission) {
      if (PermissionUtils.equalsPermission(permission, Permission.ACCEPT_HANDOVER)) {
         return !PermissionUtils.checkSelfPermission(activity, permission) &&
                 !PermissionUtils.shouldShowRequestPermissionRationale(activity, permission);
      }
      return super.isPermissionPermanentDenied(activity, permission);
   }
}