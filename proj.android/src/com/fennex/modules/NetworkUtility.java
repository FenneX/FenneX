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

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.provider.Settings;
import android.util.Log;
import android.util.Pair;
import android.util.SparseArray;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import okhttp3.OkHttpClient;

/* Require permission <uses-permission android:name="android.permission.WAKE_LOCK" /> for downloadFile
 */
public class NetworkUtility
{
    /** Download buffer size. 1024 is a sweet spot. */
    private static final int DOWNLOAD_BUFFER_SIZE = 1024;

    /** Sending the download's progress at most X time per second */
    private static final int PROGRESS_UPDATE_PER_SECOND = 5;

    public native static void notifySuccess(int downloadID);
    public native static void notifyError(int downloadID, int errorCode, String errorResponse);
    public native static void notifyProgressUpdate(int downloadID, long current, long total);
    public native static void notifyLengthResolved(int downloadID, long total);

    private static Integer currentlyDownloadingTask = null;
    private static SparseArray<Pair<String, String>> waitingDownloadTasks = new SparseArray<>();

    @SuppressWarnings("unused")
    private static final String TAG = "NetworkUtility";

    public static boolean isConnected(Context context) {
        ConnectivityManager connectivity = (ConnectivityManager) context.getApplicationContext().
                getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null) {
            NetworkInfo[] info = connectivity.getAllNetworkInfo();
            if (info != null)
                for (NetworkInfo anInfo : info)
                    if (anInfo.getState() == NetworkInfo.State.CONNECTED)
                        return true;
        }
        return false;
    }
    @SuppressWarnings("unused")
    public static boolean isConnected() {
        return isConnected(NativeUtility.getMainActivity());
    }

    @SuppressWarnings("unused")
    public static void openWifiSettings(){
        if(NativeUtility.getMainActivity() != null){
            Intent intent = new Intent(Settings.ACTION_WIFI_SETTINGS);
            NativeUtility.getMainActivity().startActivity(intent);
        }
    }

    @SuppressWarnings("unused")
    public static void downloadFile(int downloadId, String url, String savePath) {
        if(currentlyDownloadingTask == null) {
            currentlyDownloadingTask = downloadId;
            Thread thread = new Thread() {
                @Override
                public void run() {
                    downloadFileImpl(downloadId, url, savePath);
                }
            };
            thread.start();
        }
        else {
            waitingDownloadTasks.put(downloadId, new Pair<>(url, savePath));
        }
    }

    private static void launchNextTask() {
        if(waitingDownloadTasks.size() > 0) {
            currentlyDownloadingTask = waitingDownloadTasks.keyAt(0);
            Pair<String, String> taskInfos = waitingDownloadTasks.get(currentlyDownloadingTask);
            waitingDownloadTasks.delete(currentlyDownloadingTask);
            downloadFileImpl(currentlyDownloadingTask, taskInfos.first, taskInfos.second);
        }
    }

    private static void downloadFileImpl(int downloadId, String url, String savePath) {
        Log.d(TAG,"Starting download of " + url);
        OkHttpClient client = new OkHttpClient();
        okhttp3.Request.Builder builder = new okhttp3.Request.Builder()
                .method("GET", null)
                .url(url);
        try {
            okhttp3.Response response = client.newCall(builder.build()).execute();
            if(!response.isSuccessful()) {
                String message;
                try {
                    message = response.body() != null ? response.body().string() : "No error provided";
                } catch (IOException e) {
                    message = "Error unavailable due to IOException";
                }
                String finalMessage = message;
                NativeUtility.getMainActivity().runOnGLThread(() -> notifyError(downloadId, response.code(), finalMessage));
                launchNextTask();
                return;
            }
            assert response.body() != null;
            long knownLength = response.body().contentLength();
            NativeUtility.getMainActivity().runOnGLThread(() -> notifyLengthResolved(downloadId, knownLength));
            InputStream is = response.body().byteStream();

            BufferedInputStream input = new BufferedInputStream(is);
            OutputStream output = new FileOutputStream(new File(savePath));

            byte[] data = new byte[DOWNLOAD_BUFFER_SIZE];

            long total = 0;
            int count;
            long lastProgressUpdate = System.currentTimeMillis();

            while ((count = input.read(data)) != -1) {
                total += count;
                output.write(data, 0, count);

                //Rate-limit of progress updates
                if (System.currentTimeMillis() - lastProgressUpdate > 1000 / PROGRESS_UPDATE_PER_SECOND) {
                    final long currentDownloaded = total;
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyProgressUpdate(downloadId, currentDownloaded, knownLength));
                    lastProgressUpdate = System.currentTimeMillis();
                }
            }

            output.flush();
            output.close();
            input.close();
            NativeUtility.getMainActivity().runOnGLThread(() -> notifySuccess(downloadId));
            Log.d(TAG,"Download success of " + url);
        } catch (IOException e) {
            NativeUtility.getMainActivity().runOnGLThread(() -> notifyError(downloadId, -1, e.getMessage()));
            Log.d(TAG,"Download failure of " +url + " => " + e.getMessage());
        }
        launchNextTask();
    }
}
