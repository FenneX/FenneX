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

//Start and get agent methods
void AnalyticsWrapper::GAStartSession(const std::string& apiKey)
{
    JniMethodInfo minfo;
    
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GAStartSession", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring jApiKey = minfo.env->NewStringUTF(apiKey.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jApiKey);
    minfo.env->DeleteLocalRef(minfo.classID);
    
    if(!sharedInstance()->appVersion.empty())
    {
        GASetAppVersion(sharedInstance()->appVersion.c_str());
        sharedInstance()->appVersion = "";
    }
    minfo.env->DeleteLocalRef(jApiKey);
}

std::string AnalyticsWrapper::GAGetGAAgentVersion()
{
    /**
     * No method available to retrieve the version of GA.
     */
    return "";
}

//GA methods
void AnalyticsWrapper::GASetAppVersion(const std::string& version)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetAppVersion", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jVersion = minfo.env->NewStringUTF(version.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jVersion);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jVersion);
}

void AnalyticsWrapper::GASetSecureTransportEnabled(bool value)
{
    /**
     * Not implemented in Google Analytics. There is ga_anonymizeIp that remove the last octet of each packet in
     * analytics.xml. Maybe it is Fields.ANONYMIZE_IP or Fields.USE_SECURE.
     */
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetSecureTransportEnabled", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)value);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AnalyticsWrapper::GASetDebugLogEnabled(bool value)
{
    /**
     * Can only change the type of log you want to have (VERBOSE, WARNING, ERROR and INFO) and you can't not
     * disable it.
     */
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetDebugLogEnabled", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)value);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AnalyticsWrapper::GASetTrackExceptionsEnabled(bool value)
{
    /**
     * Cannot be set programmaticaly. Maybe it is Fields.EX_FATAL or Fields.EX_DESCRIPTION but there
     * is no description of what these fields are.
     */
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetTrackExceptionsEnabled", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)value);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AnalyticsWrapper::GALogPageView(const std::string& pageName)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GALogPageView", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPageName = minfo.env->NewStringUTF(pageName.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPageName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPageName);
}

void AnalyticsWrapper::GALogEvent(const std::string& eventName, const std::string& label, int value)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GALogEvent", "(Ljava/lang/String;Ljava/lang/String;I)V");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring jEventName = minfo.env->NewStringUTF(eventName.c_str());
    jstring jLabel = minfo.env->NewStringUTF(label.c_str());
    
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jEventName, jLabel, (jint)value);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jEventName);
    minfo.env->DeleteLocalRef(jLabel);
}

void AnalyticsWrapper::GAEndSession()
{
    CCLOG("Google Analytics: GAEndSession().");
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GAEndSession", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
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
    jstring jPageName = minfo.env->NewStringUTF(eventName.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPageName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPageName);
}

void AnalyticsWrapper::firebaseLogEventWithParameters(const std::string& eventName, cocos2d::CCDictionary * parameters) {
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");

    string eventNameString("firebaseLogEventWithParameters,");
    eventNameString += eventName;
    jstring stringArg0 = minfo.env->NewStringUTF(eventNameString.c_str());
    jstring stringArg2 = minfo.env->NewStringUTF("false");
    jobjectArray array = jobjectArrayFromCCDictionary(minfo.env, parameters);
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, array, stringArg2);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(stringArg0);
    minfo.env->DeleteLocalRef(array);
    minfo.env->DeleteLocalRef(stringArg2);
}