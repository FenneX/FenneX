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

#import "NSString+RangeOfCharacters.h"

@implementation NSString (RangeOfCharacters)
-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet {
    return [self rangeOfCharactersFromSet:aSet options:0];
}

-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask {
    NSRange range = {0,[self length]};
    return [self rangeOfCharactersFromSet:aSet options:mask range:range];
}

-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask range:(NSRange)range {
    NSInteger start, curr, end, step=1;
    if (mask & NSBackwardsSearch) {
        step = -1;
        start = range.location + range.length - 1;
        end = range.location-1;
    } else {
        start = range.location;
        end = start + range.length;
    }
    if (!(mask & NSAnchoredSearch)) {
        // find first character in set
        for (;start != end; start += step) {
            if ([aSet characterIsMember:[self characterAtIndex:start]]) {
#ifdef NOGOTO
                break;
#else
                // Yeah, a goto. If you don't like them, define NOGOTO.
                // Method will work the same, it will just make unneeded
                // test whether character at start is in aSet
                goto FoundMember;
#endif
            }
        }
#ifndef NOGOTO
        goto NoSuchMember;
#endif
    }
    if (![aSet characterIsMember:[self characterAtIndex:start]]) {
    NoSuchMember:
        // no characters found within given range
        range.location = NSNotFound;
        range.length = 0;
        return range;
    }
    
FoundMember:
    for (curr = start; curr != end; curr += step) {
        if (![aSet characterIsMember:[self characterAtIndex:curr]]) {
            break;
        }
    }
    if (curr < start) {
        // search was backwards
        range.location = curr+1;
        range.length = start - curr;
    } else {
        range.location = start;
        range.length = curr - start;
    }
    return range;
}

-(NSString*)substringFromSet:(NSCharacterSet*)aSet {
    return [self substringFromSet:aSet options:0];
}

-(NSString*)substringFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask  {
    NSRange range = {0,[self length]};
    return [self substringFromSet:aSet options:mask range:range];
}
-(NSString*)substringFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask range:(NSRange)range {
    range = [self rangeOfCharactersFromSet:aSet options:mask range:range];
    if (NSNotFound == range.location) {
        return nil;
    }
    return [self substringWithRange:range];
}

@end
