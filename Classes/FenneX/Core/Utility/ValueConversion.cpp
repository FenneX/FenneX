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

#include "ValueConversion.h"

NS_FENNEX_BEGIN

std::vector<std::string> ValueConversion::toVectorString(Value val)
{
    if(val.getType() != Value::Type::VECTOR)
    {
        throw std::bad_cast();
    }
    std::vector<std::string> result;
    for(Value strVal : val.asValueVector())
    {
        if(strVal.getType() != Value::Type::STRING)
        {
            throw std::bad_cast();
        }
        result.push_back(strVal.asString());
    }
    return result;
}

std::map<std::string, std::string> ValueConversion::toMapStringString(Value val)
{
    if(val.getType() != Value::Type::MAP)
    {
        throw std::bad_cast();
    }
    std::map<std::string, std::string> result;
    for(auto iter = val.asValueMap().begin(); iter != val.asValueMap().end(); iter++)
    {
        if(iter->second.getType() != Value::Type::STRING)
        {
            throw std::bad_cast();
        }
        result[iter->first] = iter->second.asString();
    }
    return result;
}

NS_FENNEX_END
