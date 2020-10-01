package com.fennex.modules;

import android.annotation.SuppressLint;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Source: https://gist.github.com/samkirton/0242ba81d7ca00b475b9
 * There are a few modifications:
 * - make linter happy
 * - it doesn't need a ContentResolver parameter
 * - save in DCIM/Camera folder instead of default "Pictures" folder
 */
class CapturePhotoUtils {
    @SuppressWarnings({"WeakerAccess", "UnusedReturnValue"})
    public static String insertImage(String imagePath, String name, String description) throws FileNotFoundException {
        // Check if file exists with a FileInputStream
        FileInputStream stream = new FileInputStream(imagePath);
        //noinspection TryFinallyCanBeTryWithResources
        try {
            Bitmap bm = BitmapFactory.decodeFile(imagePath);
            String ret = insertImage(bm, name, description);
            bm.recycle();
            return ret;
        } finally {
            try {
                stream.close();
            } catch (IOException ignored) {
            }
        }
    }

    private static final String CAMERA_IMAGE_BUCKET_NAME = "Camera";
    private static final String CAMERA_IMAGE_FULL_PATH = Environment.getExternalStorageDirectory().toString() + File.separator + Environment.DIRECTORY_DCIM + File.separator + CAMERA_IMAGE_BUCKET_NAME;
    private static final String CAMERA_IMAGE_BUCKET_ID =
            getBucketId(CAMERA_IMAGE_FULL_PATH);

    private static String getBucketId(@SuppressWarnings("SameParameterValue") String path) {
        return String.valueOf(path.toLowerCase().hashCode());
    }
    /**
     * A copy of the Android internals  insertImage method, this method populates the
     * meta data with DATE_ADDED and DATE_TAKEN. This fixes a common problem where media
     * that is inserted manually gets saved at the end of the gallery (because date is not populated).
     * @see Images.Media#insertImage(ContentResolver, Bitmap, String, String)
     */
    @SuppressLint("InlinedApi")
    @SuppressWarnings("WeakerAccess")
    public static String insertImage(Bitmap source,
                                     String title,
                                     String description) {
        ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
        ContentValues values = new ContentValues();
        values.put(Images.Media.TITLE, title);
        values.put(Images.Media.DISPLAY_NAME, title);
        values.put(Images.Media.DESCRIPTION, description);
        values.put(Images.Media.MIME_TYPE, "image/jpeg");
        // Add the date meta data to ensure the image is added at the front of the gallery
        values.put(Images.Media.DATE_ADDED, System.currentTimeMillis() / 1000);
        values.put(Images.Media.DATE_TAKEN, System.currentTimeMillis());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            values.put(MediaStore.MediaColumns.RELATIVE_PATH, Environment.DIRECTORY_DCIM + File.separator + CAMERA_IMAGE_BUCKET_NAME);
        }
        else {
            //Legacy incantations to save in /DCIM/Camera
            values.put(Images.Media.BUCKET_DISPLAY_NAME, CAMERA_IMAGE_BUCKET_NAME);
            values.put(Images.Media.BUCKET_ID, CAMERA_IMAGE_BUCKET_ID);
            @SuppressLint("SimpleDateFormat") SimpleDateFormat sdf = new SimpleDateFormat("dd-MM-yyyy_hh-mm-ss");
            values.put(Images.Media.DATA, CAMERA_IMAGE_FULL_PATH + File.separator + sdf.format(new Date()) + ".jpg");
        }

        Uri url = null;
        String stringUrl = null;    /* value to be returned */

        try {
            url = cr.insert(Images.Media.EXTERNAL_CONTENT_URI, values);

            if (source != null && url != null) {
                try (OutputStream imageOut = cr.openOutputStream(url)) {
                    source.compress(Bitmap.CompressFormat.JPEG, 50, imageOut);
                }

                long id = ContentUris.parseId(url);
                // Wait until MINI_KIND thumbnail is generated.
                Bitmap miniThumb = Images.Thumbnails.getThumbnail(cr, id, Images.Thumbnails.MINI_KIND, null);
                // This is for backward compatibility.
                storeThumbnail(cr, miniThumb, id, 50F, 50F,Images.Thumbnails.MICRO_KIND);
            } else if(url != null) {
                cr.delete(url, null, null);
                url = null;
            }
        } catch (Exception e) {
            if (url != null) {
                cr.delete(url, null, null);
                url = null;
            }
        }

        if (url != null) {
            stringUrl = url.toString();
        }

        return stringUrl;
    }

    /**
     * A copy of the Android internals StoreThumbnail method, it used with the insertImage to
     * populate the android.provider.MediaStore.Images.Media#insertImage with all the correct
     * meta data. The StoreThumbnail method is private so it must be duplicated here.
     * @see android.provider.MediaStore.Images.Media (StoreThumbnail private method)
     */
    @SuppressWarnings({"SameParameterValue", "UnusedReturnValue"})
    private static Bitmap storeThumbnail(
            ContentResolver cr,
            Bitmap source,
            long id,
            float width,
            float height,
            int kind) {

        // create the matrix to scale it
        Matrix matrix = new Matrix();

        float scaleX = width / source.getWidth();
        float scaleY = height / source.getHeight();

        matrix.setScale(scaleX, scaleY);

        Bitmap thumb = Bitmap.createBitmap(source, 0, 0,
                source.getWidth(),
                source.getHeight(), matrix,
                true
        );

        ContentValues values = new ContentValues(4);
        values.put(Images.Thumbnails.KIND,kind);
        values.put(Images.Thumbnails.IMAGE_ID,(int)id);
        values.put(Images.Thumbnails.HEIGHT,thumb.getHeight());
        values.put(Images.Thumbnails.WIDTH,thumb.getWidth());

        Uri url = cr.insert(Images.Thumbnails.EXTERNAL_CONTENT_URI, values);
        if(url != null) {
            try {
                OutputStream thumbOut = cr.openOutputStream(url);
                thumb.compress(Bitmap.CompressFormat.JPEG, 100, thumbOut);
                assert thumbOut != null;
                thumbOut.close();
                return thumb;
            } catch (IOException ignored) {
            }
        }
        return null;
    }
}
