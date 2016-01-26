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

#include <jni.h>
#include "cocos2d.h"
#include "FenneX.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "NetworkUtility.h"
#include "platform/android/jni/JniHelper.h"

USING_NS_FENNEX;

#define  CLASS_NAME "com/fennex/modules/NetworkUtility"

NS_FENNEX_BEGIN

bool isConnected()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isConnected", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool isConnected =  minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return isConnected;
}

void openWifiSettings()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "openWifiSettings", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

int nextDownloadID = 0;
std::map<int, std::function<void()>> onSuccessCallbacks;
std::map<int, std::function<void(DownloadError)>> onErrorCallbacks;
std::map<int, std::function<void(float)>> onProgressCallbacks;
std::map<int, std::function<void(int)>> onSizeReceivedCallbacks;

void downloadFile(std::string url, std::string localPath, std::function<void()> onSuccess, std::function<void(DownloadError)>onError, std::function<void(float)>onProgressUpdate, std::function<void(int)> onSizeReceived)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "downloadFile", "(ILjava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jurl = minfo.env->NewStringUTF(url.c_str());
    jstring jpath = minfo.env->NewStringUTF(localPath.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)nextDownloadID, jurl, jpath);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jurl);
    minfo.env->DeleteLocalRef(jpath);
    if(onSuccess) onSuccessCallbacks[nextDownloadID] = onSuccess;
    if(onError) onErrorCallbacks[nextDownloadID] = onError;
    if(onProgressUpdate) onProgressCallbacks[nextDownloadID] = onProgressUpdate;
    if(onSizeReceived) onSizeReceivedCallbacks[nextDownloadID] = onSizeReceived;
    nextDownloadID++;
}

void notifySuccess(int downloadID)
{
    std::map<int, std::function<void()>>::iterator result = onSuccessCallbacks.find((int)downloadID);
    if (result != onSuccessCallbacks.end())
    {
        result->second();
    }
}

void notifyError(int downloadID, int errorCode)
{
    std::map<int, std::function<void(DownloadError)>>::iterator result = onErrorCallbacks.find((int)downloadID);
    if (result != onErrorCallbacks.end())
    {
        result->second((DownloadError)errorCode);
    }
}

void notifyProgressUpdate(int downloadID, float percent)
{
    std::map<int, std::function<void(float)>>::iterator result = onProgressCallbacks.find((int)downloadID);
    if (result != onProgressCallbacks.end())
    {
        result->second(percent);
    }
}

void notifyLengthResolved(int downloadID, int length)
{
    std::map<int, std::function<void(int)>>::iterator result = onSizeReceivedCallbacks.find((int)downloadID);
    if (result != onSizeReceivedCallbacks.end())
    {
        result->second(length);
    }
}

NS_FENNEX_END

extern "C"
{
    void Java_com_fennex_modules_NetworkUtility_notifySuccess(JNIEnv* env, jobject thiz, jint downloadID)
    {
        notifySuccess((int)downloadID);
    }
    
    void Java_com_fennex_modules_NetworkUtility_notifyError(JNIEnv* env, jobject thiz, jint downloadID, jint errorCode)
    {
        notifyError((int)downloadID, (int)errorCode);
    }
    
    void Java_com_fennex_modules_NetworkUtility_notifyProgressUpdate(JNIEnv* env, jobject thiz, jint downloadID, jfloat percent)
    {
        notifyProgressUpdate((int)downloadID, (float)percent);
    }
    
    void Java_com_fennex_modules_NetworkUtility_notifyLengthResolved(JNIEnv* env, jobject thiz, jint downloadID, jint length)
    {
        notifyLengthResolved((int)downloadID, (int)length);
    }
}
