package com.fennex.modules;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.AnimationUtils;

/**
 * Created by Fradow on 12/06/15.
 */
public class SplashDialog extends Dialog {
    private int activityID;
    private Context context;
    public SplashDialog(Context context, int activity) {
        super(context, android.R.style.Theme_NoTitleBar_Fullscreen);
        activityID = activity;
        this.context = context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LayoutInflater inflator=getLayoutInflater();
        View view=inflator.inflate(activityID, null, false);
        view.startAnimation(AnimationUtils.loadAnimation(context, android.R.anim.fade_in));
        setContentView(view);
        setCancelable(false);
    }

    public void discard()
    {
        context = null;
        this.cancel();
    }
}