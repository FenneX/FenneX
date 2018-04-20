package com.fennex.modules;

import android.content.ActivityNotFoundException;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Toast;

import org.videolan.libvlc.util.Extensions;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Locale;
import java.util.Stack;
import java.util.StringTokenizer;

import static android.app.Activity.RESULT_CANCELED;

public class VideoPicker implements ActivityResultResponder {

    private static final String TAG = "VideoPicker";
    private static final int VIDEO_GALLERY = 40;
    private static final int CAMERA_CAPTURE = 41;
    private static volatile VideoPicker instance = null;

    private static boolean isPending = false;
    private static String _fileName;
    private static FileUtility.FileLocation _location;
    
    private VideoPicker() { }

    public static VideoPicker getInstance() 
    {
        if (instance == null) 
        {
            isPending = false;
            synchronized (ImagePicker .class)
            {
                if (instance == null) 
                {
                	instance = new VideoPicker();
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

    public native static void notifyVideoPickedWrap(String name, int location);
    public native static void notifyVideoFound(String path);
    public native static void notifyVideoName(String path, String name);
    public native static void notifyGetAllVideosFinished();
    public native static void notifyVideoPickCancelled();

    
    public static boolean pickVideoFromLibrary(String saveName, int location)
    {
    	VideoPicker.getInstance(); //ensure the instance is created
        _fileName = saveName;
        _location = FileUtility.FileLocation.valueOf(location);
    	boolean error = false;
		try
		{
			Intent intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
			intent.setType("video/*");
            isPending = true;
			NativeUtility.getMainActivity().startActivityForResult(intent, VIDEO_GALLERY);
		}
		catch(ActivityNotFoundException e)
		{
	    	Log.d(TAG, "intent for image pick from library not found : " + e.getMessage());
	    	error = true;
		}
    	return error;
    }

    public static boolean pickVideoFromCamera(String saveName, int location)
    {
        VideoPicker.getInstance(); //ensure the instance is created
        _fileName = saveName;
        _location = FileUtility.FileLocation.valueOf(location);
        boolean error = false;
        try
        {
            Intent intent = new Intent(MediaStore.ACTION_VIDEO_CAPTURE);
            intent.putExtra(MediaStore.EXTRA_OUTPUT, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            isPending = true;
            NativeUtility.getMainActivity().startActivityForResult(intent, CAMERA_CAPTURE);
        }
        catch(ActivityNotFoundException e)
        {
            Log.d(TAG, "intent for image pick from Camera not found : " + e.getMessage());
            error = true;
        }
        return error;
    }
    
    public static void getAllVideos()
    {
    	//This code is heavily copied from VLC org.videolan.vlc.MediaLibrary
    	Thread thread = new Thread()
        {
            @Override
            public void run() {

            	final Stack<File> directories = new Stack<File>();
            	final HashSet<String> directoriesScanned = new HashSet<String>();

                String storageDirs[] = getStorageDirectories();
                for (String dir: storageDirs)
                {
                    File f = new File(dir);
                    if (f.exists())
                    	directories.add(f);
                }

                MediaItemFilter mediaFileFilter = new MediaItemFilter();

                ArrayList<File> mediaToScan = new ArrayList<File>();
                
                // Count total files, and stack them
                while (!directories.isEmpty()) {
                    File dir = directories.pop();
                    String dirPath = dir.getAbsolutePath();
                    File[] f = null;

                    // Skip some system folders
                    if (dirPath.startsWith("/proc/") || dirPath.startsWith("/sys/") || dirPath.startsWith("/dev/"))
                        continue;

                    // Do not scan again if same canonical path
                    try {
                        dirPath = dir.getCanonicalPath();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (directoriesScanned.contains(dirPath))
                        continue;
                    else
                        directoriesScanned.add(dirPath);

                    // Do no scan media in .nomedia folders
                    if (new File(dirPath + "/.nomedia").exists()) {
                        continue;
                    }

                    // Filter the extensions and the folders
                    try {
                        if ((f = dir.listFiles(mediaFileFilter)) != null) {
                            for (File file : f) {
                                if (file.isFile()) {
                                    mediaToScan.add(file);
                                } else if (file.isDirectory()) {
                                    directories.push(file);
                                }
                            }
                        }
                    } 
                    catch (Exception e)
                    {
                        // listFiles can fail in OutOfMemoryError, go to the next folder
                    }
                }

                // Process the stacked items
                for (File file : mediaToScan) {
                	final String path = file.getPath();
                	if(path.length() > 0)
                	{
                		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
                		{
                			public void run()
	            			{
	                          	try
	                          	{
		        	    	        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
		        	    	        Log.i(TAG, "Setting data source to : " + path);
		        	    	        retriever.setDataSource(path); //SetDataSource can fail. If it does, the format is unsupported
		                          	notifyVideoFound(path); 
		        	    	        Log.i(TAG, "Setting data source success of : " + path);
		        	    	        String title = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);
		        	    	        String videoName = title != null ? title : path.lastIndexOf('/') >= 0 ? path.substring(path.lastIndexOf('/')+1) : path;
		        	    	        Log.i(TAG, "video found, path : " + path + ", name : " + videoName);
		        	    	        notifyVideoName(path, videoName);
		        	    	        String duration = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
		        	    	        if(duration != null && duration.length() > 0 && Integer.parseInt(duration) > 0)
		        	    	        {
		        	    	        	VideoPlayer.notifyVideoDurationAvailable(path, (float)Integer.parseInt(duration) / 1000.0f);
		        	    	        }
	                          	}
	                          	catch(Exception e)
	                          	{
	                          		Log.e(TAG, "Can't get metadata for path : " + path);
	                          	}
            		  		}
            			});
                	}
                }

        		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
        		{
        			public void run()
        			{
        				notifyGetAllVideosFinished();
        			}
        		});
            }
        };
        thread.start();
    }
    
    static String[] getStorageDirectories()
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
        } catch (IOException ignored) {}
        finally {
            if (bufReader != null) {
                try {
                    bufReader.close();
                }
                catch (IOException ignored) {}
            }
        }
        return dirs;
    }
    
	@Override
	public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        isPending = false;
        if(resultCode == RESULT_CANCELED)
        {
            notifyVideoPickCancelled();
        }
        else if (requestCode == VIDEO_GALLERY || requestCode == CAMERA_CAPTURE)
		{
            Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data.getExtras());
			Log.d(TAG, "intent data: " + data.getDataString());
	        Uri videoUri = data.getData();

            // Copy file into the app
            String path = getFullPathFromURI(videoUri);
            _fileName += path.substring(path.lastIndexOf("."));
            String destinationPath = FileUtility.getFullPath(_fileName, _location);
            File destinationFile = new File(destinationPath);
            destinationFile.getParentFile().mkdirs();

            Log.d(TAG, "video path : " + path + ", new filename : " + _fileName);
            if(!destinationFile.exists())
            {
                InputStream in = null;
                OutputStream out = null;
                try
                {
                    in = new FileInputStream(path);
                    out = new FileOutputStream(destinationPath);
                    byte[] buffer = new byte[1024];
                    int read;
                    while ((read = in.read(buffer)) != -1) {
                        out.write(buffer, 0, read);
                    }
                    in.close();
                    in = null;

                    // write the output file
                    out.flush();
                    out.close();
                    out = null;
                }
                catch(Exception e)
                {
                    Log.i("FileUtility", "Could not move file : " + _fileName);
                    e.printStackTrace();
                    return false;
                }
            }
            final String name = _fileName; // In case we relaunch with a new filename before the thread run
            final FileUtility.FileLocation location = _location;
    		NativeUtility.getMainActivity().runOnGLThread(new Runnable() 
    		{
    			public void run()
    			{
    				notifyVideoPickedWrap(name, location.getValue());
    			}
    		});
			return true;
		}
		else
		{
			Log.d(TAG, "not video picker activity");
		}
        return false;
	}

	private String getFullPathFromURI(Uri contentUri)
	{
	    String path = null;
	    try
	    {
			ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
			String[] projection = {MediaStore.MediaColumns.DATA};
			Cursor cur = cr.query(contentUri, projection, null, null, null);
			if(cur != null && cur.moveToFirst())
			{
			   path = cur.getString(0);
			   cur.close();
			}
	    }
	    catch(Exception e)
	    {
	    	e.printStackTrace();
	    	Log.e(TAG, "Error while getting video name, returning null");
	    	path = null;
	    }
	    return path;
	}

    /**
     * Filters all irrelevant files
     */
    static private class MediaItemFilter implements FileFilter {

        @Override
        public boolean accept(File f) {
            boolean accepted = false;
            if (!f.isHidden()) {
                String[] folder_blacklist = {
                        "/alarms",
                        "/notifications",
                        "/ringtones",
                        "/media/alarms",
                        "/media/notifications",
                        "/media/ringtones",
                        "/media/audio/alarms",
                        "/media/audio/notifications",
                        "/media/audio/ringtones",
                        "/Android/data/" };
                HashSet<String> folderBlacklist = new HashSet<String>();
                folderBlacklist.addAll(Arrays.asList(folder_blacklist));
                if (f.isDirectory() && !folderBlacklist.contains(f.getPath().toLowerCase(Locale.ENGLISH))) {
                    accepted = true;
                } else {
                    String fileName = f.getName().toLowerCase(Locale.ENGLISH);
                    int dotIndex = fileName.lastIndexOf(".");
                    if (dotIndex != -1) {
                        String fileExt = fileName.substring(dotIndex);
                        accepted = //Media.AUDIO_EXTENSIONS.contains(fileExt) || //Logiral doesn't need audio files
                                Extensions.VIDEO.contains(fileExt);
                    }
                }
            }
            return accepted;
        }
    }
}
