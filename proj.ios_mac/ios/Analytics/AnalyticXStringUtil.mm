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

#import "AnalyticXStringUtil.h"
#import "FenneX.h"
USING_NS_CC;
USING_NS_FENNEX;

@implementation AnalyticXStringUtil

+ (NSString *)nsstringFromCString:(const char *)cstring {
    if (cstring == NULL) {
        return NULL;
    }
    
    NSString * nsstring = [[NSString alloc] initWithBytes:cstring length:strlen(cstring) encoding:NSUTF8StringEncoding];
    return [nsstring autorelease];
}
+ (const char *)cstringFromNSString:(NSString *)nsstring {
    
    if (nsstring == NULL) {
        return NULL;
    }
    
    return [nsstring UTF8String];
}

@end