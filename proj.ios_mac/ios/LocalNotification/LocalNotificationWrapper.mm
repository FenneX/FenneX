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

#include "LocalNotificationWrapper.h"
#import "NSCCConverter.h"
#import <UIKit/UIKit.h>

void scheduleNotification(float timeFromNow, const std::string& alertBody, const std::string& alertAction, const std::string& soundName, CCDictionary* userInfo)
{
    UILocalNotification* notif = [UILocalNotification new];
    notif.fireDate = [NSDate dateWithTimeIntervalSinceNow:timeFromNow];
    notif.alertBody = !alertBody.empty() ? [NSString stringWithCString:alertBody.c_str() encoding:NSUTF8StringEncoding] : nil;
    notif.alertAction = !alertAction.empty() ? [NSString stringWithCString:alertAction.c_str() encoding:NSUTF8StringEncoding] : nil;
    notif.userInfo = [NSCCConverter nsDictionaryFromCCDictionary:userInfo];
    if(!soundName.empty())
    {
        if([[NSBundle mainBundle] pathForResource:[[NSString stringWithCString:soundName.c_str() encoding:NSUTF8StringEncoding]stringByDeletingPathExtension] ofType:@"mp3"] != NULL)
        {
            
            notif.soundName = [NSString stringWithCString:soundName.c_str() encoding:NSUTF8StringEncoding];
        }
        else
        {
            notif.soundName = UILocalNotificationDefaultSoundName;
        }
    }
    [[UIApplication sharedApplication] scheduleLocalNotification:notif];
    [notif release];
}

void cancelAllNotifications()
{
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
}