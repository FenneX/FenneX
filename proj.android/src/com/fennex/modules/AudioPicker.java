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
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.annotation.TargetApi;
import android.content.Intent;
import android.os.Build;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

public class AudioPicker implements ActivityResultResponder{
private static final String TAG = "AudioPicker";
private static final int MUSIC_LIBRARY = 30;

private static String _fileName;
private static String _identifier;
private static String storageDirectory;
private static boolean isPending = false;

private static volatile AudioPicker instance = null;
    
    private AudioPicker() { }

    public static AudioPicker getInstance() 
    {
        if (instance == null) 
        {
            isPending = false;
            synchronized (AudioPicker .class)
            {
                if (instance == null) 
                {
                	instance = new AudioPicker ();
        			NativeUtility.getMainActivity().addResponder(instance);
                }
            }
        }
        return instance;
    }

    public void destroy()
    {
        if(isPending)
        {
            Toast.makeText(NativeUtility.getMainActivity(), TOO_MUCH_APP, Toast.LENGTH_LONG).show();
            isPending = false;
        }
        instance = null;
    }
	
	public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        isPending = false;
		Log.d(TAG, "OnActivityResult: requestCode = " + requestCode + " resultCode = " + resultCode + " data = " + data);
		if(requestCode == MUSIC_LIBRARY)
		{
			try {
				InputStream is = NativeUtility.getMainActivity().getContentResolver().openInputStream(data.getData());
				byte buffer[] = new byte[is.available()];
				Log.d(TAG, FileUtility.getLocalPath() + "/" + _fileName + ".mp3");
				FileOutputStream selectedMusic = new FileOutputStream(FileUtility.getLocalPath() + "/" + _fileName + ".mp3");
				//FileOutputStream selectedMusic = new FileOutputStream(storageDirectory + "/" + _fileName + ".mp3");
				is.read(buffer);
				selectedMusic.write(buffer);
        		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
        		{
        			public void run()
        			{
        				notifySoundPickedWrap(_fileName+".mp3", _identifier);
        			}
        		});
				return true;
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return false;
	}

	@TargetApi(19)
	public static boolean pickSound(String promptText, String saveName, String identifier)
	{
		Log.d(TAG, "promptText: " + promptText + " | saveName: " + saveName);
		AudioPicker.getInstance();
		Intent intent;
		Log.d(TAG, "Build version:" + Build.VERSION.SDK_INT);
		_fileName = saveName;
		_identifier = identifier;
		if (Build.VERSION.SDK_INT >= 19){
			intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		    intent.addCategory(Intent.CATEGORY_OPENABLE);
		} else {
			intent = new Intent(Intent.ACTION_GET_CONTENT);
			//createTemporaryFolder();
		}
		 intent.setType("audio/*");
        isPending = true;
		 NativeUtility.getMainActivity().startActivityForResult(intent, MUSIC_LIBRARY);
		return true;
	}
	
	void notifySoundPicked(String name, String identifier){
		//TODO notifySoundPicked
	}

	void notifySoundEncoded(String name, String identifier){
		//TODO notifySoundEncoded
	}

	String audioPickerCurrentExport(){
		//TODO audioPickerCurrentExport
		return null;
	}

	void stopAudioPickerExport(){
		//TODO stopAudioPickerExport
	}
	
	static void createTemporaryFolder()
	{
		if((Environment.getExternalStorageState() != Environment.MEDIA_BAD_REMOVAL &&
        		Environment.getExternalStorageState() != Environment.MEDIA_MOUNTED_READ_ONLY &&
        		Environment.getExternalStorageState() != Environment.MEDIA_NOFS &&
        		Environment.getExternalStorageState() != Environment.MEDIA_REMOVED &&
        		Environment.getExternalStorageState() != Environment.MEDIA_UNMOUNTABLE &&
        		Environment.getExternalStorageState() != Environment.MEDIA_UNMOUNTED))
        {
			storageDirectory = Environment.getExternalStorageDirectory().toString();
	    	File directory = new File(storageDirectory + "/TimeIn");
	    	directory.delete();
	    	storageDirectory += "/TimeIn";
	    	if(directory.mkdir())
			{
				Log.d(TAG, "Folder " + storageDirectory + " does not exist, folder created.");
			}
			else
				Log.d(TAG, "Folder " + storageDirectory + " does not exist, cannot create folder.");
        }
		else
			Log.e(TAG, "Error external storage.");
	}
	
	 private native static void notifySoundPickedWrap(String name, String identifier);
}
