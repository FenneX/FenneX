/****************************************************************************
 Copyright (c) 2013-2014 Auticiel SAS

 http://www.fennex.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************///

package com.fennex.modules;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.util.SparseIntArray;

import java.util.HashMap;
import java.util.Map;


public class DevicePermissions
{
    public native static void notifyPermissionRequestEnded(int permission, boolean result);
    public native static void notifyCustomPermissionRequestEnded(String permission, boolean result);

    public static final int EXTERNAL_STORAGE_PERMISSION = 0;
    public static final int AUDIO_PERMISSION = 1;
    public static final int CAMERA_PERMISSION = 2;
    public static final int PHONE_STATE_PERMISSION = 3;

    private static final SparseIntArray currentRequest = new SparseIntArray();
    private static final Map<Integer, String> currentCustomRequest = new HashMap<>();
    private static int lastID = 42; // Do not start at 0 since it's a default value for onRequestPermissionsResult

    @SuppressWarnings("unused,WeakerAccess")
    public static boolean hasPermission(int permissionValue)
    {
        return hasPermission(NativeUtility.getMainActivity(), permissionValue);
    }

    @SuppressWarnings("unused")
    public static boolean hasPermission(String permission)
    {
        return hasPermission(NativeUtility.getMainActivity(), new String[]{ permission });
    }

    @SuppressWarnings("WeakerAccess")
    public static boolean hasPermission(Context context, int permissionValue) {
        String[] permissions = getPermissionString(permissionValue);
        return hasPermission(context, permissions);
    }

    private static boolean hasPermission(Context context, String[] permissions) {
        boolean allowed = true;
        int i = 0;
        while(allowed && i < permissions.length)
        {
            allowed = (ContextCompat.checkSelfPermission(context, permissions[i]) == PackageManager.PERMISSION_GRANTED);
            i++;
        }
        return allowed;
    }

    @SuppressWarnings("unused")
    public static boolean requestPermission(String permission)
    {
        // It's no longer possible to use those permissions on Android 13+
        // Code requesting that should already be migrated to use app internal storage on Android 11+
        // and only actually need it for migration purpose from Android 10- or if still running on Android 10-
        if((permission.equals(Manifest.permission.READ_EXTERNAL_STORAGE) || permission.equals(Manifest.permission.WRITE_EXTERNAL_STORAGE)) && android.os.Build.VERSION.SDK_INT >= 33)
            return true;
        String [] permissions = new String[]{ permission };
        if(!hasPermission(NativeUtility.getMainActivity(), permissions)) {
            currentCustomRequest.put(lastID, permission);
            ActivityCompat.requestPermissions(NativeUtility.getMainActivity(),
                    permissions,
                    lastID);
            lastID++;
            return false;
        }
        return true;
    }

    @SuppressWarnings("unused")
    public static boolean requestPermission(int permissionValue)
    {
        // It's no longer possible to use those permissions on Android 13+
        // Code requesting that should already be migrated to use app internal storage on Android 11+
        // and only actually need it for migration purpose from Android 10- or if still running on Android 10-
        if(permissionValue == EXTERNAL_STORAGE_PERMISSION && android.os.Build.VERSION.SDK_INT >= 33)
            return true;
        String[] permissions = getPermissionString(permissionValue);
        if (!hasPermission(permissionValue)) {
            // Permission is not granted, ask for it
            currentRequest.put(lastID, permissionValue);
            ActivityCompat.requestPermissions(NativeUtility.getMainActivity(),
                    permissions,
                    lastID);
            lastID++;
            return false;
        }
        return true;
    }

    public static void onRequestPermissionsResult(int requestID,
                                                  @SuppressWarnings({"unused", "CStyleArrayDeclaration"}) String permissions[],
                                                  int[] grantResults) {
        if(NativeUtility.getMainActivity() != null)
        {
            int permissionId = currentRequest.get(requestID, -1);
            if(permissionId != -1) {
                NativeUtility.getMainActivity().runOnGLThread(() -> {
                    notifyPermissionRequestEnded(currentRequest.get(requestID),
                            grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED);
                    currentRequest.delete(requestID);
                });
                return;
            }
            if(currentCustomRequest.containsKey(requestID)) {
                NativeUtility.getMainActivity().runOnGLThread(() -> {
                    notifyCustomPermissionRequestEnded(currentCustomRequest.get(requestID),
                            grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED);
                    currentCustomRequest.remove(requestID);
                });
            }
        }

    }

    //Synchronized with C++ enum class Permission
    private static String[] getPermissionString(int value){
        switch (value) {
            case EXTERNAL_STORAGE_PERMISSION:
                return new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
            case AUDIO_PERMISSION:
                return new String[]{Manifest.permission.RECORD_AUDIO};
            case CAMERA_PERMISSION:
                return new String[]{Manifest.permission.CAMERA};
            case PHONE_STATE_PERMISSION:
                return new String[]{Manifest.permission.READ_PHONE_STATE};
            default:
                throw new AssertionError("Unknown permission, did you fill the Java part after updating enum class Permission?");
        }
    }
}
