//
//  FileUtility.h
//  ClassIt
//
//  Created by François Dupayrat on 10/06/15.
//
//

#ifndef ClassIt_FileUtility_h
#define ClassIt_FileUtility_h

#include <string>
#include <vector>

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

#endif
