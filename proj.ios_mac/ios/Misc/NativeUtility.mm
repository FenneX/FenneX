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
#import "NSFileManager+ApplicationSupport.h"
#import "NSStringUtility.h"

NS_FENNEX_BEGIN

MPVolumeView *invisibleVolumeView = nullptr;

bool isRunningTests()
{
    //Copied from https://stackoverflow.com/questions/6748087/xcode-test-vs-debug-preprocessor-macros
    NSDictionary* environment = [[NSProcessInfo processInfo] environment];
    return (environment[@"XCInjectBundleInto"] != nil);
}

bool isPhone()
{
    return [[UIDevice currentDevice] userInterfaceIdiom] != UIUserInterfaceIdiomPad;
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

uint64_t getTotalStorageSpace()
{
    NSError *error = nil;
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:NSHomeDirectory() error: &error];
    if (!dictionary) {
        NSLog(@"Error Obtaining System Memory Info: Domain = %@, Code = %ld", [error domain], (long)[error code]);
        return 0;
    }
    return [[dictionary objectForKey: NSFileSystemSize] unsignedLongLongValue];
}

uint64_t getAvailableStorageSpace()
{
    NSError *error = nil;
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:NSHomeDirectory() error: &error];
    if (!dictionary) {
        NSLog(@"Error Obtaining System Memory Info: Domain = %@, Code = %ld", [error domain], (long)[error code]);
        return 0;
    }
    return [[dictionary objectForKey:NSFileSystemFreeSize] unsignedLongLongValue];
}

std::string getMovieFolderName()
{
    return "Movies";
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
    
    // get the current language code (for example, English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
    return [languageCode UTF8String];
}

std::string getLocalCountry()
{
    // get the current language and country config
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    
    // get the current country code (for example, France is "FR", Canada is "CA" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * countryCode = [temp objectForKey:NSLocaleCountryCode];
    return [countryCode UTF8String];
}

void preventIdleTimerSleep(bool prevent)
{
    [UIApplication sharedApplication].idleTimerDisabled = prevent;
}

bool doesPreventIdleTimerSleep()
{
    return [UIApplication sharedApplication].idleTimerDisabled;
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

NSDateFormatterStyle dateFormatToFormatterStyle(DateFormat format)
{
    switch (format)
    {
        case DateFormat::SHORT:
            return NSDateFormatterShortStyle;
        case DateFormat::MEDIUM:
            return NSDateFormatterMediumStyle;
        case DateFormat::LONG:
            return NSDateFormatterLongStyle;
        case DateFormat::FULL:
            return NSDateFormatterFullStyle;
        case DateFormat::NONE:
        default:
            return NSDateFormatterNoStyle;
    }
}

//will format the date in long format (example: November 23, 1937) and time in short format (example: 3:30 PM) according to user local
std::string formatDateTime(time_t date, DateFormat dayFormat, DateFormat hourFormat)
{
    NSDate *nsdate = [NSDate dateWithTimeIntervalSince1970:date];
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    
    [formatter setDateStyle:dateFormatToFormatterStyle(dayFormat)];
    [formatter setTimeStyle:dateFormatToFormatterStyle(hourFormat)];
    
    NSString *result = [formatter stringForObjectValue:nsdate];
    return std::string([result UTF8String]);
}

std::string formatDateTime(time_t date, std::string formatTemplate)
{
    NSDate *nsdate = [NSDate dateWithTimeIntervalSince1970:date];
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    // The components will be reordered according to the locale
    [formatter setDateFormat:[NSDateFormatter dateFormatFromTemplate:getNSString(formatTemplate) options:0 locale:[NSLocale currentLocale]]];
    
    NSString *result = [formatter stringFromDate:nsdate];
    return std::string([result UTF8String]);
}

long parseDate(const std::string& date, DateFormat dayFormat)
{
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    [formatter setDateStyle:dateFormatToFormatterStyle(dayFormat)];
    NSDate *nsdate = [formatter dateFromString:[NSString stringWithFormat:@"%s", date.c_str()]];
    return round([nsdate timeIntervalSince1970]);
}

std::string formatDurationShort(int seconds)
{
    NSDateComponentsFormatter* formatter = [[[NSDateComponentsFormatter alloc] init] autorelease];
    formatter.allowedUnits = NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitSecond;
    formatter.unitsStyle = NSDateComponentsFormatterUnitsStyleAbbreviated;
    formatter.zeroFormattingBehavior = NSDateComponentsFormatterZeroFormattingBehaviorDefault;
    return std::string([[formatter stringFromTimeInterval:seconds] UTF8String]);
}

//Ignore the deprecations about "volume" and "setVolume".
//There is no proper replacement, the suggestion to use MPVolumeView is bad, as we don't want a view there
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
float getDeviceVolume()
{
    if(invisibleVolumeView == nullptr)
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

#pragma GCC diagnostic pop

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
    //removed condition if (&UIApplicationOpenSettingsURLString != nullptr) as we target iOS 8.0 minimum now
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
    return true;
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
