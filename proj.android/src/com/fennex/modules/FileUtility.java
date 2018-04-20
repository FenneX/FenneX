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
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.channels.FileLock;
import java.nio.channels.OverlappingFileLockException;
import java.util.HashMap;
import java.util.Map;

public class FileUtility implements ActivityResultResponder {
    private static final int FILE_PICK = 50;
    private static final String TAG = "FileUtility";
    private HashMap<String, FileLock> currentLocks;
    private HashMap<String, RandomAccessFile> currentFiles;
    private static volatile FileUtility instance = null;
    private static boolean isPending = false;
    private FileUtility() {
        currentLocks = new HashMap<>();
        currentFiles = new HashMap<>();
    }

    // Match FileUtility.cpp enum values, see that file for details
    @SuppressWarnings("WeakerAccess")
    public enum FileLocation {
        Local(0),
        PreconfiguredPath(1),
        Public(2),
        ApplicationSupport(3),
        Absolute(4),
        Resources(5),
        Unknown(-1);

        private int value;
        private static Map map = new HashMap<>();

        FileLocation(int value) {
            this.value = value;
        }

        static {
            for (FileLocation location : FileLocation.values()) {
                //noinspection unchecked
                map.put(location.value, location);
            }
        }

        public static FileLocation valueOf(int location) {
            return (FileLocation) map.get(location);
        }

        public int getValue() {
            return value;
        }
    }

