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
import android.os.Build;
import android.provider.Settings;
import android.util.Log;
import android.util.SparseArray;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;

import okhttp3.OkHttpClient;

/* Require permission <uses-permission android:name="android.permission.WAKE_LOCK" /> for downloadFile
 */
public class NetworkUtility
{
    //Provides various clients that are compatible with Let's Encrypt CA, even if the device doesn't have
    //ISRG Root X1 certificate, which is the case on all Android < 7.1.1
    public static TrustManagerFactory getTrustManagerFactory() throws NoSuchAlgorithmException, KeyStoreException, IOException, CertificateException {
        //Note: hardcode it, because the device might not even have the certificate to download it over https
        String isgCert =
                "-----BEGIN CERTIFICATE-----\n" +
                        "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" +
                        "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" +
                        "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" +
                        "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" +
                        "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" +
                        "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" +
                        "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" +
                        "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" +
                        "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" +
                        "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" +
                        "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" +
                        "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" +
                        "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" +
                        "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" +
                        "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" +
                        "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" +
                        "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" +
                        "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" +
                        "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" +
                        "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" +
                        "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" +
                        "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" +
                        "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" +
                        "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" +
                        "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" +
                        "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" +
                        "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" +
                        "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" +
                        "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" +
                        "-----END CERTIFICATE-----";

        CertificateFactory cf = CertificateFactory.getInstance("X.509");
        Certificate isgCertificate = cf.generateCertificate(new ByteArrayInputStream(isgCert.getBytes(StandardCharsets.UTF_8)));

        // Create a KeyStore containing our trusted CAs
        KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
        keyStore.load(null, null);
        keyStore.setCertificateEntry("isrg_root", isgCertificate);

        //Default TrustManager to get device trusted CA
        TrustManagerFactory defaultTmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        defaultTmf.init((KeyStore) null);

        X509TrustManager trustManager = (X509TrustManager) defaultTmf.getTrustManagers()[0];
        int number = 0;
        for(Certificate cert : trustManager.getAcceptedIssuers()) {
            keyStore.setCertificateEntry(Integer.toString(number), cert);
            number++;
        }

        // Create a TrustManager that trusts the CAs in our KeyStore
        TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        tmf.init(keyStore);
        return tmf;
    }
    public static OkHttpClient getHttpClient() throws CertificateException, IOException, KeyStoreException, NoSuchAlgorithmException, KeyManagementException {
        //Code from https://stackoverflow.com/questions/64844311/certpathvalidatorexception-connecting-to-a-lets-encrypt-host-on-android-m-or-ea
        OkHttpClient.Builder builder = new OkHttpClient.Builder();

        if (Build.VERSION.SDK_INT <= 25) {
            TrustManagerFactory tmf = getTrustManagerFactory();
            SSLContext context = SSLContext.getInstance("TLS");
            context.init(null, tmf.getTrustManagers(), null);
            builder.sslSocketFactory(context.getSocketFactory(), (X509TrustManager) tmf.getTrustManagers()[0]);
        }
        return builder.build();
    }

    /** Download buffer size. 1024 is a sweet spot. */
    private static final int DOWNLOAD_BUFFER_SIZE = 1024;

    /** Sending the download's progress at most X time per second */
    private static final int PROGRESS_UPDATE_PER_SECOND = 15;

    public native static void notifySuccess(int downloadID);
    public native static void notifyError(int downloadID, int errorCode, String errorResponse);
    public native static void notifyProgressUpdate(int downloadID, long current, long total);
    public native static void notifyLengthResolved(int downloadID, long total);

    private static Integer currentlyDownloadingTask = null;
    private static SparseArray<Quartet<String, String, String, String>> waitingDownloadTasks = new SparseArray<>();

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
    public static void downloadFile(int downloadId, String url, String savePath, String authorizationHeader, String sessionCookie) {
        if(currentlyDownloadingTask == null) {
            Log.d(TAG, "Starting download immediately for " + url);
            currentlyDownloadingTask = downloadId;
            Thread thread = new Thread() {
                @Override
                public void run() {
                    downloadFileImpl(downloadId, url, savePath, authorizationHeader, sessionCookie);
                }
            };
            thread.start();
        }
        else {
            Log.d(TAG, "Adding download task for " + url + ", already " + waitingDownloadTasks.size() + " waiting");
            waitingDownloadTasks.put(downloadId, new Quartet<>(url, savePath, authorizationHeader, sessionCookie));
        }
    }

    private static void launchNextTask() {
        if(waitingDownloadTasks.size() > 0) {
            currentlyDownloadingTask = waitingDownloadTasks.keyAt(0);
            Quartet<String, String, String, String> taskInfos = waitingDownloadTasks.get(currentlyDownloadingTask);
            waitingDownloadTasks.delete(currentlyDownloadingTask);
            Log.d(TAG, "Launching download task for " + taskInfos.first + ", there are " + waitingDownloadTasks.size() + " tasks waiting in queue");
            downloadFileImpl(currentlyDownloadingTask, taskInfos.first, taskInfos.second, taskInfos.third, taskInfos.fourth);
        }
        else {
            currentlyDownloadingTask = null;
        }
    }

    private static void downloadFileImpl(int downloadId, String url, String savePath, String authorizationHeader, String sessionCookie) {
        Log.d(TAG,"Starting download of " + url);

        //Ensure parent directory exists
        int lastIndexOf = savePath.lastIndexOf("/");
        if(lastIndexOf != -1) {
            //noinspection ResultOfMethodCallIgnored
            new File(savePath.substring(0, lastIndexOf + 1)).mkdirs();
        }


        OkHttpClient client;
        try {
            client = getHttpClient();
        } catch (CertificateException | KeyManagementException | IOException | KeyStoreException | NoSuchAlgorithmException e) {
            e.printStackTrace();
            NativeUtility.getMainActivity().runOnGLThread(() -> notifyError(downloadId, -1, "Could not create OkHttpClient"));
            launchNextTask();
            return;
        }
        okhttp3.Request.Builder builder = new okhttp3.Request.Builder()
                .method("GET", null)
                .url(url);
        if(!sessionCookie.isEmpty()) {
            builder = builder.addHeader("Cookie", sessionCookie);
        }
        else if(!authorizationHeader.isEmpty()) {
            builder = builder.addHeader("Authorization", authorizationHeader);
        }
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
