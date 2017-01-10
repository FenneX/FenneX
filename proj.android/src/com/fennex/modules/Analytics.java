package com.fennex.modules;

import android.os.Bundle;
import android.util.Log;

import com.google.analytics.tracking.android.EasyTracker;
import com.google.analytics.tracking.android.Fields;
import com.google.analytics.tracking.android.GoogleAnalytics;
import com.google.analytics.tracking.android.Logger;
import com.google.analytics.tracking.android.MapBuilder;
import com.google.analytics.tracking.android.Tracker;
import com.google.firebase.analytics.FirebaseAnalytics;

/**
 * Created by admin on 30/09/16.
 */
public class Analytics {

    public static FirebaseAnalytics mFirebaseAnalytics;

    /**
     * Google Analytics methods.
     */
    static public void GAStartSession(String apiKey){
        Log.i("Google Analytics", "GAStartSession");
        if(NativeUtility.getMainActivity() != null)
        {
            EasyTracker.getInstance(NativeUtility.getMainActivity()).activityStart(NativeUtility.getMainActivity());
            EasyTracker.getInstance(NativeUtility.getMainActivity()).set(Fields.TRACKING_ID, apiKey);
        }
    }

    static public void GAEndSession(){
        Log.i("Google Analytics", "GAEndSession");
        if(NativeUtility.getMainActivity() != null)
        {
            EasyTracker.getInstance(NativeUtility.getMainActivity()).activityStop(NativeUtility.getMainActivity());
        }
    }

    static public void GALogPageView(String pageName){
        Log.i("Google Analytics", "GALogPageView: " + pageName);
        if(NativeUtility.getMainActivity() != null)
        {
            Tracker easyTracker = EasyTracker.getInstance(NativeUtility.getMainActivity());

            easyTracker.set(Fields.SCREEN_NAME, pageName);

            easyTracker.send(MapBuilder
                    .createAppView()
                    .build()
            );
        }
    }

    static public void GALogEvent(String eventName, String label, int value){
        Log.i("Google Analytics", "GALogEvent: eventName-> " + eventName + " | label-> " + label + " | value-> " + value);
        if(NativeUtility.getMainActivity() != null)
        {
            Tracker easyTracker = EasyTracker.getInstance(NativeUtility.getMainActivity());

            easyTracker.send(MapBuilder
                    .createEvent(eventName,			// Event category (required)
                            "button_press",		// Event action (required)
                            label,   			// Event label
                            (long) value)       // Event value
                    .build()
            );
        }
    }

    static public void GASetSessionContinueSeconds(int seconds){
        Log.i("Google Analytics", "GASetSessionContinueSeconds is set in res/value/analytics.xml");
    }

    static public void GASetDebugLogEnabled(boolean value){
        Log.i("Google Analytics", "GASetDebugLogEnabled: " + value);
        if(NativeUtility.getMainActivity() != null)
        {
            if(value)
                GoogleAnalytics.getInstance(NativeUtility.getMainActivity()).getLogger().setLogLevel(Logger.LogLevel.VERBOSE);
            else
                GoogleAnalytics.getInstance(NativeUtility.getMainActivity()).getLogger().setLogLevel(Logger.LogLevel.ERROR);
        }
    }

    static public void GASetAppVersion(String version){
        Log.i("Google Analytics", "GASetAppVersion: " + version);
        if(NativeUtility.getMainActivity() != null)
        {
            Tracker easyTracker = EasyTracker.getInstance(NativeUtility.getMainActivity());
            easyTracker.set(Fields.APP_VERSION, version);
        }
    }

    static public void GASetTrackExceptionsEnabled(boolean value) {
        Log.i("Google Analytics", "GASetTrackExceptionsEnabled is set in res/value/analytics.xml");
    }

    static public void GASetSecureTransportEnabled(boolean value) {
        Log.i("Google Analytics", "GASetSecureTransportEnabled: " + value);
        if(NativeUtility.getMainActivity() != null)
        {
            Tracker easyTracker = EasyTracker.getInstance(NativeUtility.getMainActivity());
            easyTracker.set(Fields.USE_SECURE, String.valueOf(value));
        }
    }

    static public void firebaseSetProperty(String propertyName, String propertyValue){
        mFirebaseAnalytics.setUserProperty(propertyName, propertyValue);
    }

    static public void firebaseLogPageView(String pageName){
        firebaseLogEventWithParameters("change_scene", FirebaseAnalytics.Param.ITEM_NAME, pageName);
    }

    static public void firebaseLogEvent(String eventName) {
        mFirebaseAnalytics.logEvent(eventName.replace(' ', '_').replace('-', '_'), new Bundle());
    }

    static public void firebaseLogEventWithParameters(String eventName,  String label, String value) {
        Bundle bundle = new Bundle();
        bundle.putString(label, value);
        mFirebaseAnalytics.logEvent(eventName.replace(' ', '_').replace('-', '_'), bundle);
    }
}
