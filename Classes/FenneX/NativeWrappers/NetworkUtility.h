/****************************************************************************
 Copyright (c) 2013-2016 Auticiel SAS
 
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

#ifndef NetworkUtility_h
#define NetworkUtility_h

#include "cocos2d.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN

// Detect if we are connected using OS utility to know that
bool isConnected();

// Will launch settings page for iOS > 8.0 and do nothing before (not possible).
// It will go directly to the wifi settings on android
void openWifiSettings();


typedef enum
{
    NotImplemented = -1,
    UnknownError = 1,
    ServerError = 2,
    NetworkUnavailable = 3,
    CannotWrite = 4,
}DownloadError;

/* Download a file to a specific location. The url must be publicly accessible, and the localPath writable
 url: full URL, containing the protocol (http/https)
 localPath: absolute path, must be writable
 onSuccess: callback when the file is fully downloaded
 onError: callback when there is an error. The parameter indicates the error type
 onProgressUpdate: callback when the progress % change. The parameter is a float between 0 and 1 indicating the completion
 onSizeReceived: callback when we know the total size of the file. The parameter is an int of the total size in bytes
 Warning: currently not implemented on iOS, will return error NotImplemented all the time
 */
void downloadFile(std::string url, std::string localPath, std::function<void()> onSuccess, std::function<void(DownloadError)>onError, std::function<void(float)>onProgressUpdate, std::function<void(int)> onSizeReceived);

NS_FENNEX_END

#endif /* NetworkUtility_h */
