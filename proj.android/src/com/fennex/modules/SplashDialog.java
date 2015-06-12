package com.fennex.modules;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;

/**
 * Created by Fradow on 12/06/15.
 */
public class SplashDialog extends Dialog {
    private int activityID;
    public SplashDialog(Context context, int activity) {
        super(context, android.R.style.Theme_NoTitleBar_Fullscreen);
        activityID = activity;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(activityID);
        setCancelable(false);
    }
}