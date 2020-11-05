package com.fennex.modules;

import android.app.AlarmManager;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;

import com.fennex.modules.notifications.NotificationDatabase;
import com.fennex.modules.notifications.NotificationInformation;
import com.fennex.modules.notifications.NotificationPublisher;

@SuppressWarnings("unused")
public class NotificationHandler {

    private static final String TAG = "NotificationHandler";

    private static int nextId = 0;

    private static String _channelId = "";

    public static void createNotificationChannel(String name, String description, String channelId) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
            && !_channelId.isEmpty()
            && !_channelId.equals(channelId)) {
                throw new AssertionError("A notification channel already exist with a different name");
        }

        _channelId = channelId;
        // Create the NotificationChannel, but only on API 26+ because
        // the NotificationChannel class is new and not in the support library
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            Context context = NativeUtility.getMainActivity();

            int importance = NotificationManager.IMPORTANCE_DEFAULT;
            NotificationChannel channel = new NotificationChannel(channelId, name, importance);
            channel.setDescription(description);
            // Register the channel with the system; you can't change the importance
            // or other notification behaviors after this
            NotificationManager notificationManager = context.getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(channel);
            } else {
                throw new AssertionError("couldn't get Notification manager");
            }
        }
    }

    public static void planNotification(long timestamp, String text, String title, String url, int notificationId) {
        Context context = NativeUtility.getMainActivity();
        NotificationInformation notification = new NotificationInformation(
                timestamp,
                text,
                title,
                url,
                notificationId,
                _channelId,
                NativeUtility.getMainActivity().getSmallIcon());

        // Create the AlarmManager to launch notification when needed
        AlarmManager alarms = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);

        planNotification(context, alarms, notification);
        // Save notification in case of a reboot
        NotificationDatabase.saveNotification(context, notification);
    }

    /**
     *
     * @param context a context to help create intents
     * @param alarms pass the alarm Manager here to avoid demanding it multiple times
     * @param information the actual notification information you want to launch
     */
    public static void planNotification(Context context, AlarmManager alarms, NotificationInformation information) {
        if (alarms == null) {
            throw new AssertionError("couldn't get alarm manager");
        }
        // Set up the intent to start the service
        Intent intent = new Intent(context, NotificationPublisher.class);
        intent.putExtra("text", information.text);
        intent.putExtra("title", information.title);
        intent.putExtra("url", information.url);
        intent.putExtra("channelId", information.channelId);
        intent.putExtra("smallIcon", information.smallIcon);
        intent.putExtra("uid", information.uid);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, nextId, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        nextId++;

        // Schedule the notification (timestamp is in s, we need it in ms)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarms.setAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, information.timestamp * 1000, pendingIntent);
        }
        else {
            alarms.set(AlarmManager.RTC_WAKEUP, information.timestamp * 1000, pendingIntent);
        }
    }
}
