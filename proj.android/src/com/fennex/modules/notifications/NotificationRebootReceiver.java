package com.fennex.modules.notifications;

import android.app.AlarmManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.fennex.modules.NotificationHandler;

import java.util.List;

/**
 * Android does not keep pending notifications when the device is turned off
 * This BroadcastReceiver is used to recreate all the pending notifications when the device is turned on again
 * All notifications are saved using a room database (see NotificationDatabase) when created so that when the device is turned on
 * NotificationRebootReceiver can access them all and recreate the ones that are in the future
 */
public class NotificationRebootReceiver  extends BroadcastReceiver {

    private static final String BOOT_ACTION = "android.intent.action.BOOT_COMPLETED";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (BOOT_ACTION.equals(intent.getAction())) {
            final PendingResult result = goAsync();
            Thread thread = new Thread() {
                public void run() {
                    refreshNotifications(context);
                    result.finish();
                }
            };
            thread.start();
        }
    }

    private void refreshNotifications(Context context) {
        // timestamp in seconds
        long now = System.currentTimeMillis() / 1000;
        List<NotificationInformation> notifications = NotificationDatabase.getNotifications(context);
        // Create the AlarmManager to launch notifications when needed
        AlarmManager alarms = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        for(NotificationInformation notification : notifications) {
            if(notification.timestamp > now) {
                NotificationHandler.planNotification(
                        context,
                        alarms,
                        notification);
            }
            else {
                // erase past notification
                NotificationDatabase.removeNotification(context, notification);
            }
        }
    }
}
