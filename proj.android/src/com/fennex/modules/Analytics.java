package com.fennex.modules;

import android.os.Bundle;
import com.google.firebase.analytics.FirebaseAnalytics;

/**
 * Created by admin on 30/09/16.
 */
@SuppressWarnings("unused")
public class Analytics {

    public static FirebaseAnalytics mFirebaseAnalytics;

    static public void firebaseSetProperty(String propertyName, String propertyValue){
        mFirebaseAnalytics.setUserProperty(propertyName, propertyValue);
    }

    static public void firebaseLogPageView(String pageName){
        firebaseLogEventWithParameters("change_scene", FirebaseAnalytics.Param.ITEM_NAME, pageName);
    }

    static public void firebaseLogEvent(String eventName) {
        mFirebaseAnalytics.logEvent(eventName.replace(' ', '_').replace('-', '_'), new Bundle());
    }

    @SuppressWarnings("WeakerAccess")
    static public void firebaseLogEventWithParameters(String eventName, String label, String value) {
        Bundle bundle = new Bundle();
        bundle.putString(label, value);
        mFirebaseAnalytics.logEvent(eventName.replace(' ', '_').replace('-', '_'), bundle);
    }
}
