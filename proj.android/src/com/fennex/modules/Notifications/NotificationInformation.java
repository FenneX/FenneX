package com.fennex.modules.Notifications;

import androidx.room.*;

@Entity
public class NotificationInformation {
    @PrimaryKey(autoGenerate = true)
    public int uid;

    @ColumnInfo(name = "timestamp")
    public long timestamp;

    @ColumnInfo(name = "text")
    public String text;

    @ColumnInfo(name = "url")
    public String url;

    @ColumnInfo(name = "channelId")
    public String channelId;

    @ColumnInfo(name = "smallIcon")
    public int smallIcon;

    public NotificationInformation(long timestamp, String text, String url, String channelId, int smallIcon)
    {
        this.timestamp = timestamp;
        this.text = text;
        this.url = url;
        this.channelId = channelId;
        this.smallIcon = smallIcon;
    }
}
