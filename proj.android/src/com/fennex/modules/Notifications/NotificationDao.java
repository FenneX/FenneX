package com.fennex.modules.Notifications;
import androidx.room.*;

import java.util.List;

@Dao
public interface NotificationDao {
    @Query("SELECT * FROM NotificationInformation")
    List<NotificationInformation> getAll();

    @Query("SELECT * FROM NotificationInformation WHERE uid IN (:ids)")
    List<NotificationInformation> loadAllByIds(int[] ids);

    @Query("SELECT * FROM NotificationInformation WHERE uid IS (:id)")
    List<NotificationInformation> loadAllById(int id);

    @Query("SELECT * FROM NotificationInformation WHERE timestamp IS (:timestamp)")
    List<NotificationInformation> loadAllByTimestamp(int timestamp);

    @Insert
    void insertAll(NotificationInformation... users);

    @Delete
    void delete(NotificationInformation user);
}