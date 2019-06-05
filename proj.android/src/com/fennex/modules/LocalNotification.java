/****************************************************************************
Copyright (c) 2013-2014 Auticiel SAS

http://www.fennex.org

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

package com.fennex.modules;

import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class LocalNotification{
	static int mId = 0;
	static Intent intent;
	public static void scheduleNotification(final float timeFromNow, final String alertBody, final String alertAction, final String soundName, final Object[] array){
		Log.i("LocalNotification", "Setting extras and starting service");
		intent = new Intent(NativeUtility.getMainActivity(), NotificationService.class);
		intent.putExtra("timeFromNow", timeFromNow);
		intent.putExtra("alertBody", alertBody);
		intent.putExtra("alertAction", "alertAction");
		intent.putExtra("soundName", soundName);
		intent.putExtra("array", array);
		if(array == null)
			Log.i("LocalNotification", "LocalNotification array is null");
		else
			Log.i("LocalNotification", "LocalNotification array is not null");
		NativeUtility.getMainActivity().startService(intent);
	}

	public static void cancelAllNotifications(){
		Log.i("LocalNotification", "cancelNotification");
		NotificationManager mNotificationManager = (NotificationManager) NativeUtility.getMainActivity().getSystemService(Context.NOTIFICATION_SERVICE);
		mNotificationManager.cancelAll();
	}
	
	public static void stopService() {
		Log.i("LocalNotification", "stopService");
		NativeUtility.getMainActivity().stopService(intent);
	}
	
	public static void onNewIntent(Intent intent){
		Log.i("TimeIn", "onNewIntent");
		if(intent.getExtras() != null) {
			String alertBody = intent.getExtras().getString("alertBody");
			String alertAction = intent.getExtras().getString("alertAction");
			String soundName = intent.getExtras().getString("soundName");
			final Object[] array = (Object[]) intent.getExtras().getSerializable("array");
			if(array != null)
			{
        		NativeUtility.getMainActivity().runOnGLThread(() -> notifyNotifClickedWrap(array));
				Log.i("TimeIn", "alertBody: " + alertBody + " alertAction: " + alertAction + " soundName: "+ soundName);
			}
		}
	}
	
	private native static void notifyNotifClickedWrap(Object[] infos);
	private native static void notifyDeletePListFiles();
}
