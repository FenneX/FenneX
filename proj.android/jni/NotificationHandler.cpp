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
#include "NotificationHandler.h"
#include "platform/android/jni/JniHelper.h"

#define  CLASS_NAME "com/fennex/modules/NotificationHandler"

NS_FENNEX_BEGIN

void createNotificationChannel(const std::string& name, const std::string& description, const std::string& channelId)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "createNotificationChannel", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jname = minfo.env->NewStringUTF(name.c_str());
    jstring jdescription = minfo.env->NewStringUTF(description.c_str());
    jstring jchannelId = minfo.env->NewStringUTF(channelId.c_str());

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jname, jdescription, jchannelId);

    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jname);
    minfo.env->DeleteLocalRef(jdescription);
    minfo.env->DeleteLocalRef(jchannelId);
}

void planNotification(long timestamp,const std::string& text,const std::string& url)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "planNotification", "(JLjava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jlong jTimestamp = (int64_t) timestamp;

    jstring jtext = minfo.env->NewStringUTF(text.c_str());
    jstring jurl = minfo.env->NewStringUTF(url.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jTimestamp, jtext, jurl);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jtext);
    minfo.env->DeleteLocalRef(jurl);
}

NS_FENNEX_END