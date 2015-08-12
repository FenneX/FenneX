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

#ifndef FenneX__AnalyticsWrapper__
#define FenneX__AnalyticsWrapper__

#include "FenneX.h"

USING_NS_FENNEX;

class AnalyticsWrapper
{
public:
    static void init();
    //Start methods and agent version, specific to the platform since the token is different
    static void GAStartSession(const std::string& apiKey);
    static std::string GAGetGAAgentVersion();
    static void flurryStartSession(const std::string& apiKey);
    static std::string flurryGetFlurryAgentVersion();
    
    //Common methods
    //Call those methods before start session of each tracker
    static void setAppVersion(const std::string& version);
    
    //Call those methods after start session of each tracker
    static void setDebugLogEnabled(bool value);
    static void setSecureTransportEnabled(bool value);
    
    //Common usage methods
    static void logPageView(const std::string& pageName); //log a timed scene event + pageview in flurry, pageview in GA
    static void logEvent(const std::string& eventName, const std::string& label = "", int value = 0); // log eventName - Scene in Flurry, log eventName with the Scene as a category in GA
    static void endSession();
    
private:
    //GA only methods
    
    //GA tracker methods (require start session)
    static void GASetAppVersion(const std::string& version);
    static void GASetSecureTransportEnabled(bool value);
    
    //GA shared methods (can be called before start session)
    static void GASetDebugLogEnabled(bool value);
    static void GASetTrackExceptionsEnabled(bool value);
    
    //Common usage methods
    static void GALogPageView(const std::string& pageName);
    static void GALogEvent(const std::string& eventName, const std::string& label = "", int value = 0); //The category is the current scene
    static void GAEndSession();
    
    //Flurry only methods
    static void flurrySetAppVersion(const std::string& version);
    static void flurrySetDebugLogEnabled(bool value);// default is NO
    static void flurrySetSecureTransportEnabled(bool value);// set data to be sent over SSL, default is NO
    
    
    //Common usage methods
    static void flurryLogPageView(); //Only count the number on Flurry, the time and name is tracked via timed events
    static void flurryLogEvent(const std::string& eventName);
    static void flurryLogEventWithParameters(const std::string& eventName, cocos2d::CCDictionary * parameters);
    
    //Required to keep track of time on scenes
    static void flurryLogEventTimed(const std::string& eventName, bool timed);
    static void flurryLogEventWithParametersTimed(const std::string& eventName, cocos2d::CCDictionary * parameters, bool timed);//On Android, the *parameters* will be ignored
    static void flurryEndTimedEventWithParameters(const std::string& eventName, cocos2d::CCDictionary * parameters); // non-nil parameters will update the parameters
    
    static void flurryEndSession();
    
protected:
    //the variables can't be static, use a shared instance to access them instead
    static AnalyticsWrapper* sharedInstance();
    //For Flurry, you must set AppVersion  BEFORE starting the session
    //For GA, you must set it AFTER starting the version
    //To go around that, AnalyticsWrapper requires to set them before starting the session, and will call GA methods after the start session
    std::string appVersion;
    std::string lastPageName;
};

#endif 
