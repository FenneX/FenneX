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

package com.fennex.modules.deprecated;

import java.util.Calendar;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import android.app.IntentService;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import androidx.core.app.NotificationCompat;
import android.util.Log;

import com.fennex.modules.NativeUtility;


public class NotificationService extends IntentService{
	private float timeFromNow = 0;
	
	public NotificationService() {
		super("NotificationService");
		Log.i("LocalNotification", "Constructor");
	}  

	/**
	 * The IntentService calls this method from the default worker thread with
	 * the intent that started the service. When this method returns, IntentService
	 * stops the service, as appropriate.
	 */
	@Override
	protected void onHandleIntent(final Intent intent) {
		timeFromNow = intent.getExtras().getFloat("timeFromNow");
		Log.i("LocalNotification", "scheduleNotification -> timeFromNow: " + timeFromNow);
    	Timer t = new Timer();  
		TimerTask task = new TimerTask() {
			@Override
			public void run() {  
				createNotif(intent);
			}
		};
		t.schedule(task, (long) (timeFromNow*1000));
	}

	@Override
	public int onStartCommand (Intent intent, int flags, int startId) {
		Log.i("LocalNotification", "onStartCommand");
		return super.onStartCommand(intent, flags, startId);
	}
	
	@Override
	public void onDestroy() {
		Log.i("LocalNotification", "onDestroy");
		super.onDestroy();
	}
	
	private void createNotif(Intent intent){
		int mId = 0;

		String alertBody = intent.getExtras().getString("alertBody");
		String alertAction = intent.getExtras().getString("alertAction");
		String soundName = intent.getExtras().getString("soundName");
		Object[] array = (Object[]) intent.getExtras().getSerializable("array");
		if(array == null)
			Log.i("LocalNotification", "array is null.");
		else
			Log.i("LocalNotification", "array is not null.");
		Log.i("LocalNotification", "scheduleNotification -> alertBody: " + alertBody + " alertAction: " + alertAction + " soundName: "+ soundName);
		Calendar c = Calendar.getInstance();

		Date endDate = new Date((long) (c.getTimeInMillis()+(timeFromNow*1000)));
		NotificationCompat.Builder mBuilder =
				new NotificationCompat.Builder(this)
		.setSmallIcon(NativeUtility.getMainActivity().getSmallIcon())
		.setContentTitle("Time In")
		.setContentText(alertBody)
		.setAutoCancel(true)
		.setWhen(endDate.getTime());

		Intent resultIntent = new Intent(this, NativeUtility.getMainActivity().getClass());

		resultIntent.putExtra("alertBody", alertBody);
		resultIntent.putExtra("alertAction", "alertAction");
		resultIntent.putExtra("soundName", soundName);
		resultIntent.putExtra("array", array);
		
		PendingIntent resultPendingIntent =
				PendingIntent.getActivity(
						this,
						0,
						resultIntent,
						PendingIntent.FLAG_UPDATE_CURRENT);

		mBuilder.setContentIntent(resultPendingIntent);
		
		NotificationManager mNotificationManager =
				(NotificationManager) this.
				getSystemService(Context.NOTIFICATION_SERVICE);
		mNotificationManager.notify(mId, mBuilder.build());
	}
}
