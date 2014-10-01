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

static AnalyticsWrapper *s_SharedInstance = NULL;

AnalyticsWrapper* AnalyticsWrapper::sharedInstance(void)
{
    if (!s_SharedInstance)
    {
        s_SharedInstance = new AnalyticsWrapper();
    }
    
    return s_SharedInstance;
}

void AnalyticsWrapper::init()
{
    sharedInstance()->lastPageName = "";
    sharedInstance()->appVersion = "";
}

void AnalyticsWrapper::setAppVersion(const char * version)
{
    flurrySetAppVersion(version);
    sharedInstance()->appVersion = std::string(version);
}

void AnalyticsWrapper::setDebugLogEnabled(bool value)
{
    GASetDebugLogEnabled(value);
    flurrySetDebugLogEnabled(value);
}

void AnalyticsWrapper::setSecureTransportEnabled(bool value)
{
    GASetSecureTransportEnabled(value);
    flurrySetSecureTransportEnabled(value);
}

void AnalyticsWrapper::logPageView(const char* pageName)
{
    if(!sharedInstance()->lastPageName.empty())
    {
        flurryEndTimedEventWithParameters(ScreateF("Scene: %s", sharedInstance()->lastPageName.c_str())->getCString(), NULL);
    }
    flurryLogEventTimed(ScreateF("Scene: %s", pageName)->getCString(), true);
    flurryLogPageView();
    GALogPageView(pageName);
    sharedInstance()->lastPageName = pageName;
}

void AnalyticsWrapper::logEvent(const char* eventName, const char* label, int value)
{
    GALogEvent(eventName, label, value);
    const char* fullFlurryName = ScreateF("%s - %s", eventName, !sharedInstance()->lastPageName.empty()? sharedInstance()->lastPageName.c_str() : "NoScene")->getCString();
    if(label == NULL)
    {
        flurryLogEvent(fullFlurryName);
    }
    else
    {
        cocos2d::CCDictionary* param = cocos2d::CCDictionary::create();
        param->setObject(cocos2d::CCInteger::create(value), label);
        flurryLogEventWithParameters(fullFlurryName, param);
    }
}

void AnalyticsWrapper::endSession()
{
    GAEndSession();
    flurryEndSession();
}
