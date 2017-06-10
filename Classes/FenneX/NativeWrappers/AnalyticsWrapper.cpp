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

void AnalyticsWrapper::setAppVersion(const std::string& version)
{
    // automatic with firebase
    sharedInstance()->appVersion = version;
}

void AnalyticsWrapper::setDebugLogEnabled(bool value)
{
    GASetDebugLogEnabled(value);
}

void AnalyticsWrapper::setSecureTransportEnabled(bool value)
{
    GASetSecureTransportEnabled(value);
}

void AnalyticsWrapper::logPageView(const std::string& pageName)
{
    GALogPageView(pageName);
    firebaseLogPageView(pageName);
    sharedInstance()->lastPageName = pageName;
}

void AnalyticsWrapper::logEvent(const std::string& eventName, const std::string& label, int value)
{
    GALogEvent(eventName, label, value);
    std::string fullFlurryName = eventName + " - " + (!sharedInstance()->lastPageName.empty()? sharedInstance()->lastPageName : "NoScene");
    if(label.empty())
    {
        firebaseLogEvent(fullFlurryName);
    }
    else
    {
        firebaseLogEventWithParameters(fullFlurryName, label, std::to_string(value));
    }
}

void AnalyticsWrapper::endSession()
{
    GAEndSession();
}
