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

/*
 Warning: currently not implemented on iOS, will return error NotImplemented all the time and throw an assertion
 Require OkHttp on Android, add dependency in build.gradle: implementation 'com.squareup.okhttp3:okhttp:3.12.1'
 Note: OkHttp 3.12.x support Android 4, and is supported until December 2020. OkHttp 3.13+ is Android 5+ only.
 
 Download a file to a specific location. The url must be publicly accessible, and the fullPath writable
 url: full URL, containing the protocol (http/https)
 fullPath: absolute path, must be writable
 onSuccess: callback when the file is fully downloaded
 onError: callback when there is an error. Include http error code (or -1 if it's not a http error) and error response
 onProgressUpdate: callback with progress update, only called every 2%, with number of bytes downloaded and total size in bytes
 onSizeReceived: callback when we know the total size of the file, in bytes, as indicated by Content-Length header
 */
void downloadFile(std::string url,
                  std::string fullPath,
                  std::function<void()> onFileDownloaded,
                  std::function<void(int, const std::string&)> onDownloadFailure,
                  std::function<void(long, long)>onProgressUpdate,
                  std::function<void(long)> onSizeReceived);

NS_FENNEX_END

#endif /* NetworkUtility_h */
