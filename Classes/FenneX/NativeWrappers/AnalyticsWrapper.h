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
    //Common usage methods
    static void logPageView(const std::string& pageName); //log a timed scene event + pageview in GA
    static void logEvent(const std::string& eventName, const std::string& label = "", int value = 0); // log eventName - log eventName with the Scene as a category in GA
    
    static void firebaseSetProperty(const std::string& propertyName, const std::string& propertyValue);
private:
    static void firebaseLogPageView(const std::string& pageName);
    static void firebaseLogEvent(const std::string& eventName);
    static void firebaseLogEventWithParameters(const std::string& eventName, const std::string& label = "", const std::string& value = "");
protected:
    //the variables can't be static, use a shared instance to access them instead
    static AnalyticsWrapper* sharedInstance();
    std::string lastPageName;
};

#endif 
