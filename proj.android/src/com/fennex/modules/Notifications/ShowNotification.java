package com.fennex.modules;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.IBinder;

import androidx.annotation.Nullable;

public class ShowNotification extends Service {
    private final static String TAG = "ShowNotification";
    private static int mId = 0;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // Set up the main action intent
        Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(url));

        // Set up the pending intent from the main intent
        PendingIntent pendingIntent = PendingIntent.getActivity(context, mId, i, PendingIntent.FLAG_UPDATE_CURRENT);
        mId++;

        // Create the notification
        Notification notification = new Notification.Builder(context)
                .setContentText(alertBody)
                .setContentIntent(pendingIntent)
                .build();
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
    }
}
