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

#include <jni.h>
#include "FenneX.h"
#include <stdio.h>
#include <stdlib.h>
#include "NativeUtility.h"
#include "platform/android/jni/JniHelper.h"

#define  CLASS_NAME "com/fennex/modules/NativeUtility"

NS_FENNEX_BEGIN

bool isRunningTests()
{
    return false;
}

bool isPhone()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isPhone", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

std::string getOpenUrl()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getOpenUrl", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring jOpenUrl = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string openUrl = JniHelper::jstring2string(jOpenUrl);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jOpenUrl);
    return openUrl;
}

std::string getAppName()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAppName", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string path = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return path;
}

std::string getPackageIdentifier()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getPackageIdentifier", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string path = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return path;
}

std::string getUniqueIdentifier()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getUniqueIdentifier", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string identifier = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return identifier;
}

std::string getAppVersionNumber()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAppVersionNumber", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string versionNumber = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return versionNumber;
}

int getAppVersionCode()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAppVersionCode", "()I");
    CCAssert(functionExist, "Function doesn't exist");

    int versionCode = minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return versionCode;
}

std::string getDeviceModelIdentifier()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceModelIdentifier", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string modelIdentifier = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return modelIdentifier;
}

std::string getDeviceModelName()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceModelName", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string modelName = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return modelName;
}

std::string getDeviceVersion()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceVersion", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string deviceVersion = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return deviceVersion;
}

int getDeviceSDK()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceSDK", "()I");
    CCAssert(functionExist, "Function doesn't exist");

    int sdk = minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return sdk;
}

uint64_t getTotalStorageSpace()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getTotalStorageSpace", "()J");
    CCAssert(functionExist, "Function doesn't exist");

    //jlong is a 64-bit signed integer. Cast it to a 64-bit unsigned integer
    uint64_t totalSpace = (uint64_t)minfo.env->CallStaticLongMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return totalSpace;
}

uint64_t getAvailableStorageSpace()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAvailableStorageSpace", "()J");
    CCAssert(functionExist, "Function doesn't exist");

    uint64_t availableSpace = (uint64_t)minfo.env->CallStaticLongMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return availableSpace;
}

std::string getMovieFolderName()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getMovieFolderName", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string folderName = JniHelper::jstring2string(name);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(name);
    return folderName;
}


void copyResourceFileToLocal(const std::string& path)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "copyResourceFileToLocal", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jpath = minfo.env->NewStringUTF(path.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jpath);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jpath);
}

std::string getLocalLanguage()
{
    return Application::getInstance()->getCurrentLanguageCode();
}

void preventIdleTimerSleep(bool prevent)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "preventIdleTimerSleep", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)prevent);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool doesPreventIdleTimerSleep()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "doesPreventIdleTimerSleep", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void startSceneInitialisation()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "startSceneInitialisation", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void runGarbageCollector()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "runGarbageCollector", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string formatDate(time_t date)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "formatDate", "(J)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring result = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, (jlong)date);
    std::string dateString = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(result);
    return dateString;
}

std::string formatDateTime(time_t dateTime, DateFormat dayFormat, DateFormat hourFormat)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "formatDateTime", "(JII)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring result = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, (jlong)dateTime, (jint)dayFormat, (jint)hourFormat);
    std::string dateTimeString = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(result);
    return dateTimeString;
}

std::string formatDateTime(time_t date, std::string formatTemplate)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "formatDateTime", "(JLjava/lang/String;)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring jformatTemplate = minfo.env->NewStringUTF(formatTemplate.c_str());
    jstring result = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, (jlong)date, jformatTemplate);
    std::string dateString = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jformatTemplate);
    minfo.env->DeleteLocalRef(result);
    return dateString;    
}

std::string formatDurationShort(int seconds)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "formatDurationShort", "(I)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");

    jstring result = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, (jint)seconds);
    std::string dateTimeString = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(result);
    return dateTimeString;
}

float getDeviceVolume()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceVolume", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setDeviceVolume(float volume)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceVolume", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)volume);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void setDeviceNotificationVolume(float volume)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceNotificationVolume", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)volume);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float getVolumeStep()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getVolumeStep", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setBackgroundColor(int r, int g, int b)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setBackgroundColor", "(III)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)r, (jint)g, (jint)b);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void vibrate(int milliseconds)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "vibrate", "(I)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)milliseconds);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool canVibrate()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "canVibrate", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float getDeviceLuminosity()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceLuminosity", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setDeviceLuminosity(float luminosity)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceLuminosity", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)luminosity);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool openSystemSettings()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "openSystemSettings", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool isPackageInstalled(const std::string& packageId)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isPackageInstalled", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jpackageId = minfo.env->NewStringUTF(packageId.c_str());
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, jpackageId);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jpackageId);
    return result;
}

int getApplicationVersion(const std::string& packageId)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getApplicationVersion", "(Ljava/lang/String;)I");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jpackageId = minfo.env->NewStringUTF(packageId.c_str());
    int result = minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID, jpackageId);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jpackageId);
    return result;
}

NS_FENNEX_END

extern "C"
{
    void Java_com_fennex_modules_NativeUtility_notifyMemoryWarning(JNIEnv* env, jobject thiz)
    {
        notifyMemoryWarning();
    }
    void Java_com_fennex_modules_NativeUtility_notifyVolumeChanged(JNIEnv* env, jobject thiz)
    {
        notifyVolumeChanged();
    }
    void Java_com_fennex_modules_NativeUtility_notifyUrlOpened(JNIEnv* env, jobject thiz, jstring name)
    {
        notifyUrlOpened(JniHelper::jstring2string(name));
    }

    jstring Java_com_fennex_modules_NativeUtility_getNativeString(JNIEnv* env, jobject thiz, jstring key)
    {
        std::string result = getNativeString(JniHelper::jstring2string(key));
        jstring jresult = cocos2d::StringUtils::newStringUTFJNI(JniHelper::getEnv(), result.c_str());
        return jresult;
    }
}

