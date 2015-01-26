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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

//import org.cocos2dx.lib.Cocos2dxBitmap;

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.AudioManager;
import android.media.ToneGenerator;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Vibrator;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.util.Log;
import android.view.WindowManager;

@TargetApi(Build.VERSION_CODES.HONEYCOMB)
public class NativeUtility 
{
    private static final String TAG = "NativeUtility";
    
    public native static void notifyMemoryWarning();
    public native static void notifyVolumeChanged();
    
	private static volatile ActivityResultNotifier mainActivity;    
	public static void setMainActivity(ActivityResultNotifier activity)
	{
		mainActivity = activity;
	}
	
	public static ActivityResultNotifier getMainActivity()
	{
		if(mainActivity == null)
		{
			Log.w(TAG, "Warning : null main Activity");
		}
		return mainActivity;
	}

    public static String getLocalPath()
    {
    	String localPath = getMainActivity().getFilesDir().getPath();
    	Log.d(TAG, "returning local path : " + localPath);
    	return localPath;
    }
    
    public static String getAppName()
    {
    	String appName = getMainActivity().getClass().getSimpleName();
    	Log.d(TAG, "returning app name : " + appName);
    	return appName;
    }

    public static String getPackageIdentifier()
    {
        String packageName = getMainActivity().getClass().getPackage().getName();
        Log.d(TAG, "returning app package identifier : " + packageName);
        return packageName;
    }

    public static String getUniqueIdentifier()
    {
        String packageName = Settings.Secure.getString(getMainActivity().getApplicationContext().getContentResolver(), Settings.Secure.ANDROID_ID);;
        Log.d(TAG, "returning app package identifier : " + packageName);
        return packageName;
    }
    
    public static boolean isPhone()
    {
    	boolean result = ((getMainActivity().getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) < Configuration.SCREENLAYOUT_SIZE_LARGE);
    	Log.i(TAG, "is phone ? " + (result ? "yes" : "no"));
    	return result;
    	/*boolean xlarge = ((getMainActivity().getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == 4);
    	boolean large = ((getMainActivity().getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE);
    	return (xlarge || large);*/
    }
    
    public static void copyResourceFileToLocal(String path)
    {
    	Log.d(TAG, "copying resource file to local : " + path);
        File destinationFile = new File(NativeUtility.getLocalPath() + java.io.File.separator + path);    
    	if(!destinationFile.exists())
    	{
         	Log.i(TAG, "File doesn't exist, doing the copy");
            AssetManager am = NativeUtility.getMainActivity().getAssets();
            try { 
                InputStream in = am.open(path);
                FileOutputStream f = new FileOutputStream(destinationFile); 
                byte[] buffer = new byte[1024];
                int len1 = 0;
                while ((len1 = in.read(buffer)) > 0) {
                    f.write(buffer, 0, len1);
                }
                f.close();
             	Log.i(TAG, "Copied " + path + " to " + NativeUtility.getLocalPath() + java.io.File.separator + path);
            } catch (IOException e) {
                e.printStackTrace();
            }
    	}
    	else
        {
         	Log.i(TAG, "didn't copied" + path + ", already exist");            	
        }
    }
    
    public static void preventIdleTimerSleep(boolean prevent)
    {
        //Those operations must be executed on the main thread, otherwise there is an exception
    	getMainActivity().runOnUiThread(prevent ? new Runnable() 
    	{
    	     @Override
    	     public void run() 
    	     {
    	     		Log.i(TAG, "Preventing app from sleep");
    	     		getMainActivity().getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    	    }
    	}
    	: new Runnable() 
    	{
   	     @Override
   	     public void run() 
   	     {
   	     		Log.i(TAG, "Stop preventing app from sleep");
   	     		getMainActivity().getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
   	    }
   	});
    }
    
    @TargetApi(19)
    public static void startSceneInitialisation()
    {
    	boolean enableCaching = true;
		ActivityManager am = (ActivityManager) getMainActivity().getSystemService("activity");
		//Disable Bitmap caching for low ram devices, as it will receive too much low memory notifications
    	if(android.os.Build.VERSION.SDK_INT >= 19)
    	{
    		enableCaching = ! am.isLowRamDevice();
    	}
    	//Cocos2dxBitmap.setIsInInitialization(enableCaching);
    }
    
    public static void runGarbageCollector()
    {
    	//Cocos2dxBitmap.setIsInInitialization(false);
    	System.gc();
    }
    
