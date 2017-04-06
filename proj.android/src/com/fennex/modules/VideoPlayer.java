package com.fennex.modules;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

import org.videolan.libvlc.IVLCVout;
import org.videolan.libvlc.LibVLC;
import org.videolan.libvlc.Media;

import android.annotation.TargetApi;
import android.content.ContentResolver;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Bitmap.CompressFormat;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.net.Uri;
import android.opengl.Visibility;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.VideoView;

public class VideoPlayer implements IVLCVout.Callback, LibVLC.HardwareAccelerationError, Runnable
{
	/*
	 * VideoPlayer implements two ways of displaying a video :
	 * - using a native Android VideoView (default)
	 * - using LibVLC : it allows to set playback rate (not supported by VideoView) but requires a long build process and specific c files not included in the Stack
	 * 
	 * both ways use the view hierarchy as follow :
	 * mainFrame : get from NativeUtility
	 * - base : a FrameLayout added just for the video
	 *   - videoView : the SurfaceView on which the video is played. It's either a VideoView or a SurfaceView (for LibVLC)
	 */
	public static String TAG = "VideoPlayer";
	public static FrameLayout mainFrame = null;
	public static FrameLayout base = null;
	public static SurfaceView videoView = null;
	public static String path;
	public static float localX;
	public static float localY;
	public static float localHeight;
	public static float localWidth;
	public static boolean toFront;
	public static boolean isFullScreen = false;
	public static int widthScreen = NativeUtility.getMainActivity().getMainLayout().getWidth();
	public static int heightScreen = NativeUtility.getMainActivity().getMainLayout().getHeight();
	//Used by VLC implementation only to keep the video size for setSurfaceSize (avoid flickering)
	private static int currentVideoWidth;
	private static int currentVideoHeight;
	
	public static boolean useVLC = false;
	
	private static VideoPlayer instance = null;
	
	private static boolean isPrepared = false;
	private static boolean shouldLoop;
	private static boolean hideOnPause;
	private static boolean videoEnded; //Video ended is there because restart is different than play for LibVLC
	private static float lastPlaybackRate; //Playback rate must be kept between sessions (when restarting video)
	private static org.videolan.libvlc.MediaPlayer vlcMediaPlayer;
	private static org.videolan.libvlc.LibVLC libVLC;

	private static org.videolan.libvlc.MediaPlayer.EventListener mPlayerListener = new MyPlayerListener(getInstance());

	public static void setUseVLC(boolean use)
	{
	    if(videoView != null)
	    {
	        Log.e(TAG, "Can't change VideoPlayer VLC/VideoView mode after starting it");
	        return;
	    }
	    if(use && !useVLC)
	    { //If the app crash here, check that libvlc is properly compiled using compile.sh
	    	try
	    	{
	            Log.i(TAG, "LibVLC loaded");
	    	}
	    	catch(Exception e)
	    	{
	    		Log.e(TAG, "Exception while loading vlcjni, it's probably not compiled for the current architecture. Falling back to VideoView display");
                e.printStackTrace();
	    		useVLC = false;
	    	}
	    }
	    useVLC = use;
	}
	
	public static VideoPlayer getInstance()
	{
		if(instance == null)
		{
			instance = new VideoPlayer();
		}
		return instance;
	}

	//Is used by VideoPicker when doing getAllVideos
	public native static void notifyVideoDurationAvailable(String path, float duration);
	public native static void notifyVideoEnded(String path);
    public native static void notifyVideoError(String path);
	
