/****************************************************************************
AnalyticX: https://github.com/diwu/AnalyticX

Copyright (c) 2012 - Di Wu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************///

package com.diwublog.AnalyticX;

import java.util.HashMap;

import android.content.Context;
import android.util.Log;

import com.fennex.modules.NativeUtility;
import com.flurry.android.Constants;
import com.flurry.android.FlurryAgent;
import com.google.analytics.tracking.android.EasyTracker;
import com.google.analytics.tracking.android.Fields;
import com.google.analytics.tracking.android.GoogleAnalytics;
import com.google.analytics.tracking.android.Logger.LogLevel;
import com.google.analytics.tracking.android.MapBuilder;
import com.google.analytics.tracking.android.Tracker;

public class AnalyticXBridge {

	public static Context sessionContext;

	static int Bridge (String arg0, String arg1, String arg2) {

		if (arg0.equalsIgnoreCase("flurryLogEvent")) {
			return AnalyticXBridge.flurryLogEvent(arg1);
		} else if (arg0.equalsIgnoreCase("flurryLogEventTimed")) {
			return AnalyticXBridge.flurryLogEventTimed(arg1, arg2);
		} else if (arg0.equalsIgnoreCase("flurryEndTimedEvent")) {
			AnalyticXBridge.flurryEndTimedEvent(arg1);
		} else if (arg0.equalsIgnoreCase("flurryLogPageView")) {
			AnalyticXBridge.flurryLogPageView();
		} else if (arg0.equalsIgnoreCase("flurrySetAppVersion")) {
			AnalyticXBridge.flurrySetAppVersion(arg1);
		} else if (arg0.equalsIgnoreCase("flurrySetLogEnabled")) {
			AnalyticXBridge.flurrySetLogEnabled(arg2);
		} else if (arg0.equalsIgnoreCase("flurrySetSecureEnabled")) {
			AnalyticXBridge.flurrySetSecureEnabled(arg2);
		} else if (arg0.equalsIgnoreCase("flurryOnStartSession")) {
			AnalyticXBridge.flurryOnStartSession(arg1);
		} else if (arg0.equalsIgnoreCase("flurryOnEndSession")) {
			AnalyticXBridge.flurryOnEndSession();
		} else if (arg0.equalsIgnoreCase("flurrySetUserID")) {
			AnalyticXBridge.flurrySetUserID(arg1);
		} else if (arg0.equalsIgnoreCase("flurrySetGender")) {
			AnalyticXBridge.flurrySetGender(arg1);
		} else if (arg0.equalsIgnoreCase("flurryReportLocation")) {
			AnalyticXBridge.flurrySetReportLocation(arg2);
		}
		return 1;
	}

	static int Bridge (String arg0, String [] arg1, String arg2) {
		Log.v("diwu", "string array count = " + arg1.length);
		String[] splitedString = arg0.split(",");
		if (splitedString[0].equalsIgnoreCase("flurryLogEventWithParameters")) {
			return AnalyticXBridge.flurryLogEventWithParameters(splitedString[1], arg1);
		} else if (arg0.equalsIgnoreCase("flurryLogEventWithParametersTimed")) {
			return AnalyticXBridge.flurryLogEventWithParametersTimed(arg0, arg1, arg2);
		}
		return 1;
	}

	static private int flurryLogEvent(String eventID) {
		Log.v("diwu", "flurryLogEvent() is called... eventID = " + eventID);
		return FlurryAgent.logEvent(eventID).ordinal();
	}

	static private int flurryLogEventTimed(String eventID, String timed) {
		boolean timedBool = false;
		if (timed.equalsIgnoreCase("false")) {
			timedBool = false;
		} else if (timed.equalsIgnoreCase("true")) {
			timedBool = true;
		}
		Log.v("diwu", "flurryLogEventTimed(), eventID = " + eventID + ", timed = " + timedBool);
		return FlurryAgent.logEvent(eventID, timedBool).ordinal();
	}

	static private int flurryLogEventWithParameters(String eventID, String [] parametersArray) {
		Log.v("diwu", "flurryLogEventWithParameters... event id = " + eventID);

		HashMap<String, String> someMap = new HashMap<String, String>();

		for (int i = 0; i < parametersArray.length / 2; i++) {
			Log.v("diwu", "elm" + i + " = " + parametersArray[i]);
			Log.v("diwu", parametersArray[2*i] + " = " + parametersArray[2*i+1]);
			someMap.put(parametersArray[2*i], parametersArray[2*i+1]);
		}

		return FlurryAgent.logEvent(eventID, someMap).ordinal();
	}

	static private int flurryLogEventWithParametersTimed(String eventID, String [] parametersArray, String timed) {

		HashMap<String, String> someMap = new HashMap<String, String>();

		for (int i = 0; i < parametersArray.length / 2; i++) {
			Log.v("diwu", "total = " +  parametersArray.length / 2 + " ;elm" + i + " = " + parametersArray[2*i]);
			someMap.put(parametersArray[2*i], parametersArray[2*i+1]);
		}

		boolean timedBool = false;

		if (timed.equalsIgnoreCase("true")) {
			timedBool = true;
		}

		Log.v("diwu", "flurryLogEventWithParametersTimed... event id = " + eventID + " timed = " + timedBool);

		return FlurryAgent.logEvent(eventID, someMap, timedBool).ordinal();
	}

	static private void flurryEndTimedEvent(String eventID) {
		Log.v("diwu", "end timed event with id = " + eventID);
		FlurryAgent.endTimedEvent(eventID);
	}