    public static FileUtility getInstance()
    {
        if (instance == null)
        {
            synchronized (FileUtility .class)
            {
                if (instance == null)
                {
                    instance = new FileUtility();
                    if(NativeUtility.getMainActivity() != null) NativeUtility.getMainActivity().addResponder(instance);
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

    public static String getLocalPath()
    {
    	return NativeUtility.getMainActivity().getFilesDir().getPath();
    }

    public static String getLocalPath(Context context)
    {
        if(context == null) return getLocalPath();
        return context.getFilesDir().getPath();
    }

    public static String getPublicPath()
    {
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }

    public static String getFullPath(String filename, FileLocation location)
    {
        return getFullPath(filename, location.getValue());
    }

    public native static String getFullPath(String filename, int location);

    public native static String findFullPath(String filename);

    public static boolean lockFile(String filename) {
        synchronized (FileUtility.class) {
            try {
                //Create parent directories recursively
                if (filename.lastIndexOf(File.separator) != -1) {
                    String directoryPath = filename.substring(0, filename.lastIndexOf(File.separator));
                    File directory = new File(directoryPath);
                    if(!directory.exists() && !directory.mkdirs()) {
                        Log.e(TAG, "Error creating directory " + directoryPath + ", cannot lock it");
                        return false;
                    }
                }
                File fileBase = new File(filename);
                RandomAccessFile file = new RandomAccessFile(fileBase, "rwd");
                FileLock lock = file.getChannel().lock();
                getInstance().currentLocks.put(filename, lock);
                getInstance().currentFiles.put(filename, file);
            } catch (OverlappingFileLockException | FileNotFoundException e) {
                e.printStackTrace();
                Log.e(TAG, "List of currently locked files:");
                for (HashMap.Entry<String, FileLock> entry : getInstance().currentLocks.entrySet()) {
                    Log.e(TAG, "    locked file " + entry.getKey());
                }
                return false;
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }

    public static String getLockedFileContents(String filename)
    {
        RandomAccessFile file = getInstance().currentFiles.get(filename);
        if(file == null) return null;
        String content = null;
        try {
            file.seek(0);
            byte[] data = new byte[(int) file.length()];
            file.readFully(data);
            content = new String(data, "UTF-8");
        } catch (IOException e) {
            e.printStackTrace();
        }
        return content;
    }

    public static boolean writeLockedFile(String filename, String content)
    {
        RandomAccessFile file = getInstance().currentFiles.get(filename);
        if(file == null) return false;
        boolean result = true;
        try {
            //Rewind and erase data
            file.seek(0);
            file.setLength(0);
            //Overwrite with new content
            file.write(content.getBytes("UTF-8"));
        } catch (IOException e) {
            result = false;
            e.printStackTrace();
        }
        return result;
    }

    public static void unlockFile(String filename) {
        synchronized (FileUtility.class) {
            FileLock lock = getInstance().currentLocks.get(filename);
            RandomAccessFile file = getInstance().currentFiles.get(filename);
            if (lock != null && file != null) {
                if (!lock.isValid()) Log.i("FileUtility", "Lock is invalid for file : " + filename);
                try {
                    lock.release();
                    file.close();
                    getInstance().currentLocks.remove(filename);
                    getInstance().currentFiles.remove(filename);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
    
    public static void deleteRecursive(File fileOrDirectory) {
        if (fileOrDirectory.isDirectory())
            for (File child : fileOrDirectory.listFiles())
                deleteRecursive(child);

        //noinspection ResultOfMethodCallIgnored
        fileOrDirectory.delete();
    }

    @SuppressWarnings("unused")
    public static String[] getFilesInFolder(String folderPath)
    {
        File dir = new File(folderPath);
        String[] files = new String[]{};
        if(dir.exists() && dir.isDirectory())
        {
            files = dir.list();
        }
        return files;
    }

    @SuppressWarnings("unused")
    public static boolean moveFileToLocalDirectory(String path)
    {
        String filename = path.substring(path.lastIndexOf("/"));
        File destinationFile = new File(FileUtility.getLocalPath() + java.io.File.separator + filename);

        if(!destinationFile.exists())
        {
            InputStream in;
            OutputStream out;
            try
            {
                in = new FileInputStream(path);
                out = new FileOutputStream(FileUtility.getLocalPath() + java.io.File.separator + filename);
                byte[] buffer = new byte[1024];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }
                in.close();

                // write the output file
                out.flush();
                out.close();

                // delete the original file
                //noinspection ResultOfMethodCallIgnored
                new File(path).delete();
            }
            catch(Exception e)
            {
                Log.i("FileUtility", "Could not move file : " + filename);
                e.printStackTrace();
                return false;
            }
        }
        else
        {
            Log.i("FileUtility", "didn't copied" + filename + ", already exist");
            try
            {
                // delete the original file
                //noinspection ResultOfMethodCallIgnored
                new File(path).delete();
            }
            catch(Exception e)
            {
                Log.i("FileUtility", "Couldn't delete file :"+ path);
                e.printStackTrace();
            }
        }
        return true;
    }

    @SuppressWarnings("unused")
    public static boolean moveFile(String path, String destinationFolder)
    {
        String filename = path.substring(path.lastIndexOf("/"));
        String destinationFilename = destinationFolder + java.io.File.separator + filename;
        File destinationFile = new File(destinationFilename);
        File destinationFolderFile = new File(destinationFolder);

        if(!destinationFolderFile.exists() && !destinationFolderFile.mkdirs()) {
            Log.e(TAG, "Error creating directory " + destinationFolder + ", cannot lock move file " + path + " to this directory");
            return false;
        }

        if(!destinationFile.exists())
        {
            InputStream in;
            OutputStream out;
            try
            {
                in = new FileInputStream(path);
                out = new FileOutputStream(destinationFilename);
                byte[] buffer = new byte[1024];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }
                in.close();

                // write the output file
                out.flush();
                out.close();

                // delete the original file
                //noinspection ResultOfMethodCallIgnored
                new File(path).delete();
            }
            catch(Exception e)
            {
                Log.i("FileUtility", "Could not move file : " + filename);
                e.printStackTrace();
                return false;
            }
        }
        else
        {
            Log.i("FileUtility", "didn't copied" + filename + ", already exist");
            try
            {
                // delete the original file
                //noinspection ResultOfMethodCallIgnored
                new File(path).delete();
            }
            catch(Exception e)
            {
                Log.i("FileUtility", "Couldn't delete file :"+ path);
                e.printStackTrace();
            }
        }
        return true;
    }

    @SuppressWarnings("unused")
    public static boolean pickFile()
    {
        FileUtility.getInstance(); //ensure the instance is created
        boolean error = false;
        try
        {
            Intent intent;
            intent = new Intent(Intent.ACTION_GET_CONTENT, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
            intent.setType("file/*");
            isPending = true;
            NativeUtility.getMainActivity().startActivityForResult(intent, FILE_PICK);
        }
        catch(ActivityNotFoundException e)
        {
            Log.d(TAG, "intent for image pick from File library not found : " + e.getMessage());
            error = true;
        }
        return error;
    }

    @Override
    public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        isPending = false;
        if (requestCode == FILE_PICK && NativeUtility.getMainActivity() != null) {
            Log.d(TAG, "intent data: " + data.getDataString());
            final Uri fileUri = data.getData();
            NativeUtility.getMainActivity().runOnGLThread(new Runnable()
            {
                public void run()
                {
                    notifyFilePicked(fileUri.getPath());
                }
            });
            return true;
        }
        return false;
    }

    @SuppressWarnings("JniMissingFunction")
    public native static void notifyFilePicked(String path);
}
