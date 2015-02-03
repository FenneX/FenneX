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
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isPhone", "()Z"), "Function doesn't exist");
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

std::string getLocalPath(const char* name)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getLocalPath", "()Ljava/lang/String;"), "Function doesn't exist");

	jstring directory = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    //minfo.env->DeleteLocalRef(minfo.classID);

	const char *nativeString = minfo.env->GetStringUTFChars(directory, 0);
    CCLOG("Getting local path : %s, name : %s", nativeString, name);
	std::string path = std::string(nativeString) + "/" + name;
	minfo.env->ReleaseStringUTFChars(directory, nativeString);
	minfo.env->DeleteLocalRef(directory);
	minfo.env->DeleteLocalRef(minfo.classID);
	return path;
}

std::string getAppName()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAppName", "()Ljava/lang/String;"), "Function doesn't exist");

	jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
       
    const char *nativeString = minfo.env->GetStringUTFChars(name, 0);
    CCLOG("Getting app name : %s", nativeString);
    std::string path = std::string(nativeString);
    minfo.env->ReleaseStringUTFChars(name, nativeString);
    return path;
}

std::string getPackageIdentifier()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getPackageIdentifier", "()Ljava/lang/String;"), "Function doesn't exist");

	jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);

	const char *nativeString = minfo.env->GetStringUTFChars(name, 0);
    CCLOG("Getting app package identifier : %s", nativeString);
    std::string path = std::string(nativeString);
    minfo.env->ReleaseStringUTFChars(name, nativeString);
    return path;
}

std::string getUniqueIdentifier()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getUniqueIdentifier", "()Ljava/lang/String;"), "Function doesn't exist");

	jstring name = (jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);

	const char *nativeString = minfo.env->GetStringUTFChars(name, 0);
    CCLOG("Getting Unique identifier : %s", nativeString);
    std::string identifier = std::string(nativeString);
    minfo.env->ReleaseStringUTFChars(name, nativeString);
    return identifier;
}

void copyResourceFileToLocal(const char* path)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "copyResourceFileToLocal", "(Ljava/lang/String;)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(path));
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string getLocalLanguage()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, "org/cocos2dx/lib/Cocos2dxHelper", "getCurrentLanguage", "()Ljava/lang/String;"), "Function doesn't exist");

    jstring str = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    std::string ret = std::string(JniHelper::jstring2string(str).c_str());
    minfo.env->DeleteLocalRef(str);

    return ret;
}

bool isConnected()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isConnected", "()Z"), "Function doesn't exist");
	return minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
}

void preventIdleTimerSleep(bool prevent)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "preventIdleTimerSleep", "(Z)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)prevent);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void startSceneInitialisation()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "startSceneInitialisation", "()V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void runGarbageCollector()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "runGarbageCollector", "()V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

const char * formatDate(time_t date)
{
	tm* t = localtime(&date);
	//We add 1 to to the month because it seems that january is 0, etc.
	//TODO : use a native method to format according user locales
	return ScreateF("%i/%i/%i", t->tm_mday, t->tm_mon+1, t->tm_year-100)->getCString();
}

float getDeviceVolume()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceVolume", "()F"), "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setDeviceVolume(float volume)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceVolume", "(F)V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)volume);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void setDeviceNotificationVolume(float volume)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceNotificationVolume", "(F)V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)volume);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float getVolumeStep()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getVolumeStep", "()F"), "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setBackgroundColor(int r, int g, int b)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setBackgroundColor", "(III)V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)r, (jint)g, (jint)b);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void vibrate(int milliseconds)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "vibrate", "(I)V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)milliseconds);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool canVibrate()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "canVibrate", "()Z"), "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float getDeviceLuminosity()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getDeviceLuminosity", "()F"), "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setDeviceLuminosity(float luminosity)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "setDeviceLuminosity", "(F)V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)luminosity);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool openSystemSettings()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "openSystemSettings", "()Z"), "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void launchYoutube()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "launchYoutube", "()V"), "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool isPackageInstalled(std::string packageId)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isPackageInstalled", "(Ljava/lang/String;)Z"), "Function doesn't exist");
	return minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(packageId.c_str()));
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

