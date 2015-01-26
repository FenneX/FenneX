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

#include "NativeUtility.h"
#include "MailUrlWrapper.h"
#import <UIKit/UIKit.h>
#import "Reachability.h"
#import <MediaPlayer/MediaPlayer.h>
#import "AppController.h"
#import <AudioToolbox/AudioServices.h>


NS_FENNEX_BEGIN

MPVolumeView *invisibleVolumeView = NULL;

bool isPhone()
{
    return [[UIDevice currentDevice] userInterfaceIdiom] != UIUserInterfaceIdiomPad;
}

std::string getLocalPath(const char* name)
{
    return std::string(getenv("HOME"))+"/Documents/"+name;
}

std::string getAppName()
{
    return [[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleDisplayName"] UTF8String];
}

std::string getPackageIdentifier()
{
    return [[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"] UTF8String];
}

std::string getUniqueIdentifier()
{
    return [[[[UIDevice currentDevice] identifierForVendor] UUIDString] UTF8String];
}

std::string getLocalLanguage()
{
    // get the current language and country config
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    
    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
    return [languageCode UTF8String];
}

bool isConnected()
{
    Reachability *reach = [Reachability reachabilityForInternetConnection];
    NetworkStatus netStatus = [reach currentReachabilityStatus];
    return netStatus != NotReachable;
}

void preventIdleTimerSleep(bool prevent)
{
    [UIApplication sharedApplication].idleTimerDisabled = prevent;
}

//will format the date in short format (example : 9/8/2010) according user local
const char* formatDate(time_t date)
{
    NSDate *nsdate = [NSDate dateWithTimeIntervalSince1970:date];//"yyyy/MM/dd"
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    
    [formatter setDateStyle:NSDateFormatterShortStyle];
    
    [formatter setTimeStyle:NSDateFormatterNoStyle];
    
    NSString *result = [formatter stringForObjectValue:nsdate];
    return [result UTF8String];
}

float getDeviceVolume()
{
    if(invisibleVolumeView == NULL)
    {
        invisibleVolumeView = [MPVolumeView new];
        [[NSNotificationCenter defaultCenter] addObserverForName:@"AVSystemController_SystemVolumeDidChangeNotification"
                                                          object:nil
                                                           queue:nil
                                                      usingBlock:^(NSNotification *note) {
                                                          notifyVolumeChanged();
                                                      }];
    }
    return [[MPMusicPlayerController applicationMusicPlayer] volume];
}

void setDeviceVolume(float volume)
{
    return [[MPMusicPlayerController applicationMusicPlayer] setVolume:volume];
}

void setDeviceNotificationVolume(float volume)
{
    //Not implemented on iOS
}

float getVolumeStep()
{
    return 1.0/16.0; //the default volume handling contains 16 steps
}

void setBackgroundColor(int r, int g, int b)
{
    //[[AppController sharedController].window setBackgroundColor:[UIColor colorWithRed:(float)r/255 green:(float)g/255 blue:(float)b/255 alpha:1.0]];
}

void vibrate(int milliseconds)
{
#warning TODO : solve conflicts with AudioPlayerRecorder
    AudioServicesPlayAlertSound(kSystemSoundID_Vibrate);
}

bool canVibrate()
{
    //Hacky solution, there is currently no method in iOS SDK to know if a device can vibrate
    return [[UIDevice currentDevice].model isEqualToString:@"iPhone"];
}

float getDeviceLuminosity() {
    UIScreen *mainScreen = [UIScreen mainScreen];
    return mainScreen.brightness;
}

void setDeviceLuminosity(float percent) {
    UIScreen *mainScreen = [UIScreen mainScreen];
    mainScreen.brightness = percent;
}

bool openSystemSettings()
{
    //Only available since iOS8
    if (&UIApplicationOpenSettingsURLString != NULL) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
        return true;
    }
    return false;
}

void launchYoutube()
{
    openUrl("http://www.youtube.fr/");
}

bool isPackageInstalled(std::string packageName)
{
    return false;
}

NS_FENNEX_END