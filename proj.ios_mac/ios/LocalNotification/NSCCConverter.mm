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

using namespace cocos2d;

@implementation NSCCConverter

+ (NSDictionary *)nsDictionaryFromValueMap:(ValueMap)map {
    if (map.size()>0) {
        return NULL;
    }
    
    
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionaryWithCapacity:map.size()];
    for(ValueMap::iterator it = map.begin(); it != map.end(); ++it) {
        NSObject* nsObject;
        Value obj = it->second;
        if(obj.getType() == Value::Type::MAP)
        {
            nsObject = @"Dictionary";
        }
        else if(obj.getType() == Value::Type::VECTOR)
        {
            nsObject = @"Array";
        }
        else if(obj.getType() == Value::Type::STRING)
        {
            const char* cstring = obj.asString().c_str();
            nsObject = [[[NSString alloc] initWithBytes:cstring length:strlen(cstring) encoding:NSUTF8StringEncoding] autorelease];
        }
        else if(obj.getType() == Value::Type::INTEGER)
        {
            nsObject = [NSNumber numberWithInt:obj.asInt()];
        }
        else if(obj.getType() == Value::Type::FLOAT)
        {
            nsObject = [NSNumber numberWithFloat:obj.asFloat()];
        }
        else if(obj.getType() == Value::Type::DOUBLE)
        {
            nsObject = [NSNumber numberWithFloat:obj.asDouble()];
        }
        else if(obj.getType() == Value::Type::BOOLEAN)
        {
            nsObject =  [NSNumber numberWithBool:obj.asBool()];
        }
        else
        {
            nsObject = @"Unknown Object";
        }
        [nsDict setValue:nsObject forKey:[NSString stringWithFormat:@"%s", it->first.c_str()]];
    }
    
    return nsDict;
}


+ (ValueMap)valueMapFromNSDictionary:(NSDictionary *)nsDictionary
{
    if (nsDictionary == nil) {
        return ValueMap();
    } else if ([nsDictionary allKeys] == NULL) {
        return ValueMap();
    } else if ([nsDictionary allKeys].count <= 0) {
        return ValueMap();
    }
    
    ValueMap map = ValueMap();
    
    
    for (int i = 0; i < [nsDictionary allKeys].count; i++) {
        NSString* key = [[nsDictionary allKeys] objectAtIndex:i];
        NSObject* obj = [nsDictionary objectForKey:key];
        Value val;
        if([obj isKindOfClass:[NSDictionary class]])
        {
            val = Value("Dictionary");
        }
        else if([obj isKindOfClass:[NSArray class]])
        {
            val = Value("Array");
        }
        else if ([obj isKindOfClass:[NSString class]])
        {
            val = Value([(NSString*)obj UTF8String]);
        }
        else if ([obj isKindOfClass:[NSNumber class]])
        {
            NSNumber* aNumber = (NSNumber*)obj;
            if((strcmp([aNumber objCType], @encode(int))) == 0) {
                val = Value([aNumber intValue]);
            } else if((strcmp([aNumber objCType], @encode(float))) == 0) {
                val = Value([aNumber floatValue]);
            } else if((strcmp([aNumber objCType], @encode(BOOL))) == 0) {
                val = Value([aNumber boolValue]);
            } else {
                if([aNumber floatValue] - (int)[aNumber floatValue] < 0.00001) {   
                    val = Value([aNumber intValue]);
                } else {
                    val = Value([aNumber floatValue]);
                }
            }
        }
        else
        {
            val = Value("Unknown Object");
        }
        map.insert({[key UTF8String], val});
    }
    
    return map;
}

@end
