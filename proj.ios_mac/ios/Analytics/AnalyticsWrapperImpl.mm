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
#import "Firebase.h"

void AnalyticsWrapper::firebaseSetProperty(const std::string& propertyName, const std::string& propertyValue)
{
    [FIRAnalytics setUserPropertyString:[NSString stringWithFormat:@"%s", propertyValue.c_str()] forName:[NSString stringWithFormat:@"%s", propertyName.c_str()]];
}

std::string replaceEventChars(const std::string& eventName)
{
    std::string event = eventName;
    std::replace(event.begin(), event.end(), ' ', '_');
    std::replace(event.begin(), event.end(), '-', '_');
    return event;
}

void AnalyticsWrapper::firebaseLogPageView(const std::string& pageName){
    firebaseLogEventWithParameters("change_scene", "item_name", pageName);
}

void AnalyticsWrapper::firebaseLogEvent(const std::string& eventName) {
    std::string event = replaceEventChars(eventName);
    [FIRAnalytics logEventWithName:[AnalyticXStringUtil nsstringFromCString:event.c_str()] parameters:nil];
}

void AnalyticsWrapper::firebaseLogEventWithParameters(const std::string& eventName, const std::string& label, const std::string& value)
{
    std::string event = replaceEventChars(eventName);
    NSDictionary* params = [NSDictionary dictionaryWithObject:[AnalyticXStringUtil nsstringFromCString:value.c_str()] forKey:[AnalyticXStringUtil nsstringFromCString:label.c_str()]];
    [FIRAnalytics logEventWithName:[AnalyticXStringUtil nsstringFromCString:event.c_str()] parameters:params];
}
