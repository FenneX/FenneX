//
//  NativeUtility.h
//  FenneX
//
//  Created by François Dupayrat on 29/04/13.
//
//

#ifndef FenneX_NativeUtility_h
#define FenneX_NativeUtility_h

#include "Shorteners.h"
#include "AppDelegate.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN

bool isPhone();
std::string getLocalPath(const char* name);

//Use AppName if you need to actually show it. Use package identifier if you need to save files for example, as it does not contain special characters
std::string getAppName();
std::string getPackageIdentifier();

/* Get an unique identifier of the device.
 - on iOS, it uses identifierForVendor, which CAN change if all apps are uninstalled at once then reinstalled. It can also be null at the beginning. If it is, retry later
 - on Android, it uses ANDROID_IT, which CAN change on factory reset and can be different if there are several accounts on the device.
 */
std::string getUniqueIdentifier();


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void copyResourceFileToLocal(const char* path);
#endif


//implemented by platform because cocos2d version doesn't return the string identifier
//Android version is a copy of getCurrentLanguageJNI defined in main.cpp
//iOS version is defined in AppController
std::string getLocalLanguage();

bool isConnected();

void preventIdleTimerSleep(bool prevent);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//The real goal of those methods (which should be called when starting a scene initialisation and after it's fully initialized and drawn)
//is to mitigate the effect of the Garbage Collector due to Label modifications (avoid running it during scene initialisation)
void startSceneInitialisation();
void runGarbageCollector();
#endif

//will format the date in short format (example : 9/8/2010) according user local
const char* formatDate(time_t date);

//Return a float between 0.0 (muted) and 1.0 (full volume)
float getDeviceVolume();
void setDeviceVolume(float volume);

void setDeviceNotificationVolume(float volume);

//Return the step you should use for the current device
float getVolumeStep();

//Will change the native background color (behind everything else, including video and opengl scene)
//The values should range from 0 to 255 (0,0,0 is black, 255,255,255 is white)
void setBackgroundColor(int r, int g, int b);

/*Will vibrate the device, or play a sound if vibrator is not available
 on iOS, the time is fixed by the iOS, and there may be some conflicts with AudioPlayerRecorder
 on Android, the <uses-permission android:name="android.permission.VIBRATE"/> permission is required
 */
void vibrate(int milliseconds);
bool canVibrate();

//Return a float between 0 and 1 describing the device luminosity
float getDeviceLuminosity();

//Change the device luminosity, must be a float between 0 and 1
void setDeviceLuminosity(float);

//Open system settings app. Should always work on iOS8+ and Android. Not available on iOS < 8
//Return true if the settings app will open
bool openSystemSettings();

//Launch Youtube intent on Android, simple openUrl on iOS
void launchYoutube();

//Return if a package is installed on android, return false on iOS
bool isPackageInstalled(std::string packageName);

//On iOS, those notifications will automatically start being thrown after getDeviceVolume has been called for the first time
//On Android, they are always on
static inline void notifyVolumeChanged()
{
	DelayedDispatcher::eventAfterDelay("VolumeChanged", DcreateP(Fcreate(getDeviceVolume()), Screate("Volume"), NULL), 0.01);
}

inline void notifyMemoryWarning(){
	AppDelegate* delegate = (AppDelegate*)cocos2d::Application::getInstance();
#warning : maybe this should be async to run on main thread ?
	delegate->applicationDidReceiveMemoryWarning();
}

NS_FENNEX_END

#endif
