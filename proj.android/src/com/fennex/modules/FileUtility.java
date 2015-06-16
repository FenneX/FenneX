package com.fennex.modules;

import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileLock;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;

/**
 * Created by Fradow on 10/06/15.
 */
public class FileUtility {
    HashMap<String, FileLock> currentLocks;
    HashMap<String, RandomAccessFile> currentFiles;
    private static volatile FileUtility instance = null;
    private FileUtility() {
        currentLocks = new HashMap<String, FileLock>();
        currentFiles = new HashMap<String, RandomAccessFile>();
    }

    public static FileUtility getInstance()
    {
        if (instance == null)
        {
            synchronized (FileUtility .class)
            {
                if (instance == null)
                {
                    instance = new FileUtility ();
                }
            }
        }
        return instance;
    }
    static boolean lockFile(String filename) {
        boolean result = true;
        try {
            File fileBase = new File(filename);
            RandomAccessFile file = new RandomAccessFile(fileBase, "rwd");
            FileLock lock = file.getChannel().lock();
            getInstance().currentLocks.put(filename, lock);
            getInstance().currentFiles.put(filename, file);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            result = false;
        } catch (IOException e) {
            e.printStackTrace();
            result = false;
        }
        return result;
    }

    static String getLockedFileContents(String filename)
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

    static boolean writeLockedFile(String filename, String content)
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

    static void unlockFile(String filename) {
        FileLock lock = getInstance().currentLocks.get(filename);
        RandomAccessFile file = getInstance().currentFiles.get(filename);
        Log.i("FileUtility", "Lock is: " + (lock.isValid() ? "valid" : "invalid"));
        if(lock != null) {
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
