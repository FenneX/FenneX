package com.fennex.modules;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

/**
 * Created by Fradow on 12/06/15.
 */
public class SplashDialog extends Dialog {
    private int activityID;
    private int splashID;
    private Context context;
    public SplashDialog(Context context, int activity, int splashImageID) {
        super(context, android.R.style.Theme_NoTitleBar_Fullscreen);
        this.context = context;
        activityID = activity;
        splashID = splashImageID;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(activityID);
        //Fade in only the image, so the background is the right color
        ImageView image = (ImageView)findViewById(splashID);
        Animation myFadeInAnimation = AnimationUtils.loadAnimation(context, android.R.anim.fade_in);
        image.startAnimation(myFadeInAnimation);
        setCancelable(false);
    }

    public void discard()
    {
        context = null;
        this.cancel();
    }
}