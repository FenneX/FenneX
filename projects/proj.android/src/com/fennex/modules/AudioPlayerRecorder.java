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

import android.util.Log;
import android.widget.Toast;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaRecorder;
import android.media.MediaPlayer;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;


public class AudioPlayerRecorder {
    private static final String LOG_TAG = "AudioPlayerRecorder";

    private static MediaRecorder mRecorder = null;
    private static MediaPlayer   mPlayer = null;
    private static FileInputStream input = null;
    private static String currentFile = null;
    private static float volume = 1;
    private static String[] audioTypes = {"mp3", "3gp", "flac", "ogg", "wav"};
    private static int repeatTimes = 0;
    
    public static native void notifyPlayingSoundEnded();
    
    public static boolean isPlaying()
    {
    	Log.d(LOG_TAG, "returning isPlaying");
    	return mPlayer != null && mPlayer.isPlaying();
    }
    
    public static boolean isRecording()
    {
    	Log.d(LOG_TAG, "returning isRecording");
    	return mRecorder != null;
    }

    public static void startPlaying(String fileName) 
    {
    	repeatTimes = 0;
        mPlayer = new MediaPlayer();
        mPlayer.setOnErrorListener(new OnErrorListener() {
            public boolean onError(MediaPlayer mp, int what, int extra) {
                Log.e(LOG_TAG, "Error with MediaPlayer, stopping it. What : " 
            + (what == MediaPlayer.MEDIA_ERROR_UNKNOWN ? "Unknown" : "Server died")
            + ", extra : "
            + (extra == MediaPlayer.MEDIA_ERROR_IO ? "IO Error" : 
            	extra == MediaPlayer.MEDIA_ERROR_UNSUPPORTED ? "Unsupported" : 
            		extra == MediaPlayer.MEDIA_ERROR_MALFORMED ? "Malformed" :
            			"Timed out"));
                AudioPlayerRecorder.stopPlaying();
                return true;
            }
        });
        mPlayer.setOnPreparedListener(new OnPreparedListener() {
            public void onPrepared(MediaPlayer mp) {
                mp.start();
                Log.i(LOG_TAG, "MediaPlayer started");
            }
        });
        mPlayer.setOnCompletionListener(new OnCompletionListener() {
            public void onCompletion(MediaPlayer mp) {
            	if(!mp.isLooping() && repeatTimes > 0)
            	{
            		mp.start();
            		repeatTimes--;
            	}
            	else
            	{
            		notifyPlayingSoundEnded();
            	}
            }
        });
        
        currentFile = startMediaPlayer(mPlayer, fileName, true, true);
        if(currentFile == null)
        {
        	mPlayer = null;
        	input = null;
        }
    }
    
    public static void playIndependent(String fileName) 
    {
        MediaPlayer tmpPlayer = new MediaPlayer();
        tmpPlayer.setOnErrorListener(new OnErrorListener() {
            public boolean onError(MediaPlayer mp, int what, int extra) {
                Log.e(LOG_TAG, "Error with MediaPlayer (getSoundDuration), stopping it. What : " 
            + (what == MediaPlayer.MEDIA_ERROR_UNKNOWN ? "Unknown" : "Server died")
            + ", extra : "
            + (extra == MediaPlayer.MEDIA_ERROR_IO ? "IO Error" : 
            	extra == MediaPlayer.MEDIA_ERROR_UNSUPPORTED ? "Unsupported" : 
            		extra == MediaPlayer.MEDIA_ERROR_MALFORMED ? "Malformed" :
            			"Timed out"));
                AudioPlayerRecorder.stopPlaying();
                return true;
            }
        });
        tmpPlayer.setOnPreparedListener(new OnPreparedListener() {
            public void onPrepared(MediaPlayer mp) {
                mp.start();
                Log.i(LOG_TAG, "Independent MediaPlayer started");
            }
        });
        tmpPlayer.setOnCompletionListener(new OnCompletionListener() {
            public void onCompletion(MediaPlayer mp) {
                mp.reset();
                mp.release();
                Log.i(LOG_TAG, "Independent MediaPlayer finished");
            }
        });
        
        startMediaPlayer(tmpPlayer, fileName, true, false);
    }
    
