/****************************************************************************
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

#include "FileUtility.h"
#include "NativeUtility.h"
#include "StringUtility.h"
#include <sys/stat.h>

NS_FENNEX_BEGIN

static std::string _preconfiguredPath = "";

std::string getPreconfiguredPath(const std::string& name)
{
    return _preconfiguredPath + name;
}

void setPreconfiguredPath(std::string path)
{
    if(!path.empty() && !stringEndsWith(path, "/"))
    { // Non-empty path must end with a slash, so that you can directly append a filename
        path += "/";
    }
    _preconfiguredPath = path;
}

FileLocation findFileLocation(std::string filename)
{
    std::vector<FileLocation> locations = {FileLocation::Local, FileLocation::Public, FileLocation::ApplicationSupport, FileLocation::Absolute, FileLocation::Resources};
    if(_preconfiguredPath.length() > 0)
    { // Only search there if it has been set
        locations.insert(locations.begin() + 1, FileLocation::PreconfiguredPath);
    }
    
    struct stat buffer;
    for(FileLocation location : locations)
    {
        // Do NOT use FileUtils::isFileExist, as it can return true if the file is found in a search path, when testing for Absolute
        if(stat (getFullPath(filename, location).c_str(), &buffer) == 0) return location;
    }
    return FileLocation::Unknown;
}

std::string getFullPath(std::string filename, FileLocation location)
{
    switch(location)
    {
        case FileLocation::Local:
            return getLocalPath(filename);
        case FileLocation::Resources:
            return getResourcesPath(filename);
        case FileLocation::Public:
            return getPublicPath(filename);
        case FileLocation::ApplicationSupport:
            return getApplicationSupportPath(filename);
        case FileLocation::PreconfiguredPath:
            return getPreconfiguredPath(filename);
        case FileLocation::Absolute:
            return filename;
        case FileLocation::Unknown:
            return "";
    }
}

std::string findFullPath(std::string filename)
{
    return getFullPath(filename, findFileLocation(filename));
}

void deleteFile(std::string filename, FileLocation location)
{
    FileUtils::getInstance()->removeFile(getFullPath(filename, location));
}

NS_FENNEX_END
