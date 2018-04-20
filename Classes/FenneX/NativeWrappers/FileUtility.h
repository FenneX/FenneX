/****************************************************************************
 Copyright (c) 2013-2015 Auticiel SAS
 
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

#ifndef ClassIt_FileUtility_h
#define ClassIt_FileUtility_h

#include <string>
#include <vector>
#include "DelayedDispatcher.h"

NS_FENNEX_BEGIN

enum class FileLocation {
    Local = 0,  // A file saved in the app private folder. That's the most commonly used location
    PreconfiguredPath = 1,  // A file in a pre-configured folder. It is probably going to be a commonly used location if you use it
    Public = 2,  // A file saved in the public folder (same as Local for iOS, as iOS doesn't have "public" folder, publicly accessible folder for Android)
    ApplicationSupport = 3,  // A file saved in ApplicationSupport on iOS, same as Local for Android. Prefer Local if there is no legacy constraint
    Absolute = 4,  // Provide an absolute path for the file. Prefer using other options, to handle files being moved gracefully.
    Resources = 5,  // A file that comes with the app binary. Searched last because you probably want to use a modified version if one exists
    Unknown = -1,  // File cannot be found
};

// Return a local (inside the app) writable path.
std::string getLocalPath(const std::string& name);

// Get and set a preconfigured path that can be used as a location. Trailing slash will be automatically added if missing
std::string getPreconfiguredPath(const std::string& name);
void setPreconfiguredPath(std::string path);

// Return a public writable path on Android. On iOS, it is the same as getLocalPath
std::string getPublicPath(const std::string& name);

// On iOS, return the path to ApplicationSupport directory. On Android, it is the same as getLocalPath
std::string getApplicationSupportPath(const std::string& name);

// Return the full path for a resource if it's found.
std::string getResourcesPath(const std::string& file);

// Will try to find the file location. It will try to see if the file exists in all FileLocations, in order they are declared
FileLocation findFileLocation(std::string filename);

// Return the full path for a provided location
std::string getFullPath(std::string filename, FileLocation location);

// Try to find the full path of a filename by searching all locations
// Returns the full path if found, and an empty string otherwise
// Prefer the previous method if you know the location
std::string findFullPath(std::string filename);

// Delete a file at provided location
// Warning: deleteFile used to unlock the file, this is no longer the case: you should unlock a file before deleting it
void deleteFile(std::string filename, FileLocation location);

/* Locking a file is useful on Android to lock a file to be sure other apps do not use it at the same time.
 * Warning: it is NOT a way to lock file from other process in the same app.
 * Since there is no way to have a file shared between apps on iOS, there is no iOS implementation, just placeholders.
*/

// Return true if the file is properly locked, otherwise false. iOS will always return false
bool lockFile(std::string filename);

// Return the file contents, which must be locked otherwise. If it's not locked or on iOS, it will return an empty reponse
std::string getLockedFileContents(std::string filename);

// Write on a locked file and return true if it's successful. If it's not locked or on iOS, it will return false
bool writeLockedFile(std::string filename, std::string content);

// Unlock a previously locked file
void unlockFile(std::string filename);

// Return all files from a folder (using absolute path)
std::vector<std::string> getFilesInFolder(std::string folderPath);

// Move a file from an absolute path to the local Directory (it will remove the original)
// If the destination already exists, it will not be copied again, but the original will still be removed. It is considered a success.
// Return true if it succeed and false otherwise (if the file doesn't exist for exemple)
// Empty implementation in iOS because there is no global shared disk space
bool moveFileToLocalDirectory(std::string path);

// Move a file from an absolute path to a specific destination folder (it will remove the original)
// It will create the missing directories if needed
// if the file already exists in the destination folder, it will not be copied, but the original will still be removed. It is considered a success.
// Return true if it succeed and false otherwise (if the file doesn't exist for exemple)
// Empty implementation in iOS because there is no global shared disk space
bool moveFile(std::string path, std::string destinationFolder);

/**
 * Launch a pick file activity on Android. It can be empty
 * This doesn't launch anything on iOS since ios is not capable of that thing. (you will not receive a "FilePicked" event)
 **/
bool pickFile();

static inline void notifyFilePicked(std::string fullPath)
{
    FenneX::DelayedDispatcher::eventAfterDelay("FilePicked", Value(ValueMap({{"Path", Value(fullPath)}})), 0.01);
}

NS_FENNEX_END

#endif
