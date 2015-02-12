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
    
	private static String _fileName;
	private static int _width;
	private static int _height;
	private static String _identifier;
    private static float _thumbnailScale;
    private static boolean _rescale;
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
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + (data != null ? data.getExtras() : "no data"));
        if (requestCode == PICTURE_GALLERY || requestCode == CAMERA_CAPTURE || requestCode == CROP)
		{
            if(requestCode == CROP || !_rescale)
            {
                try {
                    Bitmap original;
                    if(requestCode == CROP) {
                        original = BitmapFactory.decodeFile(storageDirectory+"/cropped.png");
                    }
                    else if(requestCode == PICTURE_GALLERY) {
                        Uri selectedImage = data.getData();
                        InputStream imageStream = NativeUtility.getMainActivity().getContentResolver().openInputStream(selectedImage);
                        original = BitmapFactory.decodeStream(imageStream);
                    }
                    else { //CAMERA_CAPTURE
                        /* When not using EXTRA_OUTPUT, the Intent will produce a small image, depending on device and app used
                        original = data.getParcelableExtra("data");*/
                        //The EXTRA_OUTPUT parameter of Intent
                        File file = new File(storageDirectory +  "/" + _fileName.substring(0, _fileName.length() - 4) + ".jpg");
                        original = BitmapFactory.decodeFile(file.getAbsolutePath());
                        //Clean file
                        file.delete();
                    }
                    if(original != null) {
                        Bitmap bitmap = scaleToFill(original, _width, _height);
                        //Camera handles the Bitmap itself
                        if(requestCode != CAMERA_CAPTURE && original != bitmap) {
                            original.recycle(); //this one may be huge, might as well free it right now
                        }
                        _fileName = _fileName.replaceAll(".png", "");
                        if(_thumbnailScale > 0) {
                            Bitmap bitmapThumbnail = scaleToFill(bitmap, (int)(_width * _thumbnailScale), (int)(_height * _thumbnailScale));
                            saveBitmap(bitmapThumbnail, NativeUtility.getMainActivity().getFilesDir().getPath() + "/" + _fileName + "-thumbnail.png");
                        }
                        saveBitmap(bitmap, NativeUtility.getMainActivity().getFilesDir().getPath() + "/" + _fileName + ".png");
                        NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                            public void run() {
                                notifyImagePickedWrap(_fileName, _identifier);
                            }
                        });
                    }
                } catch (IOException e) {
                    Log.d(TAG, "IOException while handling Intent result");
                    e.printStackTrace();
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
					insertPhotoIntoGallery(data);
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

    private static void saveBitmap(Bitmap bitmap, String path) throws IOException {
        FileOutputStream stream = new FileOutputStream(path);
        bitmap.compress(CompressFormat.PNG, 100, stream);
        bitmap.recycle(); //ensure the image is freed;
        stream.close();
    }

    public static boolean isCameraAvailable()
    {
    	//Note : support front and back camera since API level9+. We don't use PackageManager.FEATURE_CAMERA_ANY because it is since
    	//API level17+
    	return NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT) ||
    			NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }
    
    public static boolean pickImageFrom(String saveName, boolean useCamera, int width, int height, String identifier, float thumbnailScale, boolean rescale)
    {
    	ImagePicker.getInstance(); //ensure the instance is created
    	_fileName = saveName.concat(".png");
    	_width = width;
    	_height = height;
    	_identifier = identifier;
        _thumbnailScale = thumbnailScale;
        _rescale = rescale;
    	boolean error = false;
    	if(useCamera)
    	{
    		try
    		{
	       		Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                if(!rescale) {
                    File file = new File(storageDirectory +  "/" + saveName + ".jpg");
                    Uri outputFileUri = Uri.fromFile(file);
                    intent.putExtra(MediaStore.EXTRA_OUTPUT, outputFileUri);
                }
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
        if(factorToUse >= 1) {
            return b;
        }
        return Bitmap.createScaledBitmap(b, (int) (b.getWidth() * factorToUse), (int) (b.getHeight() * factorToUse), true);  
    }
    
    public void insertPhotoIntoGallery(Intent data)
    {
        File fi = new File(storageDirectory, "photo.png");
        fi.setReadable(true, false);
        Bitmap original = data.getParcelableExtra("data");
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