    public static float getDeviceVolume()
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        return (float)mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC) / max;
    }
    
    public static void setDeviceVolume(float volume)
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, Math.round(max * volume), 0);
    }
    
    public static void setDeviceNotificationVolume(float volume)
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int maxNotification = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_NOTIFICATION);
        mAudioManager.setStreamVolume(AudioManager.STREAM_NOTIFICATION, Math.round(maxNotification * volume), 0);//AudioManager.FLAG_SHOW_UI
    }
    
    public static float getVolumeStep()
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	float max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
    	return 1/max;
    }
    
    public static void setBackgroundColor(final int r, final int g, final int b)
    {
    	Log.w(TAG, "SetBackground color doesn't work properly yet on Android, background will stay black");
    	//This is a try, but it doesn't work, check why ...
    	getMainActivity().runOnUiThread(new Runnable() 
    	{
    	     @Override
    	     public void run() 
    	     {
    	    	 NativeUtility.getMainActivity().getWindow().setBackgroundDrawable(new ColorDrawable((Color.rgb(r, g, b))));
    	     }
    	});
    }
    
    public static boolean isConnected()
    {
    	ConnectivityManager connectivity = (ConnectivityManager) NativeUtility.getMainActivity().getApplicationContext().
    			getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null) 
        {
            NetworkInfo[] info = connectivity.getAllNetworkInfo();
            if (info != null) 
                for (int i = 0; i < info.length; i++) 
                    if (info[i].getState() == NetworkInfo.State.CONNECTED)
                    {
                        return true;
                    }
        }
        return false;
    }
    
    public static void vibrate(int milliseconds)
    {
    	Vibrator v = (Vibrator) getMainActivity().getSystemService(Context.VIBRATOR_SERVICE);
    	if (v.hasVibrator()) 
    	{
        	v.vibrate(milliseconds);
    	} 
    	else 
    	{
    	    final ToneGenerator tg = new ToneGenerator(AudioManager.STREAM_NOTIFICATION, 100);
    	    tg.startTone(ToneGenerator.TONE_PROP_BEEP);
    	}
    }
    public static boolean canVibrate()
    {
    	Vibrator v = (Vibrator) getMainActivity().getSystemService(Context.VIBRATOR_SERVICE);
        return v.hasVibrator();
    }
    
    public static float getDeviceLuminosity() 
    {
    	float brightnessValue = 0;
    	try
    	{
    		brightnessValue = android.provider.Settings.System.getInt (getMainActivity().getContentResolver(), android.provider.Settings.System.SCREEN_BRIGHTNESS);
		}
    	catch (SettingNotFoundException e) { }
    	
    	return brightnessValue/255;
    }

    public static void setDeviceLuminosity(float luminosity) 
    {
    	//1 is a reserved value, don't use it
    	final float lumi = luminosity >= 1 ? 0.999999f : luminosity <= 0.01 ? 0.01f : luminosity;
    	getMainActivity().runOnUiThread(new Runnable() 
    	{
    	     @Override
    	     public void run() 
    	     {
    	     	Settings.System.putInt(getMainActivity().getContentResolver(),
    	    	        Settings.System.SCREEN_BRIGHTNESS, (int)(lumi*255));
    	    	WindowManager.LayoutParams lp = getMainActivity().getWindow().getAttributes();
    	    	lp.screenBrightness = lumi; // 0f - no backlight ... 1f - full backlight
    	    	getMainActivity().getWindow().setAttributes(lp);
    	     }
    	});
    }
    
    public static boolean openSystemSettings()
    {
        boolean result = true;
        try {
            getMainActivity().startActivityForResult(new Intent(android.provider.Settings.ACTION_SETTINGS), 0);
        }
        catch (Exception e)
        {
            result = false;
            Log.e(TAG, "Exception when opening settings: " + e.getLocalizedMessage());
        }
        return result;
    }

    public static void launchYoutube()
    {
    	Intent youtubeIntent = getMainActivity().getPackageManager().getLaunchIntentForPackage("com.google.android.youtube");
    	if (youtubeIntent != null) 
    		getMainActivity().startActivity(youtubeIntent);
    }

    public static boolean isPackageInstalled(String packageName) {
        Context myContext = NativeUtility.getMainActivity().getBaseContext();
        PackageManager myPackageMgr = myContext.getPackageManager();
        try {
            myPackageMgr.getPackageInfo(packageName, PackageManager.GET_ACTIVITIES);
        }
        catch (PackageManager.NameNotFoundException e) {
            return false;
        }
        return true;
    }
}
