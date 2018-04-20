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


import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;



public class MailUrlManager {
	private static final String TAG = "MailUrlManager";
    

	private static volatile MailUrlManager instance = null;
	public static MailUrlManager getInstance()
	{
		if(instance == null)
		{
			instance = new MailUrlManager();
		}
		return instance;
	}
	
	public static void openUrl(String url) 
    {
		Log.d(TAG, "Opening URL : " + url);
	     Intent i = new Intent(Intent.ACTION_VIEW);  
	     i.setData(Uri.parse(url));
	     NativeUtility.getMainActivity().startActivity(i);
    }
    
	public static boolean canOpenUrl(String url)
	{
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse(url));
		PackageManager packageManager = NativeUtility.getMainActivity().getPackageManager();
		return (i.resolveActivity(packageManager) != null);
	}

    public static void sendMail(String address, String object, String message)
    {
        Log.d(TAG, "Sending mail : ");
        Intent i = new Intent(Intent.ACTION_SEND);
        i.setType("message/rfc822");
        i.putExtra(Intent.EXTRA_EMAIL  , new String[]{address});
        i.putExtra(Intent.EXTRA_SUBJECT, object);
        i.putExtra(Intent.EXTRA_TEXT   , message);
        try {
            NativeUtility.getMainActivity().startActivity(Intent.createChooser(i, "Send mail..."));
        } catch (android.content.ActivityNotFoundException ex) {
            Toast.makeText(NativeUtility.getMainActivity(), "There are no email clients installed.", Toast.LENGTH_SHORT).show();
        }
    }

	
	public static boolean copyToSDCard(String file, String resultName)
	{
		if(!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
		{
	        Log.e(TAG, "Media not mounted");
			return false;
		}
        Log.d(TAG, "Destination : " + Environment.getExternalStorageDirectory() + java.io.File.separator + resultName);
        File destinationFile = new File(Environment.getExternalStorageDirectory() + java.io.File.separator + resultName); 
        Log.d(TAG, "Source : " + FileUtility.getLocalPath() + "/" + file);
        File sourceFile = new File(FileUtility.getLocalPath() + "/" + file);
        if(!sourceFile.canRead())
        {
	        Log.e(TAG, "Can't read from source file");
        	return false;
        }
        FileInputStream in;
        FileOutputStream f;
		try {
			in = new FileInputStream(sourceFile);
	        f = new FileOutputStream(destinationFile); 
	        byte[] buffer = new byte[1024];
	        int len1 = 0;
	        while ((len1 = in.read(buffer)) > 0) {
	            f.write(buffer, 0, len1);
				Log.d(TAG, "writing file ...");
	        }
	        f.close();
	        in.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			Log.e(TAG, "File not found exception");
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			Log.e(TAG, "IO found exception");
			return false;
		}
		Log.d(TAG, "File written to SDCard");
		/* Don't seem to be necessary, reactivate it if for some reason there is no attachment
		try {
			Runtime.getRuntime().exec("chmod 777 " + "file://" + Environment.getExternalStorageDirectory() + java.io.File.separator + resultName);
		} catch (IOException e1) {
			e1.printStackTrace();
			Log.e(TAG, "Problem setting permissions");
			return false;
		}*/
    	return true;
	}
	
	public static void sendMailWithAttachment(String address, String object, String message, String attachment) 
    {
		Log.d(TAG, "Sending mail : ");
		Intent i = new Intent(Intent.ACTION_SEND);
		i.setType("application/xml");
	    i.putExtra(Intent.EXTRA_EMAIL  , new String[]{address});
	    i.putExtra(Intent.EXTRA_SUBJECT, object);
	    i.putExtra(Intent.EXTRA_TEXT   , message);
	    if(!MailUrlManager.copyToSDCard(attachment, "result.plist"))
	    {
	    	Log.e(TAG, "Copy file to SDCard failed, aborting message send");
	        Toast.makeText(NativeUtility.getMainActivity(), "Problem copying file to a readable location", Toast.LENGTH_SHORT).show();
	    	return;
	    }
	    File file = new File(Environment.getExternalStorageDirectory() + java.io.File.separator + "result.plist");
	    Uri uri = Uri.parse("file://" +
	            file.getAbsolutePath());
	    Log.d(TAG, "attachment location : " + "file://"+ file.getAbsolutePath());
	    
	    i.putExtra(Intent.EXTRA_STREAM, uri);
	    try {
	    	NativeUtility.getMainActivity().startActivity(Intent.createChooser(i, "Send mail..."));
	    } catch (android.content.ActivityNotFoundException ex) {
	        Toast.makeText(NativeUtility.getMainActivity(), "There are no email clients installed.", Toast.LENGTH_SHORT).show();
	    }
    }
}
