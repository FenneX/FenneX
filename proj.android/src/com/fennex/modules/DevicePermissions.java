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
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.SparseIntArray;

public class DevicePermissions
{
    public native static void notifyPermissionRequestEnded(int permission, boolean result);

    private static SparseIntArray currentRequest = new SparseIntArray();
    private static int lastID = 0;

    @SuppressWarnings("unused,WeakerAccess")
    public static boolean hasPermission(int permissionValue)
    {
        return hasPermission(NativeUtility.getMainActivity(), permissionValue);
    }

    @SuppressWarnings("WeakerAccess")
    public static boolean hasPermission(Context context, int permissionValue)
    {
        String[] permissions = getPermissionString(permissionValue);
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
    public static boolean requestPermission(int permissionValue)
    {
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
                                                  @SuppressWarnings("unused") String permissions[],
                                                  int[] grantResults) {
        if(NativeUtility.getMainActivity() != null)
        {
            NativeUtility.getMainActivity().runOnGLThread(() -> {
                notifyPermissionRequestEnded(currentRequest.get(requestID),
                        grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED);
                currentRequest.delete(requestID);
            });
        }

    }

    //Syncronized with C++ enum class Permission
    private static String[] getPermissionString(int value){
        switch (value) {
            case 0:
                return new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
            case 1:
                return new String[]{Manifest.permission.RECORD_AUDIO};
            case 2:
                return new String[]{Manifest.permission.CAMERA};
            case 3:
                return new String[]{Manifest.permission.READ_PHONE_STATE};
            default:
                throw new AssertionError("Unknown permission, did you fill the Java part after updating enum class Permission?");
        }
    }
}
