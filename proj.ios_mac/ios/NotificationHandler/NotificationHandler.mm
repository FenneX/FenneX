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
#include "NotificationHandler.h"
#include "NSStringUtility.h"
#import "UserNotifications/UserNotifications.h"

#define NOTIFICATION_ID_PREFIX "FenneXNotification"

NS_FENNEX_BEGIN

void planNotification(long timestamp, const std::string& text, const std::string& title, const std::string& url, int notificationId)
{
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    UNAuthorizationOptions options = UNAuthorizationOptionAlert + UNAuthorizationOptionSound;
    NSString* nsText = getNSString(text);
    NSString* nsUrl = getNSString(url);
    NSString* nsTitle = getNSString(title);
    [center requestAuthorizationWithOptions:options
      completionHandler:^(BOOL granted, NSError * _Nullable error) {
        if (!granted) {
            NSLog(@"Notification Authorization was not granted");
            return;
        }
        UNMutableNotificationContent *content = [UNMutableNotificationContent new];
        content.title = nsTitle;
        content.body = nsText;
        content.sound = [UNNotificationSound defaultSound];
        content.userInfo = @{@"OpenUrl": nsUrl};
        
        NSDate *date = [NSDate dateWithTimeIntervalSince1970:timestamp];
        NSDateComponents *triggerDate = [[NSCalendar currentCalendar]
                                            components:NSCalendarUnitYear +
                                            NSCalendarUnitMonth + NSCalendarUnitDay +
                                            NSCalendarUnitHour + NSCalendarUnitMinute +
                                            NSCalendarUnitSecond fromDate:date];
        UNCalendarNotificationTrigger *trigger = [UNCalendarNotificationTrigger triggerWithDateMatchingComponents:triggerDate
                                                                                                          repeats:NO];
        NSString *identifier = [NSString stringWithFormat:@"%s-%d", NOTIFICATION_ID_PREFIX, notificationId];
        UNNotificationRequest* request = [UNNotificationRequest requestWithIdentifier:identifier
                                                                              content:content
                                                                              trigger:trigger];
        
        [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
          if (error != nil) {
            NSLog(@"Something went wrong: %@",error);
          }
        }];
    }];
    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings * _Nonnull settings) {
      if (settings.authorizationStatus != UNAuthorizationStatusAuthorized) {
          NSLog(@"Notifications were not allowed");
      }
    }];
}

void cancelNotifications(std::vector<int> notificationIds)
{
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    UNAuthorizationOptions options = UNAuthorizationOptionAlert + UNAuthorizationOptionSound;
    NSMutableArray *array = [NSMutableArray new];
    for(int identifier : notificationIds)
    {
        [array addObject:[NSString stringWithFormat:@"%s-%d", NOTIFICATION_ID_PREFIX, identifier]];
    }
    
    [center requestAuthorizationWithOptions:options
      completionHandler:^(BOOL granted, NSError * _Nullable error) {
        if (!granted) {
            NSLog(@"Notification Authorization was not granted");
            return;
        }
        [center removePendingNotificationRequestsWithIdentifiers:array];
    }];
}

NS_FENNEX_END