    //Return the full name of the file
    private static String startMediaPlayer(MediaPlayer player, String file, boolean asyncPrepare, boolean isMain)
    {
    	//try to load from data
    	String relativeName = file;
    	if(getFileExtension(file) == null) relativeName += ".mp3";
    	String fullName = NativeUtility.getLocalPath() + "/" + relativeName;
    	
    	Log.d(LOG_TAG, "start MediaPlayer from local : " + fullName);
        File target = new File(fullName);
        //Try to load from local path
        if(target != null && target.exists())
        {
            try 
            {
            	FileInputStream stream = new FileInputStream(target);
            	if(isMain)
            		input = stream;
            	player.setDataSource(stream.getFD());
            	
            	if(asyncPrepare)
            		player.prepareAsync();
            	else
            		player.prepare();
            } 
            catch (IOException e) 
            {
            	if(isMain)
            	{
                	fullName = null;
            	}
                Log.e(LOG_TAG, "prepare() from local failed, exception : " + e.getLocalizedMessage());
            }
        }
        //try to load from package using assets
        else
        {
            AssetManager am = NativeUtility.getMainActivity().getAssets();
        	Log.d(LOG_TAG, "start MediaPlayer from resources : " + relativeName);
            try 
            {
                AssetFileDescriptor descriptor = am.openFd(relativeName);
                FileDescriptor fileDesc = descriptor.getFileDescriptor();
                if(fileDesc.valid())
                {
                	player.setDataSource(fileDesc, descriptor.getStartOffset(), descriptor.getLength());
                	descriptor.close();
                	if(asyncPrepare)
                		player.prepareAsync();
                	else
                		player.prepare();
                }
                else
                {
                    Log.e(LOG_TAG, "No sound file matching : " + currentFile);
                	fullName = null;
                }
            } 
            catch (IOException e) 
            {
            	fullName = null;
                Log.e(LOG_TAG, "prepare() failed from resources, exception : " + e.getLocalizedMessage());
            }
        }
        return fullName;
    }
    
    public static void stopPlaying() 
    {
    	Log.d(LOG_TAG, "stop playing");
    	if(mPlayer != null)
    	{
    		mPlayer.stop();
            mPlayer.release();
            if(input != null)
            {
	            try {
	    			input.close();
	    		} catch (IOException e) {
	    			// TODO Auto-generated catch block
	    			e.printStackTrace();
	    		}
	            input = null;
            }
            mPlayer = null;
    	}
    }
    public static void startRecording(String fileName) 
    {
    	currentFile = NativeUtility.getLocalPath() + "/" + fileName;
    	Log.d(LOG_TAG, "start recording : " + currentFile);
        mRecorder = new MediaRecorder();
        mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
        mRecorder.setAudioChannels(1);
        mRecorder.setAudioSamplingRate(44100);
        mRecorder.setOutputFile(currentFile);
        mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);

        try 
        {
            mRecorder.prepare();
        } 
        catch (IOException e) 
        {
            Log.e(LOG_TAG, "prepare() failed");
        }

