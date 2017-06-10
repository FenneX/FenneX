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
#import <MediaPlayer/MediaPlayer.h>
#import "AppController.h"
#import <AudioToolbox/AudioServices.h>
#import <sys/utsname.h>


NS_FENNEX_BEGIN

MPVolumeView *invisibleVolumeView = NULL;

bool isPhone()
{
    return [[UIDevice currentDevice] userInterfaceIdiom] != UIUserInterfaceIdiomPad;
}

std::string getPublicPath(const std::string& name)
{
    return std::string(getenv("HOME"))+"/Documents/"+name;
}

std::string getLocalPath(const std::string& name)
{
    return std::string(getenv("HOME"))+"/Documents/"+name;
}

std::string getOpenUrl()
{
    return [[AppController sharedController].openUrl UTF8String];
}

std::string getAppName()
{
    return [[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleDisplayName"] UTF8String];
}

std::string getPackageIdentifier()
{
    return [[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"] UTF8String];
}

std::string getAppVersionNumber()
{
    return [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"] UTF8String];
}
int getAppVersionCode()
{
    return -1;
}

//Code from http://stackoverflow.com/questions/11197509/ios-how-to-get-device-make-and-model
std::string getDeviceModelIdentifier()
{
    struct utsname systemInfo;
    
    uname(&systemInfo);
    
    NSString* code = [NSString stringWithCString:systemInfo.machine
                                        encoding:NSUTF8StringEncoding];
    return [code UTF8String];
}

std::string getDeviceModelName()
{
    struct utsname systemInfo;
    
    uname(&systemInfo);
    
    NSString* code = [NSString stringWithCString:systemInfo.machine
                                        encoding:NSUTF8StringEncoding];
    
    NSDictionary* deviceNamesByCode  = @{@"i386"      :@"Simulator",
                              @"x86_64"    :@"Simulator",
                              @"iPod1,1"   :@"iPod Touch",        // (Original)
                              @"iPod2,1"   :@"iPod Touch",        // (Second Generation)
                              @"iPod3,1"   :@"iPod Touch",        // (Third Generation)
                              @"iPod4,1"   :@"iPod Touch",        // (Fourth Generation)
                              @"iPod7,1"   :@"iPod Touch",        // (6th Generation)
                              @"iPhone1,1" :@"iPhone",            // (Original)
                              @"iPhone1,2" :@"iPhone",            // (3G)
                              @"iPhone2,1" :@"iPhone",            // (3GS)
                              @"iPad1,1"   :@"iPad",              // (Original)
                              @"iPad2,1"   :@"iPad 2",            //
                              @"iPad3,1"   :@"iPad",              // (3rd Generation)
                              @"iPhone3,1" :@"iPhone 4",          // (GSM)
                              @"iPhone3,3" :@"iPhone 4",          // (CDMA/Verizon/Sprint)
                              @"iPhone4,1" :@"iPhone 4S",         //
                              @"iPhone5,1" :@"iPhone 5",          // (model A1428, AT&T/Canada)
                              @"iPhone5,2" :@"iPhone 5",          // (model A1429, everything else)
                              @"iPad3,4"   :@"iPad",              // (4th Generation)
                              @"iPad2,5"   :@"iPad Mini",         // (Original)
                              @"iPhone5,3" :@"iPhone 5c",         // (model A1456, A1532 | GSM)
                              @"iPhone5,4" :@"iPhone 5c",         // (model A1507, A1516, A1526 (China), A1529 | Global)
                              @"iPhone6,1" :@"iPhone 5s",         // (model A1433, A1533 | GSM)
                              @"iPhone6,2" :@"iPhone 5s",         // (model A1457, A1518, A1528 (China), A1530 | Global)
                              @"iPhone7,1" :@"iPhone 6 Plus",     //
                              @"iPhone7,2" :@"iPhone 6",          //
                              @"iPhone8,1" :@"iPhone 6S",         //
                              @"iPhone8,2" :@"iPhone 6S Plus",    //
                              @"iPhone8,4" :@"iPhone SE",         //
                              @"iPhone9,1" :@"iPhone 7",          //
                              @"iPhone9,3" :@"iPhone 7",          //
                              @"iPhone9,2" :@"iPhone 7 Plus",     //
                              @"iPhone9,4" :@"iPhone 7 Plus",     //
                              
                              @"iPad4,1"   :@"iPad Air",          // 5th Generation iPad (iPad Air) - Wifi
                              @"iPad4,2"   :@"iPad Air",          // 5th Generation iPad (iPad Air) - Cellular
                              @"iPad4,4"   :@"iPad Mini",         // (2nd Generation iPad Mini - Wifi)
                              @"iPad4,5"   :@"iPad Mini",         // (2nd Generation iPad Mini - Cellular)
                              @"iPad4,7"   :@"iPad Mini",         // (3rd Generation iPad Mini - Wifi (model A1599))
                              @"iPad6,7"   :@"iPad Pro (12.9\")", // iPad Pro 12.9 inches - (model A1584)
                              @"iPad6,8"   :@"iPad Pro (12.9\")", // iPad Pro 12.9 inches - (model A1652)
                              @"iPad6,3"   :@"iPad Pro (9.7\")",  // iPad Pro 9.7 inches - (model A1673)
                              @"iPad6,4"   :@"iPad Pro (9.7\")"   // iPad Pro 9.7 inches - (models A1674 and A1675)
                              };
    
    NSString* deviceName = [deviceNamesByCode objectForKey:code];
    
    if (!deviceName) {
        // Not found on database. At least guess main device type from string contents:
        
        if ([code rangeOfString:@"iPod"].location != NSNotFound) {
            deviceName = @"iPod Touch";
        }
        else if([code rangeOfString:@"iPad"].location != NSNotFound) {
            deviceName = @"iPad";
        }
        else if([code rangeOfString:@"iPhone"].location != NSNotFound){
            deviceName = @"iPhone";
        }
        else {
            deviceName = @"Unknown";
        }
    }
    
    return [deviceName UTF8String];
}

std::string getDeviceVersion()
{
    return [[[UIDevice currentDevice] systemVersion] UTF8String];
}

int getDeviceSDK()
{
    return [[[UIDevice currentDevice] systemVersion] intValue];
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

void preventIdleTimerSleep(bool prevent)
{
    [UIApplication sharedApplication].idleTimerDisabled = prevent;
}

//will format the date in short format (example : 9/8/2010) according user local
std::string formatDate(time_t date)
{
    NSDate *nsdate = [NSDate dateWithTimeIntervalSince1970:date];//"yyyy/MM/dd"
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    
    [formatter setDateStyle:NSDateFormatterShortStyle];
    
    [formatter setTimeStyle:NSDateFormatterNoStyle];
    
    NSString *result = [formatter stringForObjectValue:nsdate];
    return std::string([result UTF8String]);
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

bool isPackageInstalled(const std::string& packageName)
{
    return false;
}

int getApplicationVersion(const std::string& packageName)
{
    return -1;
}

NS_FENNEX_END
