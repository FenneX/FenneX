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

#include "NetworkUtility.h"
#import <UIKit/UIKit.h>
#import "Reachability.h"

NS_FENNEX_BEGIN

bool isConnected()
{
    Reachability *reach = [Reachability reachabilityForInternetConnection];
    NetworkStatus netStatus = [reach currentReachabilityStatus];
    return netStatus != NotReachable;
}

void openWifiSettings()
{
    //Only available since iOS8
    //removed condition if (&UIApplicationOpenSettingsURLString != nullptr) as we target iOS 8.0 minimum now
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
}

void downloadFile(std::string url,
                  std::string fullPath,
                  std::function<void()> onFileDownloaded,
                  std::function<void(int, const std::string&)> onDownloadFailure,
                  std::function<void(long, long)>onProgressUpdate,
                  std::function<void(long)> onSizeReceived,
                  std::string authorizationHeader)
{
    CCASSERT(1, "iOS downloadFile is not implemented on iOS, please use FileDownloader, which use Cocos2d-x Downloader");
    onDownloadFailure(-1, "NotImplemented");
}

NS_FENNEX_END
