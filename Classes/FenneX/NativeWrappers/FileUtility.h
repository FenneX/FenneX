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
#include "FenneX.h"

/*Locking a file is useful on Android to lock a file to be sure other apps do not use it at the same time.
 Warning: it is NOT a way to lock file from other process in the same app.
 Since there is no way to have a file shared between apps on iOS, there is no iOS implementation, just placeholders.
*/

//Return true if the file is properly locked, otherwise false. iOS will always return false
bool lockFile(std::string filename);

//Return the file contents, which must be locked otherwise. If it's not locked or on iOS, it will return an empty reponse
std::string getLockedFileContents(std::string filename);

//Write on a locked file and return true if it's successful. If it's not locked or on iOS, it will return false
bool writeLockedFile(std::string filename, std::string content);

//Unlock a previously locked file
void unlockFile(std::string filename);

// Return all files from a folder in Android. Doesn't do anything on iOS, just return an empty vector.
std::vector<std::string> getFilesInFolder(std::string folderPath);

// Delete the file. Need a complete path. Not quite tested on iOS
void deleteFile(std::string filename);

// Move a file from an absolute path to the local Directory (it will remove the original)
// If the destination already exists, it will not be copied again, but the original will still be removed. It is considered a success.
// Return true if it succeed and false otherwise (if the file doesn't exist for exemple)
// Empty implementation in iOS because there is no global shared disk space
bool moveFileToLocalDirectory(std::string path);

/**
 * launch a pick file activity on android. It can be empty
 * This doesn't launch anything on iOS since ios is not capable of that thing. (you will not receive a "FilePicked" event)
 **/
bool pickFile();

static inline void notifyFilePicked(std::string fullPath)
{
    DelayedDispatcher::eventAfterDelay("FilePicked", DcreateP(Screate(fullPath), Screate("Path"), NULL), 0.01);
}

#endif
