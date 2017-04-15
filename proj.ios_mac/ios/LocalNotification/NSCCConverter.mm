/****************************************************************************
 Copyright (c) 2013-2014 Auticiel SAS
 
 http://www.fennex.org
 
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

#import "NSCCConverter.h"
#include "Shorteners.h"

@implementation NSCCConverter

+ (NSDictionary *)nsDictionaryFromCCDictionary:(cocos2d::CCDictionary *)ccDictionary {
    if (ccDictionary == NULL) {
        return NULL;
    } else if (ccDictionary->allKeys() == NULL) {
        return NULL;
    } else if (ccDictionary->allKeys()->count() <= 0) {
        return NULL;
    }
    
    
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionaryWithCapacity:ccDictionary->allKeys()->count()];
    
    
    for (int i = 0; i < ccDictionary->allKeys()->count(); i++) {
        cocos2d::CCString* key = (cocos2d::CCString *)ccDictionary->allKeys()->objectAtIndex(i);
        cocos2d::Ref* obj = ccDictionary->objectForKey(key->getCString());
        NSObject* nsObject;
        if(isKindOfClass(obj, CCDictionary))
        {
            nsObject = @"Dictionary";
        }
        else if(isKindOfClass(obj, CCArray))
        {
            nsObject = @"Array";
        }
        else if (isKindOfClass(obj, CCString))
        {
            const char* cstring = ((CCString*)obj)->getCString();
            nsObject = [[[NSString alloc] initWithBytes:cstring length:strlen(cstring) encoding:NSUTF8StringEncoding] autorelease];
        }
        else if (isKindOfClass(obj, CCInteger))
        {
            nsObject = [NSNumber numberWithInt:TOINT(obj)];
        }
        else if (isKindOfClass(obj, CCFloat))
        {
            nsObject = [NSNumber numberWithFloat:TOFLOAT(obj)];
        }
        else if (isKindOfClass(obj, CCBool))
        {
            nsObject =  [NSNumber numberWithBool:TOBOOL(obj)];
        }
        else
        {
            nsObject = @"Unknown Object";
        }
        [nsDict setValue:nsObject forKey:[NSString stringWithFormat:@"%s", key->getCString()]];
    }
    
    return nsDict;
}


+ (cocos2d::CCDictionary *)ccDictionaryFromNSDictionary:(NSDictionary *)nsDictionary
{
    if (nsDictionary == nil) {
        return NULL;
    } else if ([nsDictionary allKeys] == NULL) {
        return NULL;
    } else if ([nsDictionary allKeys].count <= 0) {
        return NULL;
    }
    
    CCDictionary* ccDict = Dcreate();
    
    
    for (int i = 0; i < [nsDictionary allKeys].count; i++) {
        NSString* key = [[nsDictionary allKeys] objectAtIndex:i];
        NSObject* obj = [nsDictionary objectForKey:key];
        cocos2d::Ref* ccObject;
        if([obj isKindOfClass:[NSDictionary class]])
        {
            ccObject = Screate("Dictionary");
        }
        else if([obj isKindOfClass:[NSArray class]])
        {
            ccObject = Screate("Array");
        }
        else if ([obj isKindOfClass:[NSString class]])
        {
            ccObject = Screate([(NSString*)obj UTF8String]);
        }
        else if ([obj isKindOfClass:[NSNumber class]])
        {
            NSNumber* aNumber = (NSNumber*)obj;
            if((strcmp([aNumber objCType], @encode(int))) == 0) {
                ccObject = Icreate([aNumber intValue]);
            } else if((strcmp([aNumber objCType], @encode(float))) == 0) {
                ccObject = Fcreate([aNumber floatValue]);
            } else if((strcmp([aNumber objCType], @encode(BOOL))) == 0) {
                ccObject = Bcreate([aNumber boolValue]);
            } else {
                if([aNumber floatValue] - (int)[aNumber floatValue] < 0.00001) {   
                    ccObject = Icreate([aNumber intValue]);
                } else {
                    ccObject = Fcreate([aNumber floatValue]);                    
                }
            }
        }
        else
        {
            ccObject = Screate("Unknown Object");
        }
        ccDict->setObject(ccObject, [key UTF8String]);
    }
    
    return ccDict;
}

@end
