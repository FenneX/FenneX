/****************************************************************************
 Copyright (c) 2013-2014 Auticiel SAS
 Copyright (c) 2012 - Di Wu
 
 http://www.fennex.org
 AnalyticX: https://github.com/diwu/AnalyticX
 
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

#include "AnalyticsWrapper.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include "CCJniConversion.h"
#include <string>

#define CLASS_NAME "com/fennex/modules/Analytics"

using namespace std;

void AnalyticsWrapper::firebaseSetProperty(const std::string& propertyName, const std::string& propertyValue)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"firebaseSetProperty", "(Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");

    jstring jPropertyName = minfo.env->NewStringUTF(propertyName.c_str());
    jstring jPropertyValue = minfo.env->NewStringUTF(propertyValue.c_str());

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPropertyName, jPropertyValue);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPropertyName);
    minfo.env->DeleteLocalRef(jPropertyValue);
}

void AnalyticsWrapper::firebaseLogPageView(const std::string& pageName){

    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"firebaseLogPageView", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPageName = minfo.env->NewStringUTF(pageName.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPageName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPageName);
}

void AnalyticsWrapper::firebaseLogEvent(const std::string& eventName) {

    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"firebaseLogEvent", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jEventName = minfo.env->NewStringUTF(eventName.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jEventName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jEventName);
}

void AnalyticsWrapper::firebaseLogEventWithParameters(const std::string& eventName, const std::string& label, const std::string& value) {
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"firebaseLogEventWithParameters", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");

    jstring jEventName = minfo.env->NewStringUTF(eventName.c_str());
    jstring jLabel = minfo.env->NewStringUTF(label.c_str());
    jstring jValue = minfo.env->NewStringUTF(value.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jEventName, jLabel, jValue);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jEventName);
    minfo.env->DeleteLocalRef(jLabel);
    minfo.env->DeleteLocalRef(jValue);
}