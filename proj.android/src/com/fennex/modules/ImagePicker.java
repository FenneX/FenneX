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

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import androidx.exifinterface.media.ExifInterface;

import static android.app.Activity.RESULT_CANCELED;

public class ImagePicker implements ActivityResultResponder
{
    private static final String TAG = "ImagePicker";
    private static final int PICTURE_GALLERY = 30;

    private static boolean isPending = false;
	
    private static volatile ImagePicker instance = null;
    
    private ImagePicker() { }

    public static ImagePicker getInstance() 
    {
        if (instance == null) 
        {
            isPending = false;
            synchronized (ImagePicker .class)
            {
                if (instance == null) 
                {
                	instance = new ImagePicker ();
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
            NativeUtility.showToast("TooManyAppsLaunched", Toast.LENGTH_LONG);
            isPending = false;
        }
        instance = null;
    }
	
	//Return true if it uses the activity result is handled by the in-app module
    public boolean onActivityResult(int requestCode, int resultCode, Intent data){
        isPending = false;
        if(resultCode == RESULT_CANCELED)
        {
            NativeUtility.getMainActivity().runOnGLThread(ImagePicker::notifyImagePickCancelled);
        }
        else if (requestCode == PICTURE_GALLERY)
		{
            Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + (data != null ? data.getExtras() : "no data"));
            try {
                try {
                    assert data != null;
                    Uri selectedImage = data.getData();
                    assert selectedImage != null;

                    //Try to get exif data from the stream (will default to rotation = 0 if anything fails)
                    ExifInterface exif = null;
                    try (InputStream in = NativeUtility.getMainActivity().getContentResolver().openInputStream(selectedImage)) {
                        assert in != null;
                        exif = new ExifInterface(in);
                    } catch (IOException ignored) {}

                    //Open the image and convert it into a Bitmap
                    InputStream imageStream = NativeUtility.getMainActivity().getContentResolver().openInputStream(selectedImage);
                    assert imageStream != null;
                    Bitmap original = BitmapFactory.decodeStream(imageStream);

                    //Rotate according to exif data
                    Bitmap rotatedImage = rotateImage(original, getRotationFromExif(exif));
                    if(original != rotatedImage)  original.recycle();
                    imageStream.close();

                    //Save rotated image to a temporary file
                    File file = File.createTempFile("PickedImage", ".jpg");
                    FileOutputStream stream = new FileOutputStream(file);
                    rotatedImage.compress(CompressFormat.JPEG, 100, stream);
                    stream.close();
                    rotatedImage.recycle();

                    //notify native
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyImagePickedWrap(file.getAbsolutePath()));
                }
                catch (OutOfMemoryError e) {
                    Log.e(TAG, "OutOfMemoryError on ImagePicker while trying to get original Bitmap");
                    NativeUtility.showToast("ImageTooLarge", Toast.LENGTH_LONG);
                    NativeUtility.getMainActivity().runOnGLThread(ImagePicker::notifyImagePickCancelled);
                }
            } catch (IOException e) {
                Log.d(TAG, "IOException while handling Intent result");
                e.printStackTrace();
                NativeUtility.getMainActivity().runOnGLThread(ImagePicker::notifyImagePickCancelled);
            }
            return true;
        } else {
            Log.d(TAG, "not image picker activity");
        }
        return false;
    }

    private int getRotationFromExif(ExifInterface data) {
        if (data == null) return 0;
        int exifOrientation = data.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
        int rotation = 0;
        if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_90) {
            rotation = 90;
        } else if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_180) {
            rotation = 180;
        } else if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_270) {
            rotation = 270;
        }
        return rotation;
    }

    private Bitmap rotateImage(Bitmap image, int rotationAngle)
    {
        if(rotationAngle != 0)
        {
            Matrix matrix = new Matrix();
            matrix.postRotate(rotationAngle);
            image = Bitmap.createBitmap(image, 0, 0, image.getWidth(), image.getHeight(), matrix, true);
        }
        return image;
    }

    @SuppressWarnings({"WeakerAccess", "BooleanMethodIsAlwaysInverted"})
    public static boolean isCameraAvailable()
    {
    	//Note : support front and back camera since API level9+. We don't use PackageManager.FEATURE_CAMERA_ANY because it is since
    	//API level17+
    	return NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT) ||
    			NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }
    
    @SuppressWarnings("unused")
    public static void pickImageWithWidget()
    {
    	ImagePicker.getInstance(); //ensure the instance is created
        try
        {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT );
            intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
            intent.setDataAndType(android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "image/*");
            isPending = true;
            NativeUtility.getMainActivity().startActivityForResult(intent, PICTURE_GALLERY);
        }
        catch(ActivityNotFoundException e)
        {
            Log.d(TAG, "intent for image pick from File library not found : " + e.getMessage());
        }
    }
    
    protected native static void notifyImagePickedWrap(String fullpath);
    private native static void notifyImagePickCancelled();
}
