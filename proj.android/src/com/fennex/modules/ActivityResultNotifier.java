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

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.FrameLayout;

/* Implements ActivityResult so that the MainActivity doesn't have to redo all this common code
 * Responders are used to respond to ActivityResult (when you get the response of an Intent)
 * Observers can monitor the Activity lifecycle (onCreate, onResume, etc.... for the detail, see Activity documentation)
 */
public abstract class ActivityResultNotifier extends Cocos2dxActivity implements MainActivityUtility
{
    protected static SplashDialog splashDialog;
	private ArrayList<ActivityResultResponder> responders;
	private ArrayList<ActivityObserver> observers;
	private boolean active = false;
	public boolean isActive() { return active; }

    //The minimum duration is the fade_in duration, default to 500, try to load from config, plus a flat time after anim
    final private long SPLASH_DURATION_AFTER_ANIM = 500;
    private long splashMinDuration = 500 + SPLASH_DURATION_AFTER_ANIM;
    private static long launchTime = 0;
	
	public ActivityResultNotifier()
	{
		responders = new ArrayList<ActivityResultResponder>();
		observers = new ArrayList<ActivityObserver>();
	}
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		if(launchTime != 0 && this.getIntent().getDataString() != null)
		{
			NativeUtility.notifyUrlOpened(this.getIntent().getDataString());
		}
        if(getSplashScreenLayout() != -1 && getSplashScreenImageID() != -1 && launchTime == 0) {
            splashDialog = new SplashDialog(this, getSplashScreenLayout(), getSplashScreenImageID());
            splashDialog.show();
            launchTime = SystemClock.elapsedRealtime();
        }
		super.onCreate(savedInstanceState);
        splashMinDuration = getResources().getInteger(android.R.integer.config_longAnimTime) + SPLASH_DURATION_AFTER_ANIM;
    	NativeUtility.setMainActivity(this);
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.CREATE);
		}
	}

    public void discardSplashDialog()
    {
        if(splashDialog != null) {
            final long currentTime = SystemClock.elapsedRealtime();
            //Ensure the splashScreen is longer than the animation
            if(currentTime - launchTime > splashMinDuration) {
				new Timer().schedule(new TimerTask() {
					@Override
					public void run() {
						if(splashDialog!= null)
						{
							splashDialog.cancel();
							splashDialog = null;
						}
					}
				}, 100);
            }
            else {
                Log.i("FenneX", "Duration of runnable : " + (splashMinDuration - (currentTime - launchTime)));
                new Timer().schedule(new TimerTask() {
                    @Override
                    public void run() {
						if(splashDialog!= null)
						{
							splashDialog.discard();
							splashDialog = null;
						}
                    }
                }, splashMinDuration - (currentTime - launchTime) + 100);
            }
        }
    }
	
	@Override
	public void addResponder(ActivityResultResponder responder) {
		if(!responders.contains(responder))
		{
			Log.i("ActivityResultNotifier", "Adding responder : " + responder);
			responders.add(responder);
		}
	}
	
	@Override
	public void removeResponder(ActivityResultResponder responder) {
		responders.remove(responder);		
	}
	
	public void addObserver(ActivityObserver observer) {
		if(!observers.contains(observer))
		{
			Log.i("ActivityResultNotifier", "Adding observer : " + observer);
			observers.add(observer);
		}
	}
	
	public void removeObserver(ActivityObserver observer) {
		observers.remove(observer);		
	}
    
    //TODO : add a system to add onActivityResult listeners
	@Override
	protected void onActivityResult(int requestCode, int resultcode, Intent intent)
	{
		Log.i("ActivityResultNotifier", "Activity result received ...");
		super.onActivityResult(requestCode, resultcode, intent);
		if (resultcode == RESULT_OK)
		{
			Log.i("ActivityResultNotifier", "Valid intent, notifying children ...");
			for(ActivityResultResponder responder : responders)
			{
				Log.i("ActivityResultNotifier", "Notifying child : " + responder);
				responder.onActivityResult(requestCode, resultcode, intent);
			}
			Log.i("ActivityResultNotifier", "Done notifying");
		}
		else
		{
			Log.i("ActivityResultNotifier", "Result not OK");
		}
	}
	
	protected void onNewIntent(Intent intent)
	{
		LocalNotification.onNewIntent(intent);
		super.onNewIntent(intent);
	}
	
	@Override
	public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
		// This is done to handle problem from having a surfaceview (like in VideoPLayer) overriding onKeyDown. ( So we override it again)
		return org.cocos2dx.lib.Cocos2dxGLSurfaceView.getInstance().onKeyDown(pKeyCode, pKeyEvent);
	}

	@Override
	public void onLowMemory()
	{
        /* WARNING : NOT IMPLEMENTED ON V3
		if(Cocos2dxBitmap.isInInitialization() && Cocos2dxBitmap.forceClearBitmaps())
		{
			Log.i("MainActivity", "Solve memory warning by releasing labels bitmaps");
			return;
		}*/

		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
		{
			public void run()
			{
				NativeUtility.notifyMemoryWarning();
			}
		});
	}
	
	@Override
	public boolean onKeyUp (int keyCode, KeyEvent event) 
	{
		if(keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
		{
    		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
    		{
    			public void run()
    			{
    				NativeUtility.notifyVolumeChanged();
    			}
    		});
		}
		// This is done to handle problem from having a surfaceview (like in VideoPLayer) overriding onKeyDown. ( So we override it again)
		return org.cocos2dx.lib.Cocos2dxGLSurfaceView.getInstance().onKeyUp(keyCode, event);
	}
	
	
	//
	// Methods to notify Observers of lifecycle changes
	//
	@Override
	public void onStart()
	{
		super.onStart();
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.START);
		}
	}
	
	@Override
	public void onResume()
	{
		super.onResume();
		active = true;
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.RESUME);
		}
	}
	
	@Override
	public void onPause()
	{
		super.onPause();
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.PAUSE);
		}
	}
	
	@Override
	public void onStop()
	{
        if(splashDialog != null) {
            splashDialog.cancel();
            splashDialog = null;
        }
		super.onStop();
		active = false;
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.STOP);
		}
	}
	
	@Override
	public void onRestart()
	{
		super.onRestart();
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.RESTART);
		}
	}
	
	@Override
	public void onDestroy()
	{
        if(splashDialog != null) {
            splashDialog.cancel();
            splashDialog = null;
        }
        for(ActivityResultResponder responder : responders)
        {
            responder.destroy();
        }
		super.onDestroy();
		for(ActivityObserver observer : observers)
		{
			observer.onStateChanged(ActivityObserver.DESTROY);
            observer.destroy();
		}
	}

    public FrameLayout getMainLayout()
    {
        return mFrameLayout;
    }
}
