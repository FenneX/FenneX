package com.fennex.modules;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

import org.videolan.libvlc.EventHandler;
import org.videolan.libvlc.IVideoPlayer;
import org.videolan.libvlc.LibVLC;
import org.videolan.libvlc.LibVlcException;
import org.videolan.libvlc.LibVlcUtil;
import org.videolan.libvlc.Media;
import org.videolan.libvlc.MediaList;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.annotation.TargetApi;
import android.content.ContentResolver;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.Bitmap.CompressFormat;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.VideoView;

public class VideoPlayer extends Handler implements IVideoPlayer
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
	private static boolean videoEnded; //Video ended is there because restart is different than play for LibVLC
	
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
	    		System.loadLibrary("vlcjni");
	            Log.i(TAG, "LibVLC loaded");
	            
                LibVlcUtil.getLibVlcInstance().init(NativeUtility.getMainActivity());
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
	
	static void initVideoPlayer(String file, float x, float y, float height, float width, boolean front, boolean loop)
	{
		path = file;
		toFront = front;
		localX = x;
		localY = y;
		localHeight = height;
		localWidth = width;
		isPrepared = false;
		shouldLoop = loop;
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
                    LibVLC vlc = null;
                    try {
                        vlc = LibVlcUtil.getLibVlcInstance();
                    } catch (LibVlcException e) {
                        e.printStackTrace();
                    }
                    vlc.attachSurface(mSurfaceHolder.getSurface(), VideoPlayer.getInstance());
                    mSurfaceHolder.addCallback(VideoPlayer.getInstance().mSurfaceCallback);

                    MediaList list = vlc.getPrimaryMediaList();
                    list.clear();
                    EventHandler.getInstance().addHandler(getInstance());
    	            list.getEventHandler().addHandler(getInstance());
                    Media m = new Media(vlc, LibVLC.PathToURI(videoFile.getPath()));
                    list.add(m);
                    vlc.setMediaList(list);
                    vlc.playIndex(0);
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
            		FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
            				VideoPlayer.isFullScreen ? (int) VideoPlayer.widthScreen : (int) VideoPlayer.localWidth, 
            				VideoPlayer.isFullScreen ? (int) VideoPlayer.heightScreen : (int) VideoPlayer.localHeight);
            		lp.leftMargin = (int)(VideoPlayer.isFullScreen ? 0 : widthScreen - localX - (localWidth / 2) + 0.5);
            		lp.topMargin = (int)(VideoPlayer.isFullScreen ? 0 : heightScreen - localY - (localHeight / 2) + 0.5);
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

	public static void play()
	{
		Log.i(TAG, "Play.");
		if(useVLC)
		{
			try {
				if(videoEnded)
				{
					LibVlcUtil.getLibVlcInstance().playIndex(0);					
				}
				else
				{
					LibVlcUtil.getLibVlcInstance().play();
				}
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
		videoEnded = false;
	}


	public static void pause()
	{
		Log.i(TAG, "Pause.");
		if(useVLC)
		{
			try {
				LibVlcUtil.getLibVlcInstance().pause();
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else if(isPrepared)
		{
			((VideoView)videoView).pause();
		}
	}

	public static void stop()
	{
		Log.i(TAG, "Stop.");
		NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if(base != null && videoView != null)
				{
					if(useVLC)
					{
						try {
							LibVlcUtil.getLibVlcInstance().stop();
						} catch (LibVlcException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					else
					{
						((VideoView)videoView).stopPlayback();
					}
					base.removeAllViews();
					mainFrame.removeView(base);
				}
			}});
	}

	public static float getPlaybackRate()
	{
		if(useVLC)
		{
			try {
				return LibVlcUtil.getLibVlcInstance().getRate();
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		Log.e(TAG, "getPlaybackRate is only implemented for LibVLC");
		return 1;
	}

	public static void setPlaybackRate(float rate)
	{
		if(useVLC)
		{
			try {
				LibVlcUtil.getLibVlcInstance().setRate(rate);
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return;
		}
		Log.e(TAG, "setPlaybackRate is only implemented for LibVLC");
	}

	public static void setHideOnPause(boolean hide)
	{

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
		    		lp.leftMargin = (int)(isFullScreen ? 0 : widthScreen - localX - (localWidth / 2));
		    		lp.topMargin = (int)(isFullScreen ? 0 : heightScreen - localY - (localHeight / 2));
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
			try {
				duration = LibVlcUtil.getLibVlcInstance().getLength();
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
			try {
				position = LibVlcUtil.getLibVlcInstance().getTime();
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
			try {
				LibVlcUtil.getLibVlcInstance().setTime((long)(position * 1000));
			} catch (LibVlcException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
		        retriever.setDataSource(videoFile.getAbsolutePath());
		        int timeInSeconds = 1;
		        thumb = retriever.getFrameAtTime(timeInSeconds * 1000000,
		                    MediaMetadataRetriever.OPTION_CLOSEST_SYNC); 
				thumb.compress(CompressFormat.PNG, 80, streamThumbnail);
				thumb.recycle(); //ensure the image is freed;
		    } catch (Exception ex) {
		        Log.i(TAG, "MediaMetadataRetriever got exception:" + ex);
		        thumbPath = null;
		    }
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
	
	public static boolean videoExists(String path)
	{
		return getFile(path) != null;
	}
	
	public static File getFile(String path)
	{
		Uri localURI = NativeUtility.getMainActivity().getUriFromFileName(path);
		File localFile = null;
		final String[] storageDict = getStorageDirectories();
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
	
	//LibVLC specific method, implements IVideoPlayer
	@Override
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
        		lp.leftMargin = (int)(VideoPlayer.isFullScreen ? widthScreen - videoWidth - (widthScreen-videoWidth)/2 : widthScreen - localX - (videoWidth / 2));
        		lp.topMargin = (int)(VideoPlayer.isFullScreen ? heightScreen - videoHeight - (heightScreen-videoHeight)/2 : heightScreen - localY - (videoHeight / 2));
        		videoView.setLayoutParams(lp);
        		videoView.invalidate();
        	}
        });
    }
    
    @Override
    public void handleMessage(Message msg)
    {
    	int event = msg.getData().getInt("event", -1);
    	if(event == EventHandler.MediaPlayerVout)
    	{
    		Log.i(TAG, "MediaPlayer playing, duration : " + getDuration());  

    		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
    		{
    			public void run()
    			{
    				notifyVideoDurationAvailable(path, getDuration());
    			}
    		});
    	}
    	else if(event == EventHandler.MediaPlayerEndReached)
    	{
            try {
            	LibVLC vlc = LibVlcUtil.getLibVlcInstance();
            	if(shouldLoop)
            	{
            		vlc.playIndex(0);
            	}
            	else
            	{
            		videoEnded = true;
            		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
            		{
            			public void run()
            			{
            				notifyVideoEnded(path);
            			}
            		});
            	}
            } catch (LibVlcException e) {
                e.printStackTrace();
            }   		
    	}
    	else if(event == EventHandler.HardwareAccelerationError)
    	{
    		Log.i(TAG, "Hardware Acceleration Error, disabling hardware acceleration");
            try {
                LibVLC vlc = LibVlcUtil.getLibVlcInstance();
    			vlc.setHardwareAcceleration(0);	
    			vlc.playIndex(0);
            } catch (LibVlcException e) {
                e.printStackTrace();
            }
    	}
    }
    /**
     * attach and disattach surface to the lib
     */
    private final SurfaceHolder.Callback mSurfaceCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            if(format == PixelFormat.RGBX_8888)
                Log.d(TAG, "Pixel format is RGBX_8888");
            else if(format == PixelFormat.RGB_565)
                Log.d(TAG, "Pixel format is RGB_565");
            else if(format == ImageFormat.YV12)
                Log.d(TAG, "Pixel format is YV12");
            else
                Log.d(TAG, "Pixel format is other/unknown");
            LibVLC vlc = null;
            try {
                vlc = LibVlcUtil.getLibVlcInstance();
            } catch (LibVlcException e) {
                e.printStackTrace();
            }
            vlc.attachSurface(holder.getSurface(), VideoPlayer.getInstance());
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            LibVLC vlc = null;
            try {
                vlc = LibVlcUtil.getLibVlcInstance();
            } catch (LibVlcException e) {
                e.printStackTrace();
            }
            vlc.detachSurface();
        }
    };


    public static String[] getStorageDirectories()
    {
        String[] dirs = null;
        BufferedReader bufReader = null;
        try {
            bufReader = new BufferedReader(new FileReader("/proc/mounts"));
            ArrayList<String> list = new ArrayList<String>();
            list.add(Environment.getExternalStorageDirectory().getPath());
            String line;
            while((line = bufReader.readLine()) != null) {
                if(line.contains("vfat") || line.contains("exfat") ||
                        line.contains("/mnt") || line.contains("/Removable")) {
                    StringTokenizer tokens = new StringTokenizer(line, " ");
                    String s = tokens.nextToken();
                    s = tokens.nextToken(); // Take the second token, i.e. mount point

                    if (list.contains(s))
                        continue;

                    if (line.contains("/dev/block/vold")) {
                        if (!line.startsWith("tmpfs") &&
                                !line.startsWith("/dev/mapper") &&
                                !s.startsWith("/mnt/secure") &&
                                !s.startsWith("/mnt/shell") &&
                                !s.startsWith("/mnt/asec") &&
                                !s.startsWith("/mnt/obb")
                                ) {
                            list.add(s);
                        }
                    }
                }
            }

            dirs = new String[list.size()];
            for (int i = 0; i < list.size(); i++) {
                dirs[i] = list.get(i);
            }
        }
        catch (FileNotFoundException e) {}
        catch (IOException e) {}
        finally {
            if (bufReader != null) {
                try {
                    bufReader.close();
                }
                catch (IOException e) {}
            }
        }
        return dirs;
    }
}