        mRecorder.start();
    }

    public static void stopRecording() 
    {
    	Log.d(LOG_TAG, "stop recording");
        mRecorder.stop();
        mRecorder.release();
        mRecorder = null;
    }
    
    public static void deleteFile(String fileName) 
    {
    	currentFile = NativeUtility.getLocalPath() + "/" + fileName;
    	File file = new File(currentFile);
    	if(!file.delete())
    	{
    		Log.e(LOG_TAG, "Couldn't delete file : " + currentFile);
    	}
    }
    
    public static void fadeVolumeOut()
    {
    	final float speed = 0.010f;

    	Log.d(LOG_TAG, "fade volume out");
    	if(mPlayer != null)
    	{
    		new Thread(new Runnable() {
    	        public void run() {
    	        	while(volume >= 0 && mPlayer.isPlaying())
    	    		{
    	    			volume -= speed;
    	    			mPlayer.setVolume(volume, volume);
    	    			try {
							Thread.sleep(25);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
    	    		}
    	        	mPlayer.stop();
    	        	volume = 1;
    	        }
    	    }).start();
    	}
    }
    
    static void setNumberOfLoops(int loops)
    {
		if(mPlayer != null)
		{
			mPlayer.setLooping(loops < 0);
		}
    	if(loops >= 0)
    	{
    		repeatTimes = loops;
    	}
    }

    public static void play()
    {
    	Log.d(LOG_TAG, "play music");
    	if(mPlayer != null)
    			mPlayer.start();
    }

    public static void pause()
    {
    	Log.d(LOG_TAG, "pause music");
    	if(mPlayer != null)
    		if(mPlayer.isPlaying())
    			mPlayer.pause();
    }
    
    public static void restart()
    {
    	Log.d(LOG_TAG, "restart music");
    	//stopPlaying();
    	//startPlaying(file);
    	if(mPlayer != null)
    		mPlayer.seekTo(0);
    }
    
    public static float getSoundDuration(String fileName)
    {
        MediaPlayer tmpPlayer = new MediaPlayer();
        tmpPlayer.setOnErrorListener(new OnErrorListener() {
            public boolean onError(MediaPlayer mp, int what, int extra) {
                Log.e(LOG_TAG, "Error with MediaPlayer (getSoundDuration), stopping it. What : " 
            + (what == MediaPlayer.MEDIA_ERROR_UNKNOWN ? "Unknown" : "Server died")
            + ", extra : "
            + (extra == MediaPlayer.MEDIA_ERROR_IO ? "IO Error" : 
            	extra == MediaPlayer.MEDIA_ERROR_UNSUPPORTED ? "Unsupported" : 
            		extra == MediaPlayer.MEDIA_ERROR_MALFORMED ? "Malformed" :
            			"Timed out"));
                AudioPlayerRecorder.stopPlaying();
                return true;
            }
        });
        
        String fullName = startMediaPlayer(tmpPlayer, fileName, false, false);
        //Convert to seconds
        float duration = (float) (fullName != null ? (float)tmpPlayer.getDuration() / 1000.0 : 0.0);
        tmpPlayer.reset();
        tmpPlayer.release();
        return duration;
    }
    
    //Will return the '.' with the extension
    public static String getFileExtension(String file)
    {
    	if(file.contains("."))
    	{
    		String extension = file.substring(file.lastIndexOf('.'));
    		if(isAValidAudioFormat(extension))
    			return file.substring(file.indexOf('.'));
    	}
    		return null;
    }
    
    public static boolean isAValidAudioFormat(String extension)
    {
    	for(int i = 0; i < audioTypes.length; i++)
    	{
    		if(extension.endsWith(audioTypes[i]))
    			return true;
    	}
    	return false;
    }
    
    public static boolean checkMicrophonePermission ()
    {
    	boolean permissionOK = NativeUtility.getMainActivity().checkCallingOrSelfPermission("android.permission.RECORD_AUDIO") == PackageManager.PERMISSION_GRANTED;
    	if(!permissionOK)
    	{
    		Toast.makeText(NativeUtility.getMainActivity(), "Warning, missiong RECORD_AUDIO permission", Toast.LENGTH_LONG).show();
    	}
    	return permissionOK;
    }
    
    private static MediaMetadataRetriever getMetadata(String file)
    {
    	String relativeName = file;
    	if(getFileExtension(file) == null) relativeName += ".mp3";
    	String fullName = NativeUtility.getLocalPath() + "/" + relativeName;
    	
        File target = new File(fullName);
        //Try to load from local path
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        if(target != null && target.exists())
        {
            try 
            {
            	FileInputStream stream = new FileInputStream(target);
            	retriever.setDataSource(stream.getFD());
            } 
            catch (IOException e) 
            {
                Log.e(LOG_TAG, "MediaMetadataRetriever from local failed, exception : " + e.getLocalizedMessage());
            }
        }
        //try to load from package using assets
        else
        {
            AssetManager am = NativeUtility.getMainActivity().getAssets();
            try 
            {
                AssetFileDescriptor descriptor = am.openFd(relativeName);
                FileDescriptor fileDesc = descriptor.getFileDescriptor();
                if(fileDesc.valid())
                {
                	retriever.setDataSource(fileDesc, descriptor.getStartOffset(), descriptor.getLength());
                	descriptor.close();
                }
                else
                {
                    Log.e(LOG_TAG, "No sound file matching : " + currentFile);
                }
            } 
            catch (IOException e) 
            {
                Log.e(LOG_TAG, "MediaMetadataRetriever failed from resources, exception : " + e.getLocalizedMessage());
            }
        }
        return retriever;
    }
    
    public static String getAuthor(String file)
    {
        return getMetadata(file).extractMetadata(MediaMetadataRetriever.METADATA_KEY_ARTIST);
    }
    
    public static String getTitle(String file)
    {
        return getMetadata(file).extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);
    }
}
