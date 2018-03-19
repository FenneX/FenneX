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
import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import static android.app.Activity.RESULT_CANCELED;

public class ImagePicker implements ActivityResultResponder
{
    private static final String TAG = "ImagePicker";
    private static final int PICTURE_GALLERY = 30;
    private static final int CAMERA_CAPTURE = 31;
    private static final int CROP = 32;
    
	private static String _fileName;
    private static FileUtility.FileLocation _location;
	private static int _width;
	private static int _height;
	private static String _identifier;
    private static float _thumbnailScale;
    private static boolean _rescale;
	private Uri uriOfSavedPhoto;
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
            Toast.makeText(NativeUtility.getMainActivity(), TOO_MUCH_APP, Toast.LENGTH_LONG).show();
            isPending = false;
        }
        instance = null;
    }
	
	//Return true if it uses the activity result is handled by the in-app module
    public boolean onActivityResult(int requestCode, int resultCode, Intent data){
        isPending = false;
        if(resultCode == RESULT_CANCELED)
        {
            notifyImagePickCancelled();
        }
        else if (requestCode == PICTURE_GALLERY || requestCode == CAMERA_CAPTURE || requestCode == CROP)
		{
            Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + (data != null ? data.getExtras() : "no data"));
            try {
                if (requestCode == CROP || !_rescale) {
                    Bitmap original;
                    if (requestCode == CROP) {
                        original = BitmapFactory.decodeFile(getStorageDirectory() + "/cropped.png");
                    } else if (requestCode == PICTURE_GALLERY) {
                        Uri selectedImage = data.getData();
                        String[] orientationColumn = {MediaStore.Images.Media.ORIENTATION};
                        Cursor cur = NativeUtility.getMainActivity().getContentResolver().query(selectedImage, orientationColumn, null, null, null);
                        int orientation = 0;
                        if (cur != null && cur.moveToFirst()) {
                            orientation = cur.getInt(cur.getColumnIndex(orientationColumn[0]));
                        }
                        InputStream imageStream;
                        imageStream = NativeUtility.getMainActivity().getContentResolver().openInputStream(selectedImage);
                        original = BitmapFactory.decodeStream(imageStream);
                        original = rotateImage(original, orientation);
                    } else { //CAMERA_CAPTURE
                        /* When not using EXTRA_OUTPUT, the Intent will produce a small image, depending on device and app used
                        original = data.getParcelableExtra("data");*/
                        //The EXTRA_OUTPUT parameter of Intent
                        File file = new File(getStorageDirectory(), _fileName + ".jpg");
                        ExifInterface exif = new ExifInterface(file.getAbsolutePath());
                        int exifOrientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
                        int rotation = 0;
                        if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_90) {
                            rotation = 90;
                        } else if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_180) {
                            rotation = 180;
                        } else if (exifOrientation == ExifInterface.ORIENTATION_ROTATE_270) {
                            rotation = 270;
                        }
                        original = BitmapFactory.decodeFile(file.getAbsolutePath());
                        original = rotateImage(original, rotation);
                        final Bitmap saveToGallery = original.copy(original.getConfig(), true);
                        new Thread() {
                            public void run() {
                                try {
                                    insertPhotoIntoGallery(saveToGallery);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                saveToGallery.recycle();
                            }
                        }.start();
                        //Clean file
                        file.delete();
                    }
                    if (original != null) {
                        Bitmap bitmap = scaleToFill(original, _width, _height);
                        //Camera handles the Bitmap itself
                        if (requestCode != CAMERA_CAPTURE && original != bitmap) {
                            original.recycle(); //this one may be huge, might as well free it right now
                        }

                        //Ensure folder is created
                        new File(FileUtility.getFullPath(_fileName, _location)).getParentFile().mkdirs();

                        if (bitmap != null) {
                            //Save thumbnail if it was asked
                            if(_thumbnailScale > 0) {
                                Bitmap bitmapThumbnail = scaleToFill(bitmap, (int) (_width * _thumbnailScale), (int) (_height * _thumbnailScale));
                                if (bitmapThumbnail != null) {
                                    saveBitmap(bitmapThumbnail, FileUtility.getFullPath(_fileName + "-thumbnail.png", _location));
                                    if (bitmapThumbnail != bitmap) {
                                        bitmapThumbnail.recycle();
                                    }
                                }
                            }
                            //Save main image
                            saveBitmap(bitmap, FileUtility.getFullPath(_fileName + ".png", _location));
                            bitmap.recycle();
                        }
                        NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                            public void run() {
                                notifyImagePickedWrap(_fileName, _location.getValue(), _identifier);
                            }
                        });
                    }
                } else {
                /*
				  Store the picture in a specific folder to prevent the apps crashing when the cropped picture
				  is too big and to avoid black borders in the cropped picture when it is too small
				 */
                    Intent cropIntent = new Intent("com.android.camera.action.CROP");
                    if (requestCode == PICTURE_GALLERY)
                        cropIntent.setDataAndType(Uri.parse(data.getDataString()), "image/png");
                    else {
                        insertPhotoIntoGallery(data);
                        cropIntent.setDataAndType(uriOfSavedPhoto, "image/png");
                    }
                    cropIntent.putExtra("aspectX", 1);
                    cropIntent.putExtra("aspectY", 1);
                    Log.i(TAG, "Will save in " + getStorageDirectory() + "/cropped.png");
                    cropIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(new File(getStorageDirectory(), "cropped.png")));
                    isPending = true;
                    NativeUtility.getMainActivity().startActivityForResult(cropIntent, CROP);
                }
            } catch (IOException e) {
                Log.d(TAG, "IOException while handling Intent result");
                e.printStackTrace();
            }
            return true;
        } else {
            Log.d(TAG, "not image picker activity");
        }
        return false;
    }

    private Bitmap rotateImage(Bitmap image, int rotationAngle)
    {
        if(rotationAngle!=0)
        {
            Matrix matrix = new Matrix();
            matrix.postRotate(rotationAngle);
            image = Bitmap.createBitmap(image, 0, 0, image.getWidth(), image.getHeight(), matrix, true);
        }
        return image;
    }

    private static void saveBitmap(Bitmap bitmap, String path) throws IOException {
        FileOutputStream stream = new FileOutputStream(path);
        bitmap.compress(CompressFormat.PNG, 100, stream);
        stream.close();
    }

    @SuppressWarnings("WeakerAccess")
    public static boolean isCameraAvailable()
    {
    	//Note : support front and back camera since API level9+. We don't use PackageManager.FEATURE_CAMERA_ANY because it is since
    	//API level17+
    	return NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT) ||
    			NativeUtility.getMainActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }

    private enum PICK_OPTION {
        CAMERA(0),
        PHOTO_LIBRARY(1),
        FILE_LIBRARY(2);

        private final int value;

        PICK_OPTION(final int newValue) {
            value = newValue;
        }

        public int getValue() { return value; }
    }
    
    @SuppressWarnings("unused")
    public static boolean pickImageFrom(String saveName, int location, int pickOption, int width, int height, String identifier, float thumbnailScale, boolean rescale)
    {
    	ImagePicker.getInstance(); //ensure the instance is created
    	_fileName = saveName;
        _location = FileUtility.FileLocation.valueOf(location);
    	_width = width;
    	_height = height;
    	_identifier = identifier;
        _thumbnailScale = thumbnailScale;
        _rescale = rescale;
    	boolean error = false;
    	if(pickOption == PICK_OPTION.CAMERA.getValue())
    	{
    		try
    		{
	       		Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                if(!rescale) {
                    File file = new File(getStorageDirectory(), saveName + ".jpg");
                    Uri outputFileUri = Uri.fromFile(file);
                    intent.putExtra(MediaStore.EXTRA_OUTPUT, outputFileUri);
                }
                isPending = true;
		        NativeUtility.getMainActivity().startActivityForResult(intent, CAMERA_CAPTURE);
    		}
    		catch(ActivityNotFoundException e)
    		{
    	    	Log.d(TAG, "intent for image capture not found : " + e.getMessage());
    	    	error = true;
    		} catch (IOException e) {
                e.printStackTrace();
            }
        }
    	else
    	{
    		try
    		{
                Intent intent;
                if(pickOption == PICK_OPTION.PHOTO_LIBRARY.getValue())
                {
                    intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                }
                else
                {
                    intent = new Intent(Intent.ACTION_GET_CONTENT ,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                    intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
                }
                intent.setType("image/*");
                isPending = true;
                NativeUtility.getMainActivity().startActivityForResult(intent, PICTURE_GALLERY);
    		}
    		catch(ActivityNotFoundException e)
    		{
                if(pickOption == 1) Log.d(TAG, "intent for image pick from Galery not found : " + e.getMessage());
                else Log.d(TAG, "intent for image pick from File library not found : " + e.getMessage());

    	    	error = true;
    		}
    	}
    	return error;
    }
    
    private static String getStorageDirectory() throws IOException {
    	if((Environment.getExternalStorageState().equals(Environment.MEDIA_BAD_REMOVAL) ||
        		Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED_READ_ONLY) ||
        		Environment.getExternalStorageState().equals(Environment.MEDIA_NOFS) ||
        		Environment.getExternalStorageState().equals(Environment.MEDIA_REMOVED) ||
        		Environment.getExternalStorageState().equals(Environment.MEDIA_UNMOUNTABLE) ||
        		Environment.getExternalStorageState().equals(Environment.MEDIA_UNMOUNTED)))
        {
            throw new IOException("External storage is not accessible");
        }
        /*
          Use external cache dir to save the cropped picture because we cannot use
          the internal folder to save temporary picture, as it must be accessed by other apps.
          Ensure the directory is created
         */
        File tmpDirectoryPath = new File(Environment.getExternalStorageDirectory() + "/tmp");
        if(!tmpDirectoryPath.mkdirs() && !tmpDirectoryPath.isDirectory())
        {
            throw new IOException("Cannot create temporary folder for ImagePicker");
        }
        //noinspection ConstantConditions
        return tmpDirectoryPath.getAbsolutePath();
    }
    
    // Scale and keep aspect ratio 
    static private Bitmap scaleToFill(Bitmap b, int width, int height) {
        float factorH = height / (float) b.getHeight();
        float factorW = width / (float) b.getWidth();
        float factorToUse = (factorH > factorW) ? factorW : factorH;
        if(factorToUse >= 1) {
            return b;
        }
        Bitmap result = null;
        try {
            result = Bitmap.createScaledBitmap(b, (int) (b.getWidth() * factorToUse), (int) (b.getHeight() * factorToUse), true);
        }
        catch (OutOfMemoryError e) {
            Log.e(TAG, "OutOfMemoryError when trying to scale a bitmap with size " + b.getWidth() + "x" + b.getHeight());
            Toast.makeText(NativeUtility.getMainActivity(), "L'image sélectionnée est trop grande, merci de réessayer avec une image plus petite", Toast.LENGTH_LONG).show();
        }
        return result;
    }

    private void insertPhotoIntoGallery(Intent data) throws IOException {
        insertPhotoIntoGallery((Bitmap) data.getParcelableExtra("data"));
    }
    
    private void insertPhotoIntoGallery(Bitmap image) throws IOException {
        File fi = new File(getStorageDirectory(), "photo.png");
        fi.setReadable(true, false);
		FileOutputStream stream;
		try {
			stream = new FileOutputStream(fi);
			/* Write bitmap to file using JPEG or PNG and 80% quality hint for JPEG. */
			image.compress(CompressFormat.PNG, 100, stream);
			stream.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
        try {
            ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
            String filePath = fi.getAbsolutePath();
            String uri = "";
            int i = 0;
            while((uri == null || uri.isEmpty()) && i<10)
            {
                try
                {
                    i++;
                    uri = android.provider.MediaStore.Images.Media.insertImage(cr, filePath, null, null);
                }
                catch(NullPointerException e2)
                {
                    Log.i(TAG, "create thumbnail failed : insertImage throw nullPointerException, bitmap decode returned null for file " + filePath);
                    e2.printStackTrace();
                }

            }
            if(uri != null && !uri.isEmpty())
            {
                uriOfSavedPhoto = Uri.parse(uri);
            }
            if (!fi.delete()) {
                Log.i(TAG, "Failed to delete " + fi);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
    
    private native static void notifyImagePickedWrap(String name, int location, String identifier);
    private native static void notifyImagePickCancelled();
}
