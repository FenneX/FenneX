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

USING_NS_FENNEX;

#define  CLASS_NAME "com/fennex/modules/NativeUtility"

NS_FENNEX_BEGIN

bool isPhone()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isPhone", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void discardSplashScreen()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "discardSplashScreen", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string getPublicPath(const std::string& name)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getPublicPath", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring directory = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    
    std::string path = JniHelper::jstring2string(directory) + "/" + name;
    minfo.env->DeleteLocalRef(directory);
    minfo.env->DeleteLocalRef(minfo.classID);
    return path;
}

//Since we use this method VERY often, cache the result instead of doing a JNI Call every time
std::string localPathCache = "";

std::string getLocalPath(const std::string& name)
{
    if(localPathCache.length() == 0)
    {
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getLocalPath", "()Ljava/lang/String;");
        CCAssert(functionExist, "Function doesn't exist");
        
        jstring directory = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
        localPathCache = JniHelper::jstring2string(directory);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(directory);
    }
    return localPathCache + "/" + name;
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
    tm* t = localtime(&date);
    //We add 1 to to the month because it seems that january is 0, etc.
    //TODO : use a native method to format according user locales
    return std::to_string(t->tm_mday) + "/" + std::to_string(t->tm_mon+1) + "/" + std::to_string(t->tm_year-100);
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

void launchYoutube()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "launchYoutube", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
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
}

