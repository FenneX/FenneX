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

#ifndef NotificationHandler_h
#define NotificationHandler_h

#include "cocos2d.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN


/**
 * For this notification to work you'll have some setup up to do
 * On Android :
 * In your AndroidManifest.xml, add the following lines in your <application> node :
 <receiver android:name="com.fennex.modules.Notifications.NotificationPublisher" />
 <receiver
     android:name="com.fennex.modules.Notifications.NotificationRebootReceiver"
     android:enabled="true"
     android:exported="true"
     android:label="NotificationRebootReceiver">
     <intent-filter>
         <action android:name="android.intent.action.BOOT_COMPLETED" />
     </intent-filter>
 </receiver>
 *
 * In your build.gradle file, add the following lines in your dependencies :
 implementation "androidx.room:room-runtime:2.2.5"
 annotationProcessor "androidx.room:room-compiler:2.2.5"
 *
 * Then, in your main activity, override the getSmallIcon function so that the notification class have access to the icon you want to use
 *
 * On iOS :
 * You can only use openUrl to open your own application
 */

// In Android you need to create a notification channel
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

/**
 * A notification channel is created to help Android sort notifications between apps and help the user manage them in their settings
 *
 * Creating an existing notification channel with its original values performs no operation, so it's safe to call this function during app initialization.
 *
 * For now, only one notification channel is supported. Calling this function multiple times with different channelId will cause an assertion
 *
 * name: This should usually be the name of your application, but you can decide to display something else
 * description: specify the description that the user sees in the system settings
 * channelId: This should be a unique Id, it is not displayed and shouldn't be translated
 */
void createNotificationChannel(const std::string& name, const std::string& description, const std::string& channelId);

#endif

/**
 * plan a notification using date as a timestamp for when to show it
 * timestamp : in seconds
 * text: text to display in the notification body
 * title: title to display for the notification
 * url: url to open when clicking on the notification
 * /!\ on iOS, this will only launch your application and will work as if an url was opened to your application
 * notificationId: an identifier for the notification, this id must be unique a method to cancel notification using this Id will later be implemented
 */
void planNotification(long timestamp, const std::string& text, const std::string& title, const std::string& url, const int notificationId);

NS_FENNEX_END

#endif /* NotificationHandler_h */
