package com.fennex.modules.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;

public class NotificationPublisher extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        final PendingResult result = goAsync();
        Thread thread = new Thread() {
            public void run() {
                publishNotification(context, intent);
                result.finish();
            }
        };
        thread.start();
    }

    // This function should be launched in a background thread as it access the notification database
    public void publishNotification(Context context, Intent intent)
    {
        // If we don't have an intent, there is nothing to do
        if(intent.getExtras() == null) return;

        String text = intent.getExtras().getString("text");
        String url = intent.getExtras().getString("url");
        int uid = intent.getExtras().getInt("uid");
        String channelId = intent.getExtras().getString("channelId");
        int smallIcon = intent.getExtras().getInt("smallIcon");

        // Set up the main action intent
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse(url));

        // Set up the pending intent from the main intent
        PendingIntent pendingIntent = PendingIntent.getActivity(context, uid, i, PendingIntent.FLAG_UPDATE_CURRENT);
        // Create the notification
        Notification.Builder notificationBuilder = new Notification.Builder(context)
                .setContentText(text)
                .setContentIntent(pendingIntent)
                .setSmallIcon(smallIcon)
                .setWhen(System.currentTimeMillis());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) notificationBuilder.setChannelId(channelId);

        Notification notification = notificationBuilder.build();

        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        if(notificationManager != null)
        {
            notificationManager.notify(uid, notification);
        }
        else {
            throw new AssertionError("couldn't get Notification manager");
        }

        //Erase saved notification
        NotificationDatabase.removeNotification(context, uid);
    }
}
