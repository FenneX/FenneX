package com.fennex.modules.notifications;

import android.app.IntentService;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;

public class ShowNotification extends IntentService {
    private final static String TAG = "ShowNotification";
    private static int mId = 0;

    public ShowNotification(String name) {
        super(name);
    }

    @Override
    public void onHandleIntent(Intent intent)
    {
        String text = intent.getExtras().getString("text");
        String url = intent.getExtras().getString("url");

        // Set up the main action intent
        Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(url));

        // Set up the pending intent from the main intent
        PendingIntent pendingIntent = PendingIntent.getActivity(this, mId, i, PendingIntent.FLAG_UPDATE_CURRENT);
        // Create the notification
        Notification notification = new Notification.Builder(this)
                .setContentText(text)
                .setContentIntent(pendingIntent)
                .build();
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify((int) mId, notification);
        mId++;
    }
}
