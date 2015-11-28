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
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "CCJniConversion.h"
#include "PListPersist.h"
#include "FenneX.h"
#include <android/log.h>

USING_NS_FENNEX;

#define  CLASS_NAME "com/fennex/modules/LocalNotification"

void scheduleNotification(float timeFromNow, const std::string& alertBody, const std::string& alertAction, const std::string& soundName, CCDictionary* userInfo)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"scheduleNotification", "(FLjava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Object;)V");
    CCAssert(functionExist, "Function doesn't exist");
    
    saveObjectToFile(userInfo, "userInfo.plist");
    
    jstring stringArg1 = minfo.env->NewStringUTF(alertBody.c_str());
    jstring stringArg2 = minfo.env->NewStringUTF(alertAction.c_str());
    jstring stringArg3 = minfo.env->NewStringUTF(soundName.c_str());
    jobjectArray array = jobjectArrayFromCCDictionary(minfo.env, userInfo);
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)timeFromNow, stringArg1, stringArg2, stringArg3, array);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(stringArg1);
    minfo.env->DeleteLocalRef(stringArg2);
    minfo.env->DeleteLocalRef(stringArg3);
    minfo.env->DeleteLocalRef(array);
}

void cancelAllNotifications()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"cancelAllNotifications", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void stopService()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stopService", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void notifyNotifClicked(jobjectArray array)
{
    CCDictionary* infos = CCDictionaryFromjobjectArray(JniHelper::getEnv(), array);
    CCString* callBackEvent = (CCString*)infos->objectForKey("CallbackEvent");
    
    if(callBackEvent != NULL)
    {
        DelayedDispatcher::eventAfterDelay(callBackEvent->getCString(), infos, 0.01);
    }
    
    notifyDeletePListFiles();
}

void notifyDeletePListFiles()
{
    CCLOG("LocalNotificationWrapper->notifyDeletePListFiles");
    deleteFile("exitDate.plist");
    deleteFile("lastScene.plist");
    deleteFile("userInfo.plist");
}

extern "C"
{
    void Java_com_fennex_modules_LocalNotification_notifyNotifClickedWrap(JNIEnv* env, jobject thiz, jobjectArray array)
    {
        notifyNotifClicked(array);
    }
    
    void Java_com_fennex_modules_LocalNotification_notifyDeletePListFiles(JNIEnv* env, jobject thiz)
    {
        notifyDeletePListFiles();
    }
}
