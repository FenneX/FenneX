//
//  FileUtility.m
//  ClassIt
//
//  Created by François Dupayrat on 16/06/15.
//
//
#import "NSFileManager+ApplicationSupport.h"
#include "FileUtility.h"

NS_FENNEX_BEGIN

std::string getLocalPath(const std::string& name)
{
    return std::string(getenv("HOME"))+"/Documents/"+name;
}

std::string getPublicPath(const std::string& name)
{
    return std::string(getenv("HOME"))+"/Documents/"+name;
}

std::string getApplicationSupportPath(const std::string& name)
{
    return [[[NSFileManager defaultManager] applicationSupportDirectory] UTF8String] + std::string("/") + name;
}
std::string getResourcesPath(const std::string& file)
{
    const std::vector<std::string> searchPaths = FileUtils::getInstance()->getSearchPaths();
    for(std::string path : searchPaths)
    {
        //PreconfiguredPath will probably be in SearchPath, but ignore it for resources.
        if(path.find(getPreconfiguredPath("")) == std::string::npos)
        {
            NSString* fullPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                                 ofType:nil
                                                            inDirectory:[NSString stringWithUTF8String:path.c_str()]];
            if(fullPath)
            {
                return [fullPath UTF8String];
            }
        }
    }
    return "";
}

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
    NSArray *directoryContent = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithFormat:@"%s", folderPath.c_str()] error:NULL];
    for (int count = 0; count < (int)[directoryContent count]; count++)
    {
        newVector.push_back([[directoryContent objectAtIndex:count] UTF8String]);
    }
    return newVector;
}

bool moveFileToLocalDirectory(std::string path)
{
    return true;
}

bool moveFile(std::string path, std::string destinationFolder)
{
    return true;
}

bool pickFile()
{
    return true;
}

NS_FENNEX_END
