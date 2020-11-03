package com.fennex.modules.Notifications;
import android.content.Context;

import androidx.room.*;

import java.util.List;

@Database(entities = {NotificationInformation.class}, version = 1)
public abstract class NotificationDatabase extends RoomDatabase  {

    public abstract NotificationDao notificationDao();

    private static NotificationDatabase instance = null;

    private static NotificationDatabase getDatabase(Context context) {
        if(instance == null)
        {
            instance = Room.databaseBuilder(context, NotificationDatabase.class, "notification-database")
                    .enableMultiInstanceInvalidation()
                    .build();
        }
        return instance;
    }

    public static void saveNotification(Context context, NotificationInformation notification) {
        getDatabase(context).notificationDao().insertAll(notification);
    }

    public static List<NotificationInformation> getNotifications(Context context) {
        return getDatabase(context).notificationDao().getAll();
    }

    public static void removeNotification(Context context, NotificationInformation notification) {
        getDatabase(context).notificationDao().delete(notification);
    }

    public static void removeNotification(Context context, int uid) {
        List<NotificationInformation> notifications = getDatabase(context).notificationDao().loadAllById(uid);
        for(NotificationInformation notification : notifications)
        {
            removeNotification(context, notification);
        }
    }
}
