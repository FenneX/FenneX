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

#ifndef __FenneX__ValueConversion_h__
#define __FenneX__ValueConversion_h__

#include "cocos2d.h"
#include "FenneXMacros.h"
USING_NS_CC;

NS_FENNEX_BEGIN

/*
 Helper to convert value to and from native types.
 Can throw std::bad_cast if the Value is not the right type
 */
class ValueConversion
{
public:
    static std::vector<std::string> toVectorString(Value val);
    static std::map<std::string, std::string> toMapStringString(Value val);
    static Value fromDaysVector(std::vector<struct tm> vec);
    static std::vector<struct tm> toDaysVector(Value val);
    static Value fromBoolVector(std::vector<bool> vec);
    static std::vector<bool> toBoolVector(Value val);
};

NS_FENNEX_END

#endif /* __FenneX__ValueConversion_h__ */
