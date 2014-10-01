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

#define CLASS_NAME "com/diwublog/AnalyticX/AnalyticXBridge"

using namespace std;

//Start and get agent methods
void AnalyticsWrapper::GAStartSession(const char* apiKey)
{
	JniMethodInfo minfo;

	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GAStartSession", "(Ljava/lang/String;)V"), "Function doesn't exist");

    jstring jApiKey = minfo.env->NewStringUTF(apiKey);
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jApiKey);
	minfo.env->DeleteLocalRef(minfo.classID);

    if(!sharedInstance()->appVersion.empty())
    {
        GASetAppVersion(sharedInstance()->appVersion.c_str());
        sharedInstance()->appVersion = "";
    }
    minfo.env->DeleteLocalRef(jApiKey);
}

const char * AnalyticsWrapper::GAGetGAAgentVersion()
{
	/**
	 * No method available to retrieve the version of GA.
	 */
    return "";
}

void AnalyticsWrapper::flurryStartSession(const char * apiKey)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"), "Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryOnStartSession");
    jstring stringArg1 = minfo.env->NewStringUTF(apiKey);
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

const char * AnalyticsWrapper::flurryGetFlurryAgentVersion()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"flurryGetAgentVersion", "()I"), "Function doesn't exist");
    jint version =  minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);

    static char result[20];
    sprintf(result, "%d", version);
    return result;
}

//GA methods
void AnalyticsWrapper::GASetAppVersion(const char * version)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetAppVersion", "(Ljava/lang/String;)V"), "Function doesn't exist");
	jstring jVersion = minfo.env->NewStringUTF(version);
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
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetSecureTransportEnabled", "(Z)V"), "Function doesn't exist");
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
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetDebugLogEnabled", "(Z)V"), "Function doesn't exist");
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
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GASetTrackExceptionsEnabled", "(Z)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)value);
	minfo.env->DeleteLocalRef(minfo.classID);
}

void AnalyticsWrapper::GALogPageView(const char* pageName)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GALogPageView", "(Ljava/lang/String;)V"),"Function doesn't exist");
	jstring jPageName = minfo.env->NewStringUTF(pageName);
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPageName);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jPageName);
}

void AnalyticsWrapper::GALogEvent(const char * eventName, const char* label, int value)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GALogEvent", "(Ljava/lang/String;Ljava/lang/String;I)V"),"Function doesn't exist");

	jstring jEventName = minfo.env->NewStringUTF(eventName);
	jstring jLabel = minfo.env->NewStringUTF(label);

	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jEventName, jLabel, (jint)value);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jEventName);
	minfo.env->DeleteLocalRef(jLabel);
}

void AnalyticsWrapper::GAEndSession()
{
	CCLOG("Google Analytics: GAEndSession().");
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"GAEndSession", "()V"),"Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
}


//Flurry methods
void AnalyticsWrapper::flurrySetAppVersion(const char * version)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurrySetAppVersion");
    jstring stringArg1 = minfo.env->NewStringUTF(version);
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurrySetDebugLogEnabled(bool value)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurrySetLogEnabled");
    jstring stringArg1 = minfo.env->NewStringUTF("placeholder");
    jstring stringArg2 = minfo.env->NewStringUTF(value ? "true" : "false");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurrySetSecureTransportEnabled(bool value)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurrySetSecureEnabled");
    jstring stringArg1 = minfo.env->NewStringUTF("placeholder");
    jstring stringArg2 = minfo.env->NewStringUTF(value ? "true" : "false");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryLogPageView()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryLogPageView");
    jstring stringArg1 = minfo.env->NewStringUTF("placeholder");
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryLogEvent(const char * eventName)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryLogEvent");
    jstring stringArg1 = minfo.env->NewStringUTF(eventName);
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryLogEventWithParameters(const char * eventName, CCDictionary * parameters)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    string eventNameString("flurryLogEventWithParameters,");
    eventNameString += eventName;
	jstring stringArg0 = minfo.env->NewStringUTF(eventNameString.c_str());
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, jobjectArrayFromCCDictionary(minfo.env, parameters), stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryLogEventTimed(const char * eventName, bool timed)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryLogEventTimed");
    jstring stringArg1 = minfo.env->NewStringUTF(eventName);
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    if (timed == true)
    {
        stringArg2 = minfo.env->NewStringUTF("true");
    }

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryLogEventWithParametersTimed(const char * eventName, CCDictionary * parameters, bool timed)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    string eventNameString("flurryLogEventWithParametersTimed,");
    eventNameString += eventName;
    jstring stringArg0 = minfo.env->NewStringUTF(eventNameString.c_str());
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    if (timed == true)
    {
        stringArg2 = minfo.env->NewStringUTF("true");
    }

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, jobjectArrayFromCCDictionary(minfo.env, parameters), stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg2);
}

void AnalyticsWrapper::flurryEndTimedEventWithParameters(const char * eventName, CCDictionary * parameters)
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryEndTimedEvent");
    jstring stringArg1 = minfo.env->NewStringUTF(eventName);
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}
void AnalyticsWrapper::flurryEndSession()
{
    JniMethodInfo minfo;
    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"Bridge", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),"Function doesn't exist");

    jstring stringArg0 = minfo.env->NewStringUTF("flurryOnEndSession");
    jstring stringArg1 = minfo.env->NewStringUTF("placeholder");
    jstring stringArg2 = minfo.env->NewStringUTF("false");

    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, stringArg0, stringArg1, stringArg2);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg0);
	minfo.env->DeleteLocalRef(stringArg1);
	minfo.env->DeleteLocalRef(stringArg2);
}
