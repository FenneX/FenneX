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

#import <Foundation/Foundation.h>

@interface NSString (RangeOfCharacters)

/* note "Characters" is plural in the methods. It has poor readability, hard to
 * distinguish from the rangeOfCharacterFromSet: methods, but it's standard Apple
 * convention.
 */
-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet;
-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask;
-(NSRange)rangeOfCharactersFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask range:(NSRange)range;

// like the above, but return a string rather than a range
-(NSString*)substringFromSet:(NSCharacterSet*)aSet;
-(NSString*)substringFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask;
-(NSString*)substringFromSet:(NSCharacterSet*)aSet options:(NSStringCompareOptions)mask range:(NSRange)range;

@end
