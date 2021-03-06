/*
***************************************************************************
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

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.AudioManager;
import android.media.ToneGenerator;
import android.os.Build;
import android.os.Environment;
import android.os.LocaleList;
import android.os.StatFs;
import android.os.Vibrator;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.text.format.DateUtils;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.Locale;

@TargetApi(Build.VERSION_CODES.HONEYCOMB)
public class NativeUtility 
{
    private static final String TAG = "NativeUtility";
    
    public native static void notifyMemoryWarning();
    public native static void notifyVolumeChanged();

    //Query a String to be displayed to user with a key
    //It is C++ app responsibility to return a translated String for this key
    public native static String getNativeString(String key);
    
	private static volatile ActivityResultNotifier mainActivity;
    @SuppressWarnings("WeakerAccess")
	public static void setMainActivity(ActivityResultNotifier activity)
	{
		mainActivity = activity;
	}
	
	public static ActivityResultNotifier getMainActivity()
	{
		return mainActivity;
	}

    @SuppressWarnings("unused")
    public static String getOpenUrl()
    {
        String openUrl = getMainActivity().getIntent().getDataString();
        if(openUrl == null)
        {
            openUrl = "";
        }
        Log.d(TAG, "returning openUrl: " + openUrl);
        return openUrl;
    }
    public static native void notifyUrlOpened(String url);

    @SuppressWarnings("WeakerAccess")
    public static String getAppName()
    {
        ApplicationInfo applicationInfo = getMainActivity().getApplicationInfo();
        int stringId = applicationInfo.labelRes;
        return stringId == 0 ? applicationInfo.nonLocalizedLabel.toString() : getMainActivity().getString(stringId);
    }

    public static String getPackageIdentifier()
    {
        return getMainActivity().getClass().getPackage().getName();
    }

    @SuppressWarnings("unused")
    public static String getUniqueIdentifier()
    {
        return Settings.Secure.getString(getMainActivity().getApplicationContext().getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    @SuppressWarnings("unused")
    public static String getAppVersionNumber() throws PackageManager.NameNotFoundException {
        return getMainActivity().getPackageManager().getPackageInfo(getMainActivity().getPackageName(), 0).versionName;
    }

    @SuppressWarnings("unused")
    public static int getAppVersionCode() throws PackageManager.NameNotFoundException {
        return getMainActivity().getPackageManager().getPackageInfo(getMainActivity().getPackageName(), 0).versionCode;
    }

    public static String getUniqueIdentifierByContext(Context context)
    {
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    @SuppressWarnings("unused")
    public static String getDeviceModelIdentifier()
    {
        return Build.MODEL;
    }

    @SuppressWarnings("unused")
    public static String getDeviceModelName()
    {
        return Build.DISPLAY;
    }

    @SuppressWarnings("unused")
    public static String getDeviceVersion()
    {
        return Build.VERSION.RELEASE;
    }

    @SuppressWarnings("unused")
    public static int getDeviceSDK()
    {
        return Build.VERSION.SDK_INT;
    }

    @SuppressWarnings("unused")
    public static long getTotalStorageSpace()
    {
        StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getAbsolutePath());
        return stat.getTotalBytes();
    }

    @SuppressWarnings("unused")
    public static long getAvailableStorageSpace()
    {
        StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getAbsolutePath());
        return stat.getAvailableBytes();
    }

    @SuppressWarnings("unused")
    public static String getMovieFolderName() {
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES).getName();
    }

    @SuppressWarnings("unused")
    public static boolean isPhone()
    {
    	boolean result = ((getMainActivity().getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) < Configuration.SCREENLAYOUT_SIZE_LARGE);
    	Log.i(TAG, "is phone ? " + (result ? "yes" : "no"));
    	return result;
    }

    @SuppressWarnings("WeakerAccess")
    public static void copyResourceFileToLocal(String path)
    {
    	Log.d(TAG, "copying resource file to local : " + path);
        File destinationFile = new File(FileUtility.getLocalPath() + java.io.File.separator + path);    
    	if(!destinationFile.exists())
    	{
            if(path.contains("/"))
            {
                File parentFolder = new File(FileUtility.getLocalPath() + java.io.File.separator + path.substring(0, path.lastIndexOf("/")));
                parentFolder.mkdirs();
            }
         	Log.i(TAG, "File doesn't exist, doing the copy to " + destinationFile.getAbsolutePath());
            AssetManager am = NativeUtility.getMainActivity().getAssets();
            try { 
                InputStream in = am.open(path);
                FileOutputStream f = new FileOutputStream(destinationFile); 
                byte[] buffer = new byte[1024];
                int len1;
                while ((len1 = in.read(buffer)) > 0) {
                    f.write(buffer, 0, len1);
                }
                f.close();
             	Log.i(TAG, "Copied " + path + " to " + FileUtility.getLocalPath() + java.io.File.separator + path);
            } catch (IOException e) {
                e.printStackTrace();
            }
    	}
    	else
        {
         	Log.i(TAG, "didn't copied" + path + ", already exist");            	
        }
    }

    @SuppressWarnings("unused")
    public static String getCurrentLanguage() {
        //Do NOT use cocos2d-x version, because it doesn't work on Android >= N
        //Taken from https://stackoverflow.com/questions/14389349/android-get-current-locale-not-default
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            return LocaleList.getDefault().get(0).getLanguage();
        } else{
            return Locale.getDefault().getLanguage();
        }
    }

    @SuppressWarnings("unused")
    public static String getCurrentCountry() {
        //Do NOT use cocos2d-x version, because it doesn't work on Android >= N
        //Taken from https://stackoverflow.com/questions/14389349/android-get-current-locale-not-default
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            return LocaleList.getDefault().get(0).getCountry();
        } else{
            return Locale.getDefault().getCountry();
        }
    }

    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
    public static boolean doesPreventIdleTimerSleep()
    {
        int flags = getMainActivity().getWindow().getAttributes().flags;
        return (flags & WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON) != 0;
    }
    
    @TargetApi(19)
    @SuppressWarnings("unused")
    public static void startSceneInitialisation()
    {
    	boolean enableCaching = true;
		ActivityManager am = (ActivityManager) getMainActivity().getSystemService(Context.ACTIVITY_SERVICE);
		//Disable Bitmap caching for low ram devices, as it will receive too much low memory notifications
    	if(android.os.Build.VERSION.SDK_INT >= 19)
    	{
    		enableCaching = ! am.isLowRamDevice();
    	}
    	//Cocos2dxBitmap.setIsInInitialization(enableCaching);
    }

    @SuppressWarnings("unused")
    public static void runGarbageCollector()
    {
    	//Cocos2dxBitmap.setIsInInitialization(false);
    	System.gc();
    }

    @SuppressWarnings("unused")
    public static String formatDate(long date)
    {
        Date toConvert = new Date(date * 1000);
        return DateFormat.getDateInstance(DateFormat.SHORT).format(toConvert);
    }

    private static int getDateFormat(int nativeFormat)
    {
        switch (nativeFormat)
        {
            case 1: // SHORT
                return DateFormat.SHORT;
            case 2: // MEDIUM
                return DateFormat.MEDIUM;
            case 3: // LONG
                return DateFormat.LONG;
            case 4: // FULL
                return DateFormat.FULL;
        }
        return -1;
    }

    @SuppressWarnings("unused")
    public static String formatDateTime(long dateTime, int dayFormat, int hourFormat)
    {
        Date toConvert = new Date(dateTime * 1000);
        int dayFormatInfo = getDateFormat(dayFormat);
        int hourFormatInfo = getDateFormat(hourFormat);
        if(dayFormatInfo != -1 && hourFormatInfo != -1)
        {
            return DateFormat.getDateTimeInstance(dayFormatInfo, hourFormatInfo).format(toConvert);
        }
        else if(dayFormatInfo != -1)
        {
            return DateFormat.getDateInstance(dayFormatInfo).format(toConvert);
        }
        else if(hourFormatInfo != -1)
        {
            return DateFormat.getTimeInstance(hourFormatInfo).format(toConvert);
        }
        return "";
    }

    @SuppressWarnings("unused")
    public static String formatDateTime(long date, String formatTemplate)
    {
        String pattern = android.text.format.DateFormat.getBestDateTimePattern(Locale.getDefault(), formatTemplate);
        Date toConvert = new Date(date * 1000);
        String result = new SimpleDateFormat(pattern, Locale.getDefault()).format(toConvert);
        return new SimpleDateFormat(pattern, Locale.getDefault()).format(toConvert);
    }

    @SuppressWarnings("unused")
    public static long parseDate(String date, int dayFormat)
    {
        int dayFormatInfo = getDateFormat(dayFormat);
        try
        {
            return DateFormat.getDateInstance(dayFormatInfo).parse(date).getTime() / 1000;
        }
        catch (ParseException e)
        {
            Log.i(TAG, "parse time failed with exception : " + e.getMessage());
        }
        return 0;
    }

    @SuppressWarnings("unused")
    public static String formatDurationShort(int seconds)
    {
        return DateUtils.formatElapsedTime(seconds);
    }

    @SuppressWarnings("unused")
    public static float getDeviceVolume()
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        return (float)mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC) / max;
    }

    @SuppressWarnings("unused")
    public static void setDeviceVolume(float volume)
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, Math.round(max * volume), 0);
    }

    @SuppressWarnings("unused")
    public static void setDeviceNotificationVolume(float volume)
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	int maxNotification = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_NOTIFICATION);
        mAudioManager.setStreamVolume(AudioManager.STREAM_NOTIFICATION, Math.round(maxNotification * volume), 0);//AudioManager.FLAG_SHOW_UI
    }

    @SuppressWarnings("unused")
    public static float getVolumeStep()
    {
    	AudioManager mAudioManager = (AudioManager) getMainActivity().getSystemService(Context.AUDIO_SERVICE);
    	float max = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
    	return 1/max;
    }

    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
    public static boolean canVibrate()
    {
    	Vibrator v = (Vibrator) getMainActivity().getSystemService(Context.VIBRATOR_SERVICE);
        return v.hasVibrator();
    }

    @SuppressWarnings("unused")
    public static float getDeviceLuminosity() 
    {
    	float brightnessValue = 0;
    	try
    	{
    		brightnessValue = android.provider.Settings.System.getInt (getMainActivity().getContentResolver(), android.provider.Settings.System.SCREEN_BRIGHTNESS);
		}
    	catch (SettingNotFoundException ignored) { }
    	
    	return brightnessValue/255;
    }

    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
    public static boolean isPackageInstalled(String packageName) {
        Context myContext = NativeUtility.getMainActivity().getBaseContext();
        return isPackageInstalled(myContext, packageName);
    }

    @SuppressWarnings("unused, WeakerAccess")
    public static boolean isPackageInstalled(Context myContext, String packageName) {
        PackageManager myPackageMgr = myContext.getPackageManager();
        try {
            myPackageMgr.getPackageInfo(packageName, PackageManager.GET_ACTIVITIES);
        }
        catch (PackageManager.NameNotFoundException e) {
            return false;
        }
        return true;
    }

    @SuppressWarnings("unused")
    public static int getApplicationVersion(String packageName)
    {
        Context myContext = NativeUtility.getMainActivity().getBaseContext();
        PackageManager myPackageMgr = myContext.getPackageManager();
        int version = -1;
        try
        {
            PackageInfo pInfo = myPackageMgr.getPackageInfo(packageName, 0);
            version = pInfo.versionCode;
        }
        catch (PackageManager.NameNotFoundException ignored) {

        }

        return version;
    }

    public static void showToast(String textKey, int toastLength) {
        Toast.makeText(getMainActivity(), getNativeString(textKey), toastLength).show();
    }
}
