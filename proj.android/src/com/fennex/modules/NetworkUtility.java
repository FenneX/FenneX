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
import android.os.AsyncTask;
import android.os.PowerManager;
import android.provider.Settings;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

/* Require permission <uses-permission android:name="android.permission.WAKE_LOCK" /> for downloadFile
 */
public class NetworkUtility
{
    public native static void notifySuccess(int downloadID);
    public native static void notifyError(int downloadID, int errorCode);
    public native static void notifyProgressUpdate(int downloadID, float percent);
    public native static void notifyLengthResolved(int downloadID, int length);
    private static final String TAG = "NetworkUtility";

    public static boolean isConnected(Context context)
    {
        ConnectivityManager connectivity = (ConnectivityManager) context.getApplicationContext().
                getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null)
        {
            NetworkInfo[] info = connectivity.getAllNetworkInfo();
            if (info != null)
                for (int i = 0; i < info.length; i++)
                    if (info[i].getState() == NetworkInfo.State.CONNECTED)
                    {
                        return true;
                    }
        }
        return false;
    }
    public static boolean isConnected()
    {
        return isConnected(NativeUtility.getMainActivity());
    }

    public static void openWifiSettings()
    {
        if(NativeUtility.getMainActivity() != null)
        {
            Intent intent = new Intent(Settings.ACTION_WIFI_SETTINGS);
            NativeUtility.getMainActivity().startActivity(intent);
        }
    }

    public static void downloadFile(int downloadID, String url, String savePath)
    {
        final DownloadTask downloadTask = new DownloadTask(downloadID, savePath);
        downloadTask.execute(url);
    }

    private static class DownloadTask extends AsyncTask<String, Float, Integer> {
        private PowerManager.WakeLock mWakeLock;
        private int downloadId;
        private String savePath;
        public DownloadTask(int downloadId, String savePath) {
            this.downloadId = downloadId;
            this.savePath = savePath;
        }

        @Override
        protected Integer doInBackground(String... sUrl) {
            InputStream input = null;
            OutputStream output = null;
            HttpURLConnection connection = null;
            try {
                URL url = new URL(sUrl[0]);
                connection = (HttpURLConnection) url.openConnection();

                connection.connect();

                // expect HTTP 200 OK, so we don't mistakenly save error report
                // instead of the file
                if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                    return 2;
                }

                // this will be useful to display download percentage
                // might be -1: server did not report the length
                final int fileLength = connection.getContentLength();
                NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                    public void run() {
                        notifyLengthResolved(downloadId, fileLength);
                    }
                });

                // download the file
                input = connection.getInputStream();
                new File(savePath.substring(0, savePath.lastIndexOf("/") + 1)).mkdirs();
                File saveFile = new File(savePath);
                if(!saveFile.exists()) {
                    saveFile.createNewFile();
                }
                output = new FileOutputStream(saveFile, false);

                byte data[] = new byte[4096];
                long total = 0;
                int count;
                while ((count = input.read(data)) != -1) {
                    // allow canceling with back button
                    if (isCancelled()) {
                        input.close();
                        return null;
                    }
                    total += count;
                    // publishing the progress....
                    if (fileLength > 0) // only if total length is known
                        publishProgress(((float)total / fileLength));
                    output.write(data, 0, count);
                }
            } catch (Exception e) {
                return 4;
            } finally {
                try {
                    if (output != null)
                        output.close();
                    if (input != null)
                        input.close();
                } catch (IOException ignored) {
                }

                if (connection != null)
                    connection.disconnect();
            }
            return null;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            // take CPU lock to prevent CPU from going off if the user
            // presses the power button during download
            PowerManager pm = (PowerManager) NativeUtility.getMainActivity().getSystemService(Context.POWER_SERVICE);
            mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                    getClass().getName());
            mWakeLock.acquire();
        }

        @Override
        public void onProgressUpdate(final Float... progress) {
            super.onProgressUpdate(progress);
            // if we get here, length is known. Only the latest progress is interesting to us
            NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                public void run() {
                    notifyProgressUpdate(downloadId, progress[progress.length - 1]);
                }
            });
        }

        @Override
        protected void onPostExecute(final Integer result) {
            mWakeLock.release();
            if (result != null) {
                NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                    public void run()
                    {
                        notifyError(downloadId, result);
                    }
                });
            }
            else {
                NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                    public void run() {
                        notifySuccess(downloadId);
                    }
                });
            }
        }
    }

}
