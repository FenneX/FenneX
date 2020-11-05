package com.fennex.modules.notifications;

import androidx.room.*;

@Entity
public class NotificationInformation {

    @PrimaryKey
    public int uid;

    @ColumnInfo(name = "timestamp")
    public long timestamp;

    @ColumnInfo(name = "text")
    public String text;

    @ColumnInfo(name = "title")
    public String title;

    @ColumnInfo(name = "url")
    public String url;

    @ColumnInfo(name = "channelId")
    public String channelId;

    @ColumnInfo(name = "smallIcon")
    public int smallIcon;

    public NotificationInformation(long timestamp, String text, String title, String url, int uid, String channelId, int smallIcon)
    {
        this.timestamp = timestamp;
        this.text = text;
        this.title = title;
        this.url = url;
        this.uid = uid;
        this.channelId = channelId;
        this.smallIcon = smallIcon;
    }
}
