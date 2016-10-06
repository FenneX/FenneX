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
#import "AnalyticXStringUtil.h"
#import "GAI.h"
#import "GAIFields.h"
#import "GAIDictionaryBuilder.h"
#import "Firebase.h"

//Start and get agent methods
void AnalyticsWrapper::GAStartSession(const std::string& apiKey)
{
    [[GAI sharedInstance] trackerWithTrackingId:[NSString stringWithFormat:@"%s", apiKey.c_str()]];
    if(!sharedInstance()->appVersion.empty())
    {
        GASetAppVersion(sharedInstance()->appVersion.c_str());
        sharedInstance()->appVersion = "";
    }
}

std::string AnalyticsWrapper::GAGetGAAgentVersion()
{
    return [kGAIVersion UTF8String];
}

//GA methods
void AnalyticsWrapper::GASetAppVersion(const std::string& version)
{
    [[[GAI sharedInstance] defaultTracker] set:kGAIAppVersion value:[NSString stringWithFormat:@"%s", version.c_str()]];
}

void AnalyticsWrapper::GASetSecureTransportEnabled(bool value)
{
    [[[GAI sharedInstance] defaultTracker] set:kGAIUseSecure value:[(value ? @YES : @NO) stringValue]];
}

void AnalyticsWrapper::GASetDebugLogEnabled(bool value)
{
    [[GAI sharedInstance].logger setLogLevel:value ? kGAILogLevelVerbose : kGAILogLevelError];
}

void AnalyticsWrapper::GASetTrackExceptionsEnabled(bool value)
{
    [GAI sharedInstance].trackUncaughtExceptions = value;
}

void AnalyticsWrapper::GALogPageView(const std::string& pageName)
{
    [[[GAI sharedInstance] defaultTracker] set:kGAIScreenName value:[NSString stringWithFormat:@"%s", pageName.c_str()]];
    [[[GAI sharedInstance] defaultTracker] send:[[GAIDictionaryBuilder createAppView]  build]];
}

void AnalyticsWrapper::GALogEvent(const std::string& eventName, const std::string& label, int value)
{
    [[[GAI sharedInstance] defaultTracker]
     send:[[[GAIDictionaryBuilder createEventWithCategory:[NSString stringWithFormat:@"%s", !sharedInstance()->lastPageName.empty() ? sharedInstance()->lastPageName.c_str() : "NoScene"]
                                                   action:[NSString stringWithFormat:@"%s", eventName.c_str()]
                                                    label:!label.empty() ? [NSString stringWithFormat:@"%s", label.c_str()] : NULL
                                                    value:[NSNumber numberWithInt:value]]
            set:@"start" forKey:kGAISessionControl]
           build]];
}

void AnalyticsWrapper::GAEndSession()
{
    [[[GAI sharedInstance] defaultTracker] set:kGAISessionControl value:@"end"];
}

void AnalyticsWrapper::firebaseLogPageView(const std::string& pageName){
    firebaseLogEventWithParameters("change_scene", DcreateP(Screate(pageName), Screate("item_name"),NULL));
}

void AnalyticsWrapper::firebaseLogEvent(const std::string& eventName) {
    firebaseLogEventWithParameters(eventName, Dcreate());
}

void AnalyticsWrapper::firebaseLogEventWithParameters(const std::string& eventName, cocos2d::CCDictionary * parameters) {
    [FIRAnalytics logEventWithName:[AnalyticXStringUtil nsstringFromCString:eventName.c_str()] parameters:[AnalyticXStringUtil nsDictionaryFromCCDictionary:parameters]];
}
