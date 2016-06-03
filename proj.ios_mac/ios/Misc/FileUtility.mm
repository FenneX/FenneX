//
//  FileUtility.m
//  ClassIt
//
//  Created by François Dupayrat on 16/06/15.
//
//
#import "NSFileManager+ApplicationSupport.h"

#include "FileUtility.h"

bool lockFile(std::string filename)
{
    return false;
}

std::string getLockedFileContents(std::string filename)
{
    return "";
}

bool writeLockedFile(std::string filename, std::string content)
{
    return false;
}

void unlockFile(std::string filename)
{
}

std::vector<std::string> getFilesInFolder(std::string folderPath)
{
    std::vector<std::string> newVector;
    return newVector;
}

void deleteFile(std::string filePath)
{
    NSString* path = [[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:[NSString stringWithFormat:@"%s", filePath.c_str()]];
    [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
}

bool moveFileToLocalDirectory(std::string path)
{
    return true;
}

bool pickFile()
{
    return true;
}
