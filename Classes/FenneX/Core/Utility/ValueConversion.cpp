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

Value ValueConversion::fromStringVector(std::vector<std::string> vec)
{
    ValueVector val;
    for(std::string s : vec)
    {
        val.push_back(Value(s));
    }
    return Value(val);
}

std::vector<std::string> ValueConversion::toVectorString(Value val)
{
    if(val.isNull())
    {
        return {};
    }
    else if(val.getType() != Value::Type::VECTOR)
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

Value ValueConversion::fromStringStringMap(std::map<std::string, std::string> m)
{
    ValueMap val;
    for (auto const& x : m)
    {
        val[x.first] = Value(x.second);
    }
    return Value(val);
}

std::map<std::string, std::string> ValueConversion::toMapStringString(Value val)
{
    if(val.isNull())
    {
        return {};
    }
    else if(val.getType() != Value::Type::MAP)
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

Value ValueConversion::fromStringFloatMap(std::map<std::string, float> m)
{
    ValueMap val;
    for (auto const& x : m)
    {
        val[x.first] = Value(x.second);
    }
    return Value(val);
}

std::map<std::string, float> ValueConversion::toMapStringFloat(Value val)
{
    if(val.isNull())
    {
        return {};
    }
    else if(val.getType() != Value::Type::MAP)
    {
        throw std::bad_cast();
    }
    std::map<std::string, float> result;
    for(auto iter = val.asValueMap().begin(); iter != val.asValueMap().end(); iter++)
    {
        Value::Type type = iter->second.getType();
        if(type != Value::Type::FLOAT && type != Value::Type::DOUBLE)
        {
            throw std::bad_cast();
        }
        result[iter->first] = iter->second.asFloat();
    }
    return result;
}

Value ValueConversion::fromDaysVector(std::vector<struct tm> vec)
{
    ValueVector val;
    for(struct tm d : vec)
    {
        val.push_back(Value((int)mktime(&d)));
    }
    return Value(val);
}

std::vector<struct tm> ValueConversion::toDaysVector(Value val)
{
    std::vector<struct tm> vec;
    time_t timeVal;
    for(Value v : val.asValueVector())
    {
        timeVal = (time_t)v.asInt();
        vec.push_back(*localtime(&timeVal));
    }
    return vec;
}

Value ValueConversion::fromBoolVector(std::vector<bool> vec)
{
    ValueVector val;
    for(bool b : vec)
    {
        val.push_back(Value(b));
    }
    return Value(val);
}

std::vector<bool> ValueConversion::toBoolVector(Value val)
{
    std::vector<bool> vec;
    for(Value v : val.asValueVector())
    {
        vec.push_back(v.asBool());
    }
    return vec;
}

time_t ValueConversion::convertToTimeT(Value val, time_t defaultValue)
{
    return !val.isNull() && val.getType() == Value::Type::INTEGER ? val.asInt() :
           !val.isNull() && val.getType() == Value::Type::STRING ? std::atol(val.asString().c_str()) :
           !val.isNull() && val.getType() == Value::Type::FLOAT ? (time_t) val.asFloat() : defaultValue; // Float is a legacy from old saves
}

NS_FENNEX_END
