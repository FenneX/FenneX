package com.fennex.modules.notifications;
import android.content.Context;

import androidx.room.*;

import java.util.List;

/**
 * This class is the point of access to the room database for external classes
 * For more information about room, see :
 * https://developer.android.com/training/data-storage/room
 * Try to avoid calling the database from the main thread as it can block the UI
 */
@Database(entities = {NotificationInformation.class}, version = 1)
public abstract class NotificationDatabase extends RoomDatabase  {

    public abstract NotificationDao notificationDao();

    private static NotificationDatabase instance = null;

    private static NotificationDatabase getDatabase(Context context) {
        if(instance == null) {
            instance = Room.databaseBuilder(context, NotificationDatabase.class, "notification-database")
                    .enableMultiInstanceInvalidation()
                    .allowMainThreadQueries()
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
        getDatabase(context).notificationDao().deleteById(uid);
    }
}
