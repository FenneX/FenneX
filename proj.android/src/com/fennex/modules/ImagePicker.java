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
import java.util.ArrayList;


import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;

public class ImagePicker implements ActivityResultResponder
{
    private static final String TAG = "ImagePicker";
    private static final int PICTURE_GALLERY = 30;
    private static final int CAMERA_CAPTURE = 31;
    private static final int CROP = 32;
    private static String storageDirectory;
    private static boolean stateStorage = false;
    
	private static String _fileName;
	private static int _width;
	private static int _height;
	private static String _identifier;
	private Uri uriOfSavedPhoto;
	
    private static volatile ImagePicker instance = null;
    
    private ImagePicker() { }

    public static ImagePicker getInstance() 
    {
        if (instance == null) 
        {
            synchronized (ImagePicker .class)
            {
                if (instance == null) 
                {
                	instance = new ImagePicker ();
        			NativeUtility.getMainActivity().addResponder(instance);
        			getStorageDirectory();
                }
            }
        }
        return instance;
    }
	
	//Return true if it uses the activity result is handled by the in-app module
    public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data.getExtras());
        if (requestCode == PICTURE_GALLERY || requestCode == CAMERA_CAPTURE || requestCode == CROP)
		{
			Log.d(TAG, "intent data: " + data.getDataString());
			Log.d(TAG, "local path: " + NativeUtility.getLocalPath());
			Bitmap original = null;
			if(requestCode == CROP)
			{
				original = BitmapFactory.decodeFile(storageDirectory+"/cropped.png");
				if(original != null)
				{
					Bitmap bitmap = scaleToFill(original, 320, 320);
					Bitmap bitmapThumbnail = scaleToFill(original, 320, 320);
					original.recycle(); //this one may be huge, might as well free it right now
					try {
						String path = NativeUtility.getMainActivity().getFilesDir().getPath().concat("/".concat(_fileName));
						Log.d(TAG, "saving at path: " + path);
						FileOutputStream stream = new FileOutputStream(path);
						/* Write bitmap to file using JPEG or PNG and 80% quality hint for JPEG. */
						bitmap.compress(CompressFormat.PNG, 100, stream);
						bitmap.recycle(); //ensure the image is freed;
						stream.close();
						_fileName = _fileName.replaceAll(".png", "");
						FileOutputStream streamThumbnail = new FileOutputStream(NativeUtility.getMainActivity().getFilesDir().getPath() + "/" + _fileName + "-thumbnail.png");
						bitmapThumbnail.compress(CompressFormat.PNG, 80, streamThumbnail);
						bitmapThumbnail.recycle(); //ensure the image is freed;
						streamThumbnail.close();
						Log.d(TAG, "file saved, name : " + _fileName + " identifier : " + _identifier);

                		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
                		{
                			public void run()
	            			{
                				notifyImagePickedWrap(_fileName, _identifier);
	            			}
                		});
					} catch (FileNotFoundException e) {
						Log.d(TAG, "File Not Found Exception : check directory path");
						e.printStackTrace();
					} catch (IOException e) {
						Log.d(TAG, "IOException while closing the stream");
						e.printStackTrace();
					}
				}
			}
			else if(requestCode == PICTURE_GALLERY || requestCode == CAMERA_CAPTURE)
			{
				/**
				 * Store the picture in a specific folder to prevent the apps crashing when the cropped picture
				 * is too big and to avoid black borders in the cropped picture when it is too small
				 */
				Intent cropIntent = new Intent("com.android.camera.action.CROP");
				if(requestCode == PICTURE_GALLERY)
					cropIntent.setDataAndType(Uri.parse(data.getDataString()), "image/png");
				else
				{
					insertPhotoIntoGallery(original, data);
					cropIntent.setDataAndType(uriOfSavedPhoto, "image/png");
				}
				cropIntent.putExtra("aspectX", 1);
				cropIntent.putExtra("aspectY", 1);
				Log.i(TAG, "Will save in " + storageDirectory + "/cropped.png");
				cropIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(new File(storageDirectory, "cropped.png")));
				NativeUtility.getMainActivity().startActivityForResult(cropIntent, CROP);
			}
			return true;
		}		
		else 
		{
			Log.d(TAG, "not image picker activity");
		}
        return false;
    }
    
    public static boolean isCameraAvailable()
    {
    	//Note : support front and back camera since API level9+. We don't use PackageManager.FEATURE_CAMERA_ANY because it is since
    	//API level17+
    	return NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT) ||
    			NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }
    
    public static boolean pickImageFrom(String saveName, boolean useCamera, int width, int height, String identifier)
    {
    	ImagePicker.getInstance(); //ensure the instance is created
    	_fileName = saveName.concat(".png");
    	_width = width;
    	_height = height;
    	_identifier = identifier;
    	boolean error = false;
    	if(useCamera)
    	{
    		try
    		{
	       		Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		        NativeUtility.getMainActivity().startActivityForResult(intent, CAMERA_CAPTURE);
    		}
    		catch(ActivityNotFoundException e)
    		{
    	    	Log.d(TAG, "intent for image capture not found : " + e.getMessage());
    	    	error = true;
    		}
    	}
    	else
    	{
    		try
    		{
    			Intent intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
    			intent.setType("image/*");
    			NativeUtility.getMainActivity().startActivityForResult(intent, PICTURE_GALLERY);
    		}
    		catch(ActivityNotFoundException e)
    		{
    	    	Log.d(TAG, "intent for image pick from library not found : " + e.getMessage());
    	    	error = true;
    		}
    	}
    	return error;
    }
    
    static public void getStorageDirectory() {
    	if((Environment.getExternalStorageState() != Environment.MEDIA_BAD_REMOVAL &&
        		Environment.getExternalStorageState() != Environment.MEDIA_MOUNTED_READ_ONLY &&
        		Environment.getExternalStorageState() != Environment.MEDIA_NOFS &&
        		Environment.getExternalStorageState() != Environment.MEDIA_REMOVED &&
        		Environment.getExternalStorageState() != Environment.MEDIA_UNMOUNTABLE &&
        		Environment.getExternalStorageState() != Environment.MEDIA_UNMOUNTED))
        {
        	stateStorage = true;
        	/**
        	 * Use getExternalStorageDirectory() to save the cropped picture because we cannot use 
        	 * the folder /data/data/... to save temporary picture.
        	 */
        	storageDirectory = Environment.getExternalStorageDirectory().toString();
        	File directory = new File(storageDirectory + "/" + NativeUtility.getAppName());
        	if(!directory.exists())
        	{        		
        		if(directory.mkdir())
        		{
        			storageDirectory = storageDirectory + "/" + NativeUtility.getAppName();
        			Log.d(TAG, "Folder " + storageDirectory + " does not exist, folder created.");
        		}
        		else
        			Log.d(TAG, "Folder " + storageDirectory + " does not exist, cannot create folder.");
        	}
        	else
        		storageDirectory = storageDirectory + "/" + NativeUtility.getAppName();
        }
    }
    
    // Scale and keep aspect ratio 
    static public Bitmap scaleToFill(Bitmap b, int width, int height) {
        float factorH = height / (float) b.getHeight();
        float factorW = width / (float) b.getWidth();
        float factorToUse = (factorH > factorW) ? factorW : factorH;
        return Bitmap.createScaledBitmap(b, (int) (b.getWidth() * factorToUse), (int) (b.getHeight() * factorToUse), true);  
    }

    // Scale and dont keep aspect ratio 
    static public Bitmap strechToFill(Bitmap b, int width, int height) {
        float factorH = height / (float) b.getHeight();
        float factorW = width / (float) b.getWidth();
        return Bitmap.createScaledBitmap(b, (int) (b.getWidth() * factorW), (int) (b.getHeight() * factorH), true);  
    }
    
    //We don't use this method right now but it can be useful if we encounter another bug with a specific device
    public static boolean hasImageCaptureBug() {

        // list of known devices that have the bug
        ArrayList<String> devices = new ArrayList<String>();
        devices.add("android-devphone1/dream_devphone/dream");
        devices.add("generic/sdk/generic");
        devices.add("vodafone/vfpioneer/sapphire");
        devices.add("tmobile/kila/dream");
        devices.add("verizon/voles/sholes");
        devices.add("google_ion/google_ion/sapphire");

        return devices.contains(android.os.Build.BRAND + "/" + android.os.Build.PRODUCT + "/"
                + android.os.Build.DEVICE);
    }
    
    public void insertPhotoIntoGallery(Bitmap original, Intent data)
    {
        File fi = new File(storageDirectory, "photo.png");
        fi.setReadable(true, false);
        original = data.getParcelableExtra("data");
		FileOutputStream stream;
		try {
			stream = new FileOutputStream(fi);
			/* Write bitmap to file using JPEG or PNG and 80% quality hint for JPEG. */
			original.compress(CompressFormat.PNG, 100, stream);
			stream.close();
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
        try {
        	uriOfSavedPhoto = Uri.parse(android.provider.MediaStore.Images.Media.insertImage(NativeUtility.getMainActivity().getContentResolver(), fi.getAbsolutePath(), null, null));
            if (!fi.delete()) {
                Log.i(TAG, "Failed to delete " + fi);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
    
    private native static void notifyImagePickedWrap(String name, String identifier);
}