	public static void initVideoPlayer(String file, float x, float y, float height, float width, boolean front, boolean loop)
	{
		lastPlaybackRate = 1.0f;
		path = file;
		toFront = front;
		localX = x;
		localY = y;
		localHeight = height;
		localWidth = width;
		isPrepared = false;
		shouldLoop = loop;
		hideOnPause = false;
		final File videoFile = getFile(path);
		if(videoFile == null)
		{
			return;
		}
		Log.i(TAG, "initVideoPlayer: path = " + path + ", x = " +
				x + ", y = " + y + " height = " + height + ", width = " + width + " front = " + toFront);
		
		NativeUtility.getMainActivity().runOnUiThread(new Runnable() {

			@Override
			public void run() {
				mainFrame = NativeUtility.getMainActivity().getMainLayout();
				base = new FrameLayout(NativeUtility.getMainActivity());
				mainFrame.addView(base);
				videoEnded = false;
				if(VideoPlayer.useVLC)
				{
					videoView = new SurfaceView(NativeUtility.getMainActivity());
                    SurfaceHolder mSurfaceHolder = videoView.getHolder();
					mSurfaceHolder.setFormat(PixelFormat.RGBX_8888);
					getInstance().releasePlayer();
					try {

						// Create LibVLC
						// TODO: make this more robust, and sync with audio demo
						ArrayList<String> options = new ArrayList<String>();
						options.add("--aout=opensles");
						options.add("--audio-time-stretch"); // time stretching
						libVLC = new LibVLC(options);
						libVLC.setOnHardwareAccelerationError(getInstance());
						mSurfaceHolder.setKeepScreenOn(true);

						// Create media player
						vlcMediaPlayer = new org.videolan.libvlc.MediaPlayer(libVLC);
						vlcMediaPlayer.setEventListener(mPlayerListener);
						Media m = new Media(libVLC, path);
						vlcMediaPlayer.setMedia(m);
						// Set up video output
						final IVLCVout vout = vlcMediaPlayer.getVLCVout();
						vout.setVideoView(videoView);
						vout.addCallback(getInstance());
						vout.attachViews();

					} catch (Exception e) {
						e.printStackTrace();
					}
                    isPrepared = true;
				}
				else
			    {
					VideoView video = new VideoView(NativeUtility.getMainActivity());
					video.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {

    					@Override
    					public void onPrepared(MediaPlayer mp) {
    						Log.i(TAG, "Video is prepared, playing it ...");
    						isPrepared = true;
    						play();
                    		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
                    		{
                    			public void run()
    	            			{
                    				notifyVideoDurationAvailable(path, getDuration());
    	            			}
                    		});
    					}
    				});
    				video.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {

    					@Override
    					public void onCompletion(MediaPlayer mp) {
    						if(shouldLoop)
    						{
        						Log.i(TAG, "Video ended, replaying it ...");
        						play();
    						}
    						else
    						{
                        		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
                        		{
                        			public void run()
        	            			{
                        				notifyVideoEnded(path);
        	            			}
                        		});
                        		videoEnded = true;
    						}
    					}
    				});
    				video.setOnErrorListener(new MediaPlayer.OnErrorListener() {

						@Override
						public boolean onError(MediaPlayer mp, int what,
								int extra) {
							Log.e(TAG, "VideoView error : " + what + ", " + extra);
                            NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                                public void run() {
                                    notifyVideoError(path);
                                }
                            });
							return false;
						}
    				});
    				Uri uri = null;
    				if(videoFile.exists())
    					uri = Uri.fromFile(videoFile);
    				else
    					uri = NativeUtility.getMainActivity().getUriFromFileName(path);
    				Log.i(TAG, "Using URI : " + uri.toString() + ", path : " + uri.getPath());
    				video.setVideoURI(uri);
                    FrameLayout.LayoutParams lp = VideoPlayer.isFullScreen ?
                                                    new FrameLayout.LayoutParams(VideoPlayer.widthScreen, VideoPlayer.heightScreen, Gravity.CENTER) :
                                                    new FrameLayout.LayoutParams((int) VideoPlayer.localWidth, (int) VideoPlayer.localHeight);
            		lp.leftMargin = (int)(VideoPlayer.isFullScreen ? 0 : localX - (localWidth / 2) + 0.5);
            		lp.topMargin = (int)(VideoPlayer.isFullScreen ? 0 : heightScreen - localY - (localHeight / 2) + 0.5);
					lp.gravity = isFullScreen ? Gravity.CENTER : (Gravity.TOP | Gravity.START);
            		video.setLayoutParams(lp);
                    Log.i(TAG, "widthScreen : " + widthScreen);
                    Log.i(TAG, "heightScreen : " + heightScreen);

                    Log.i(TAG, "localX : " + localX);
                    Log.i(TAG, "localY : " + localY);

                    Log.i(TAG, "localWidth : " + localWidth);
                    Log.i(TAG, "localHeight : " + localHeight);

    				Log.i(TAG, "Placing video in x: " + lp.leftMargin);
    				Log.i(TAG, "Placing video in y: " + lp.topMargin);
    				videoView = video;
			    }
				FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams((int)widthScreen, (int)heightScreen);
				base.setLayoutParams(layoutParams);
			    
				videoView.setZOrderMediaOverlay(VideoPlayer.toFront);
				base.addView(videoView);

				if(toFront)
					base.bringToFront();
				base.setClickable(false);
				videoView.setClickable(false);
        		videoView.invalidate();
        		base.invalidate();
        		mainFrame.invalidate();
			}
		});
	}

	public static void setPlayerPosition(float x, float y, float height, float width)
	{
		localX = x;
		localY = y;
		localHeight = height;
		localWidth = width;
		NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if(videoView != null)
				{
					FrameLayout.LayoutParams lp = VideoPlayer.isFullScreen ?
							new FrameLayout.LayoutParams(VideoPlayer.widthScreen, VideoPlayer.heightScreen, Gravity.CENTER) :
							new FrameLayout.LayoutParams((int) VideoPlayer.localWidth, (int) VideoPlayer.localHeight);
					lp.leftMargin = (int)(VideoPlayer.isFullScreen ? 0 : localX - (localWidth / 2) + 0.5);
					lp.topMargin = (int)(VideoPlayer.isFullScreen ? 0 : heightScreen - localY - (localHeight / 2) + 0.5);
					lp.gravity = isFullScreen ? Gravity.CENTER : (Gravity.TOP | Gravity.START);
					videoView.setLayoutParams(lp);
					videoView.invalidate();
				}
			}});
	}

	public static void play()
	{
		Log.i(TAG, "Play.");
		if(useVLC)
		{
			if(vlcMediaPlayer != null) {
				if(videoEnded)
				{
					Log.i(TAG, "videoEnded, restarting");
					vlcMediaPlayer.stop();
				}
				vlcMediaPlayer.play();
				videoEnded = false;
			}
			else
			{
				Log.e(TAG, "stop vlcMediaPlayer is Null");
			}
		}
		else if(isPrepared)
		{
			if(videoEnded)
			{
				((VideoView)videoView).seekTo(0);
			}
			((VideoView)videoView).start();
		}
		if(hideOnPause)
		{
			NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
				@Override
				public void run() {
					videoView.setVisibility(View.VISIBLE);
					videoView.invalidate();
				}});
		}
		videoEnded = false;
	}


	public static void pause()
	{
		Log.i(TAG, "Pause.");
		if(useVLC)
		{
			if(vlcMediaPlayer != null && vlcMediaPlayer.isPlaying())
			{
				vlcMediaPlayer.pause();
			}
			else
			{
				Log.e(TAG, "pause vlcMediaPlayer is Null or not playing");
			}
		}
		else if(isPrepared)
		{
			((VideoView)videoView).pause();
		}
		if(hideOnPause)
		{
			NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
				@Override
				public void run() {
					videoView.setVisibility(View.GONE);
					videoView.invalidate();
				}});
		}
	}

	public static void stop()
	{
		Log.i(TAG, "Stop.");
		NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (base != null && videoView != null) {
					if (useVLC) {
						if(vlcMediaPlayer != null) {
							vlcMediaPlayer.pause();
						}
						else
						{
							Log.e(TAG, "stop vlcMediaPlayer is Null");
						}
					} else {
						((VideoView) videoView).stopPlayback();
					}
					base.removeAllViews();
					mainFrame.removeView(base);
				}
			}
		});
	}

	public static float getPlaybackRate()
	{
		if(useVLC)
		{
			if(videoEnded)
			{
				return lastPlaybackRate;
			}
			if(vlcMediaPlayer != null) {
				return vlcMediaPlayer.getRate();
			}
			else
			{
				Log.e(TAG, "getPlaybackRate vlcMediaPlayer is Null");
			}
			return lastPlaybackRate;
		}
		Log.e(TAG, "getPlaybackRate is only implemented for LibVLC");
		return 1;
	}

	public static void setPlaybackRate(float rate)
	{
		if(useVLC)
		{
			if(vlcMediaPlayer != null) {
				vlcMediaPlayer.setRate(rate);
			}
			else
			{
				Log.e(TAG, "setPlaybackRate vlcMediaPlayer is Null");
			}
			lastPlaybackRate = rate;
			return;
		}
		Log.e(TAG, "setPlaybackRate is only implemented for LibVLC");
	}

	public static void setHideOnPause(boolean hide)
	{
		hideOnPause = hide;
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	public static void setFullscreen(boolean fullscreen, boolean animated)
	{
		//TODO : implement animated
		isFullScreen = fullscreen;
		if(useVLC)
		{
			//This force LibVLC to recalculate the surface size to a correct value. Put 1,1 to not have a visual glitch
			getInstance().setSurfaceSize((int)currentVideoWidth,(int)currentVideoHeight,(int)currentVideoWidth,(int)currentVideoHeight,1,1);
		}
		else
		{
	        NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
	        	@Override
	        	public void run() {
					if(isFullScreen)
					{
						base.setBackgroundColor(Color.BLACK);
						base.setAlpha((float) 0.6);
					}
					else
					{
						base.setAlpha((float) 0);
					}
		    		FrameLayout.LayoutParams lp = (android.widget.FrameLayout.LayoutParams) videoView.getLayoutParams();
		    		lp.width  = isFullScreen ? (int) widthScreen : (int) localWidth;
		    		lp.height = isFullScreen ? (int) heightScreen : (int) localHeight;
					lp.leftMargin = (int)(isFullScreen ? 0 : localX - (localWidth / 2) + 0.5);
					lp.topMargin = (int)(isFullScreen ? 0 : heightScreen - localY - (localHeight / 2) + 0.5);
					lp.gravity = isFullScreen ? Gravity.CENTER : (Gravity.TOP | Gravity.START);
		    		videoView.setLayoutParams(lp);
	        		videoView.invalidate();
	        	}
	        });
		}
	}

	public static boolean isFullscreen()
	{
		return isFullScreen;
	}
	
	public static float getDuration()
	{
		float duration = 0;
		if(useVLC)
		{
			if(vlcMediaPlayer != null) {
				duration = vlcMediaPlayer.getLength();
			}
			else
			{
				Log.e(TAG, "getDuration vlcMediaPlayer is Null");
			}
		}
		else if (isPrepared)
		{
			try {
				duration = ((VideoView)videoView).getDuration();
			} catch (Exception e) {
				//If the video view isn't properly prepared, handle it
				e.printStackTrace();
				duration = 0;
			}
		}
		//convert in seconds or to 0 for empty
		duration = duration == -1 ? 0 : duration / 1000;
		return duration;
	}
	
	public static float getPosition()
	{
		float position = 0;
		if(useVLC)
		{
			if(vlcMediaPlayer != null) {
				position = vlcMediaPlayer.getTime();
			}
			else
			{
				Log.e(TAG, "getPosition getPosition is Null");
			}
		}
		else
		{
			position = ((VideoView)videoView).getCurrentPosition();
		}
		//convert in seconds or to 0 for empty
		position = position == -1 ? 0 : position / 1000;
		return position;
	}
	
	public static void setPosition(float position)
	{
		if(useVLC)
		{
			if(vlcMediaPlayer != null) {
				position = vlcMediaPlayer.setTime((long)(position * 1000));
			}
			else
			{
				Log.e(TAG, "setPosition getPosition is Null");
			}
		}
		else
		{
			((VideoView)videoView).seekTo((int)(position * 1000));
		}
	}
	
	public static String getThumbnail(String path)
	{
        File videoFile = getFile(path);
		if(videoFile == null)
		{
			return null;
		}
		//Get the video file name, without extension
		String fileName = videoFile.getName().lastIndexOf('.') > -1 ? videoFile.getName().substring(0, videoFile.getName().lastIndexOf('.')) : videoFile.getName();
		if(fileName.lastIndexOf('/') > 0)
		{
			fileName = fileName.substring(fileName.lastIndexOf('/') + 1);
		}
		//Add -thumbnail. That's the path used by cocos2dx
		String thumbPath = fileName + "-thumbnail";
		//The full path used to save it, with local path and extension
		String fullThumbPath = NativeUtility.getLocalPath().concat("/" + thumbPath + ".png");
		File thumbImageFile = new File(fullThumbPath);
		//Don't redo it if it already exists
		if(thumbImageFile.exists())
		{
			Log.d(TAG, "Video thumbnail already created at path: " + fullThumbPath);
			return thumbPath;
		}
		try {
			Log.d(TAG, "saving video thumbnail at path: " + fullThumbPath + ", video path: " + videoFile.getAbsolutePath());
			//Save the thumbnail in a PNG compressed format, and close everything. If something fails, return null
			FileOutputStream streamThumbnail = new FileOutputStream(fullThumbPath);

			//Other method to get a thumbnail. The problem is that it doesn't allow to get at a specific time 
			Bitmap thumb; //= ThumbnailUtils.createVideoThumbnail(videoFile.getAbsolutePath(),MediaStore.Images.Thumbnails.MINI_KIND);
			MediaMetadataRetriever retriever = new MediaMetadataRetriever();
			try {
				Uri appUri = NativeUtility.getMainActivity().getUriFromFileName(path);
				if(appUri != null && appUri.toString().startsWith("android.resource://")) {
					//Raw resources cannot be loaded with absolute path
					retriever.setDataSource(NativeUtility.getMainActivity(), appUri);
				}
				else {
					retriever.setDataSource(videoFile.getAbsolutePath());
				}
		        int timeInSeconds = 1;
		        thumb = retriever.getFrameAtTime(timeInSeconds * 1000000,
		                    MediaMetadataRetriever.OPTION_CLOSEST_SYNC);
                if(thumb != null) {
                    thumb.compress(CompressFormat.PNG, 80, streamThumbnail);
                    thumb.recycle(); //ensure the image is freed;
                }
                else {
                    thumbPath = null;
                }
		    } catch (Exception ex) {
		        Log.i(TAG, "MediaMetadataRetriever in getThumbnail got exception:" + ex);
		        thumbPath = null;
		    }
			retriever.release();
			streamThumbnail.close();
			Log.d(TAG, "thumbnail saved successfully");
		} catch (FileNotFoundException e) {
			Log.d(TAG, "File Not Found Exception : check directory path");
			e.printStackTrace();
			thumbPath = null;
		} catch (IOException e) {
			Log.d(TAG, "IOException while closing the stream");
			e.printStackTrace();
			thumbPath = null;
		}
		return thumbPath;
	}

	public static float[] getVideoSize(String path)
	{
		MediaMetadataRetriever retriever = new MediaMetadataRetriever();
		File videoFile = getFile(path);
		float[] size = new float[2];
		size[0] = 0;
		size[1] = 0;
		try {
			Uri appUri = NativeUtility.getMainActivity().getUriFromFileName(path);
			if(appUri != null && appUri.toString().startsWith("android.resource://")) {
				//Raw resources cannot be loaded with absolute path
				retriever.setDataSource(NativeUtility.getMainActivity(), appUri);
			}
			else {
				retriever.setDataSource(videoFile.getAbsolutePath());
			}
			size[0] = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH));
			size[1] = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT));
		} catch (Exception ex) {
			Log.i(TAG, "MediaMetadataRetriever in getVideoSize got exception:" + ex);
		}
		retriever.release();
		return size;
	}

	public static boolean isValidVideo(String path)
	{
		int dotIndex = path.lastIndexOf(".");
		if (dotIndex != -1) {
			String fileExt = path.substring(dotIndex);
			return videoExists(path) && org.videolan.libvlc.util.Extensions.VIDEO.contains(fileExt);
		}
		return false;
	}
	
	public static boolean videoExists(String path)
	{
		File videoFile = getFile(path);
		return videoFile != null && videoFile.exists() && videoFile.canRead();
	}
	
	public static File getFile(String path)
	{
		Uri localURI = NativeUtility.getMainActivity().getUriFromFileName(path);
		File localFile = null;
		final String[] storageDict = VideoPicker.getStorageDirectories();
		//First check the most common path, then check all storage directories
		boolean startWithStorageDict = path.startsWith(Environment.getExternalStorageDirectory().toString());
		for(int i = 0; i < storageDict.length && !startWithStorageDict; i++)
		{
			if(path.startsWith(storageDict[i]))
			{
				startWithStorageDict = true;
			}
		}
		if(localURI != null) 
		{
			localFile = new File(localURI.getPath());
		}
		else if(path.startsWith(NativeUtility.getLocalPath()) 
				|| startWithStorageDict)
		{
			localFile = new File(path);
		}
		else
		{
			ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
			String[] projection = {MediaStore.MediaColumns.DATA};
			Cursor cur = cr.query(Uri.parse(path), projection, null, null, null);
			if(cur != null && cur.moveToFirst())
			{
			   String filePath = cur.getString(0);
			   cur.close();
			   localFile = new File(filePath);
			   if(localFile.exists()){
				   localFile.setReadable(true, false);
				   Log.i(TAG, "File found, path : " + filePath);
				   if(!localFile.canRead())
				   {
					   Log.e(TAG, "Error, cannot read file");
					   return null;
				   }
			   } else {
				   Log.e(TAG, "File not found for path : " + filePath);
				   return null;
			   }
			} else {
				   Log.e(TAG, "Invalid URI or other problem with path : " + path);
				   return null;
			}
		}
		return localFile;
	}

	public void setSurfaceSize(final int width, final int height, final int visible_width, final int visible_height, int sar_num, int sar_den) {
		if (width * height == 0)
			return;
		currentVideoWidth = width;
		currentVideoHeight = height;
		// force surface buffer size
		NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				SurfaceHolder mSurfaceHolder = videoView.getHolder();
				mSurfaceHolder.setFixedSize(width, height);

				// set display size
				//VideoPlayer.localWidth/Height is the frame maximum size, width/height is the video size
				//visible_width/height should be ignored, it's how VLC think it should fit
				float videoWidth = (VideoPlayer.isFullScreen ? VideoPlayer.widthScreen : VideoPlayer.localWidth);
				float videoHeight = (VideoPlayer.isFullScreen ? VideoPlayer.heightScreen : VideoPlayer.localHeight);
				float ratioWidth = videoWidth / (float)width;
				float ratioHeight = videoHeight / (float)height;
				if(ratioWidth > ratioHeight)
				{
					videoWidth = videoWidth / ratioWidth * ratioHeight;
				}
				else
				{
					videoHeight = videoHeight / ratioHeight * ratioWidth;
				}

				FrameLayout.LayoutParams lp = (android.widget.FrameLayout.LayoutParams) videoView.getLayoutParams();
				lp.width  = (int)videoWidth;
				lp.height = (int)videoHeight;
				lp.leftMargin = (int)(VideoPlayer.isFullScreen ? 0 : widthScreen - localX - (videoWidth / 2));
				lp.topMargin = (int)(VideoPlayer.isFullScreen ? 0 : heightScreen - localY - (videoHeight / 2));
				lp.gravity = isFullScreen ? Gravity.CENTER : (Gravity.TOP | Gravity.START);
				videoView.setLayoutParams(lp);
				videoView.invalidate();
			}
		});
	}

	@Override
	public void onNewLayout(IVLCVout vlcVout,final int width,final int height, int visibleWidth, int visibleHeight, int sarNum, int sarDen) {
		if (width * height == 0)
			return;

		currentVideoWidth = width;
		currentVideoHeight = height;
		if(vlcVout != vlcMediaPlayer.getVLCVout())
		{
			try
			{
				vlcVout.setVideoView(videoView);
				vlcVout.attachViews();
			}
			catch(IllegalStateException e)
			{
				e.printStackTrace();
			}
		}
		setSurfaceSize(width, height, visibleWidth, visibleHeight, sarNum, sarDen);
	}

	@Override
	public void onSurfacesCreated(IVLCVout vlcVout) {
	}

	@Override
	public void onSurfacesDestroyed(IVLCVout vlcVout) {
		this.releasePlayer();
	}

	@Override
	public void run() {
		this.play();
	}

	private static class MyPlayerListener implements org.videolan.libvlc.MediaPlayer.EventListener {
		private WeakReference<VideoPlayer> mOwner;

		public MyPlayerListener(VideoPlayer owner) {
			mOwner = new WeakReference<VideoPlayer>(owner);
		}

		@Override
		public void onEvent(org.videolan.libvlc.MediaPlayer.Event event) {
			VideoPlayer player = mOwner.get();

			switch(event.type) {
				case org.videolan.libvlc.MediaPlayer.Event.EndReached:
					Log.d(TAG, "MediaPlayerEndReached");
					videoEnded = true;
					NativeUtility.getMainActivity().runOnGLThread(new Runnable()
					{
						public void run()
						{
							notifyVideoEnded(path);
							if(shouldLoop)
							{
								play();
							}
						}
					});
					break;
				case org.videolan.libvlc.MediaPlayer.Event.Playing:
				case org.videolan.libvlc.MediaPlayer.Event.Paused:
				case org.videolan.libvlc.MediaPlayer.Event.Stopped:
				default:
					break;
			}
		}
	}

	private void releasePlayer() {
		if (libVLC == null)
			return;
		vlcMediaPlayer.stop();
		final IVLCVout vout = vlcMediaPlayer.getVLCVout();
		vout.removeCallback(this);
		vout.detachViews();
		libVLC.release();
		libVLC = null;
	}

	@Override
	public void eventHardwareAccelerationError() {
		// Handle errors with hardware acceleration
		Log.e(TAG, "Error with hardware acceleration");
		this.releasePlayer();
	}
}