	static private void flurryLogPageView() {
		Log.v("diwu", "log page view()...");
		FlurryAgent.onPageView();
	}

	static private void flurrySetAppVersion(String version) {
		Log.v("diwu", "set App Version = " + version);
		FlurryAgent.setVersionName(version);
	}

	static public int flurryGetAgentVersion() {
		return FlurryAgent.getAgentVersion();
	}

	static private void flurrySetLogEnabled(String enabled) {
		if (enabled.equalsIgnoreCase("true")) {
			Log.v("diwu", "set log to true");
			FlurryAgent.setLogEnabled(true);
		} else {
			Log.v("diwu", "set log to false");
			FlurryAgent.setLogEnabled(false);
		}
	}

	static public void flurrySetSessionContinueSecond(int seconds) {
		Log.v("diwu", "is set sessionn continue seconds to " + seconds);
		FlurryAgent.setContinueSessionMillis(seconds * 1000);
	}

	static private void flurrySetSecureEnabled(String enabled) {
		boolean enabledBool = false;
		if (enabled.equalsIgnoreCase("true")) {
			enabledBool = true;
		}

		Log.v("diwu", "https is " + enabledBool);

		//FlurryAgent.setUseHttps(enabledBool);
	}

	static private void flurryOnStartSession(String apiKey) {
		Log.v("diwu", "is trying to start session... " + apiKey);
		FlurryAgent.init(sessionContext, apiKey);
		FlurryAgent.onStartSession(sessionContext);
	}

	static private void flurryOnEndSession() {
		Log.v("diwu", "flurry on end session");
		FlurryAgent.onEndSession(sessionContext);
	}

	static private void flurrySetUserID(String userID) {
		Log.v("diwu", "flurry set user id = " + userID);
		FlurryAgent.setUserId(userID);
	}

	static public void flurrySetAge(int age) {
		Log.v("diwu", "flurry set age = " + age);
		FlurryAgent.setAge(age);
	}

	static private void flurrySetGender(String gender) {
		if (gender.equalsIgnoreCase("male")) {
			Log.v("diwu", "flurry set gender: male");
			FlurryAgent.setGender(Constants.MALE);
		} else {
			Log.v("diwu", "flurry set gender: female");
			FlurryAgent.setGender(Constants.FEMALE);
		}
	}

	static private void flurrySetReportLocation(String reportLocation) {
		boolean reportLocationBool = false;

		if (reportLocation.equalsIgnoreCase("true")) {
			reportLocationBool = true;
		}

		Log.v("diwu", "reportLocation = "+ reportLocationBool);
		FlurryAgent.setReportLocation(reportLocationBool);
	}
	
	/**
	 * Google Analytics methods.
	 */
	static public void GAStartSession(String apiKey){
		Log.i("Google Analytics", "GAStartSession");
		EasyTracker.getInstance(sessionContext).activityStart(NativeUtility.getMainActivity());
		EasyTracker.getInstance(sessionContext).set(Fields.TRACKING_ID, apiKey);
	}
	
	static public void GAEndSession(){
		Log.i("Google Analytics", "GAEndSession");
		EasyTracker.getInstance(sessionContext).activityStop(NativeUtility.getMainActivity());
	}
	
	static public void GALogPageView(String pageName){
		Log.i("Google Analytics", "GALogPageView: " + pageName);
		
		Tracker easyTracker = EasyTracker.getInstance(sessionContext);

		easyTracker.set(Fields.SCREEN_NAME, pageName);

		easyTracker.send(MapBuilder
		    .createAppView()
		    .build()
		);
	}
	
	static public void GALogEvent(String eventName, String label, int value){
		Log.i("Google Analytics", "GALogEvent: eventName-> " + eventName + " | label-> " + label + " | value-> " + value);
		
		Tracker easyTracker = EasyTracker.getInstance(sessionContext);

		easyTracker.send(MapBuilder
				.createEvent(eventName,			// Event category (required)
							"button_press",		// Event action (required)
							label,   			// Event label
							(long) value)       // Event value
		      .build()
		  );
	}
	
	static public void GASetSessionContinueSeconds(int seconds){
		Log.i("Google Analytics", "GASetSessionContinueSeconds is set in res/value/analytics.xml");
	}
	
	static public void GASetDebugLogEnabled(boolean value){
		Log.i("Google Analytics", "GASetDebugLogEnabled: " + value);
		if(value)
			GoogleAnalytics.getInstance(sessionContext).getLogger().setLogLevel(LogLevel.VERBOSE);
		else
			GoogleAnalytics.getInstance(sessionContext).getLogger().setLogLevel(LogLevel.ERROR);
	}
	
	static public void GASetAppVersion(String version){
		Log.i("Google Analytics", "GASetAppVersion: " + version);
		Tracker easyTracker = EasyTracker.getInstance(sessionContext);
		easyTracker.set(Fields.APP_VERSION, version);
	}
	
	static public void GASetTrackExceptionsEnabled(boolean value) {
		Log.i("Google Analytics", "GASetTrackExceptionsEnabled is set in res/value/analytics.xml");
	}
	
	static public void GASetSecureTransportEnabled(boolean value) {
		Log.i("Google Analytics", "GASetSecureTransportEnabled: " + value);
		Tracker easyTracker = EasyTracker.getInstance(sessionContext);
		easyTracker.set(Fields.USE_SECURE, String.valueOf(value));
	}
}
