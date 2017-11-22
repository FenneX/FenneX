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

#include "CCJniConversion.h"

USING_NS_FENNEX;

#define VERBOSE_JNI_CONVERSION 0

jobjectArray jobjectArrayFromMap(JNIEnv *pEnv, ValueMap map)
{
#if VERBOSE_JNI_CONVERSION
    log("converting ValueMap to native array ....");
#endif
    if (map.size() <= 0)
    {
        return NULL;
    }

    jclass jStringCls = 0;

    jStringCls = pEnv->FindClass("java/lang/String");
    if(pEnv->ExceptionCheck())
    {
        pEnv->ExceptionDescribe();
        log("crashed when looking for String Class");
        return NULL;
    }

    jobjectArray result;

    result = pEnv->NewObjectArray( 2 * map.size(), jStringCls, NULL);
    pEnv->DeleteLocalRef(jStringCls);

    if(pEnv->ExceptionCheck())
    {
        pEnv->ExceptionDescribe();
        log("crashed when creating array");
    }
    if (result == NULL) {
        log("failed to create a new jobjectArray");
        return NULL;
    }
    int i = 0;
    for(const auto& obj : map)
    {
        std::string objToString;
        if(isValueOfType(obj.second, MAP))
        {
            objToString = "Dictionary";
        }
        else if(isValueOfType(obj.second, VECTOR))
        {
            objToString = "Array";
        }
        else if (isValueOfType(obj.second, STRING))
        {
            objToString = obj.second.asString();
        }
        else if (isValueOfType(obj.second, INTEGER))
        {
            objToString = std::to_string(obj.second.asInt());
        }
        else if (isValueOfType(obj.second, FLOAT))
        {
            objToString = std::to_string(obj.second.asFloat());
        }
        else if (isValueOfType(obj.second, DOUBLE))
        {
            objToString = std::to_string(obj.second.asDouble());
        }
        else if (isValueOfType(obj.second, BOOLEAN))
        {
            objToString = obj.second.asBool() ? "true" : "false";
        }

        jstring keyString = pEnv->NewStringUTF(obj.first.c_str());

        jstring objectString = pEnv->NewStringUTF(objToString.c_str());

#if VERBOSE_JNI_CONVERSION
        log("%s", obj.first.c_str());
#endif
        pEnv->SetObjectArrayElement(result, i * 2, keyString);
        pEnv->DeleteLocalRef(keyString);
#if VERBOSE_JNI_CONVERSION
        log("%s", objToString.c_str());
#endif
        pEnv->SetObjectArrayElement(result, i * 2 + 1, objectString);
        pEnv->DeleteLocalRef(objectString);
        i++;
    }
#if VERBOSE_JNI_CONVERSION
    log("Converted!");
#endif
    return result;
}

jobjectArray jobjectArrayFromStringVector(JNIEnv *pEnv, std::vector<std::string> vector)
{
#if VERBOSE_JNI_CONVERSION
    log("converting std::vector<std::string> to native array ....");
#endif
    if (vector.size() <= 0) {
        return NULL;
    }
    
    jclass jStringCls = 0;
    
    jStringCls = pEnv->FindClass("java/lang/String");
    if(pEnv->ExceptionCheck())
    {
        pEnv->ExceptionDescribe();
        log("crashed when looking for String Class");
        return NULL;
    }
    
    jobjectArray result;
    
    result = pEnv->NewObjectArray( vector.size(), jStringCls, NULL);
    pEnv->DeleteLocalRef(jStringCls);
    
    if(pEnv->ExceptionCheck())
    {
        pEnv->ExceptionDescribe();
        log("crashed when creating array");
    }
    if (result == NULL) {
        log("failed to create a new jobjectArray");
        return NULL;
    }
    
    int i = 0;
    for (std::string str : vector) {
        jstring objectString = pEnv->NewStringUTF(str.c_str());
#if VERBOSE_JNI_CONVERSION
        log("%s", str.c_str());
#endif
        pEnv->SetObjectArrayElement(result, i, objectString);
        pEnv->DeleteLocalRef(objectString);
        i++;
    }
#if VERBOSE_JNI_CONVERSION
    log("Converted!");
#endif
    return result;
}

typedef enum
{
    IntegerType,
    FloatType,
    BooleanType,
    StringType,
    NoType
}ConvertTypeInfo;

Value ValueFromString(std::string string)
{
    Value result;
    ConvertTypeInfo resolvedType = NoType;
    bool found = false;
    if(string.length() > 5)
    {
        found = true;
        if(strncmp(string.c_str(), "[Str]", 5) == 0)
        {
            resolvedType = StringType;
        }
        else if(strncmp(string.c_str(), "[Int]", 5) == 0)
        {
            resolvedType = IntegerType;
        }
        else if(strncmp(string.c_str(), "[Flo]", 5) == 0)
        {
            resolvedType = FloatType;
        }
        else if(strncmp(string.c_str(), "[Boo]", 5) == 0)
        {
            resolvedType = BooleanType;
        }
        else
        {
            found = false;
        }
        if(found)
        {
            string = string.substr(5, std::string::npos);
        }
    }
    if(resolvedType == BooleanType || (resolvedType == NoType &&  (string == "true" || string == "false")))
    {
        result = Value(string == "true");
    }
    else if(resolvedType == IntegerType || (resolvedType == NoType &&  (atoi(string.c_str()) != 0 || string == "0")))
    {
        result = Value(atoi(string.c_str()));
    }
    else if(resolvedType == FloatType || (resolvedType == NoType && atof(string.c_str()) != 0))
    {
        result = Value(atof(string.c_str()));
    }
    else
    {
        result = Value(string);
    }
    return result;
}

ValueMap MapFromjobjectArray(JNIEnv *pEnv, jobjectArray array)
{
#if VERBOSE_JNI_CONVERSION
    log("Converting jobjectArray to ValueMap ....");
#endif
    jsize count = 0;
    
    ValueMap result;
    
    if(array == NULL)
    {
        log("jobjectArray is null.");
        return result;
    }
    count = pEnv->GetArrayLength(array);
    if(count <= 0)
    {
        log("jobjectArray is empty.");
        return result;
    }
    else if(count % 2 != 0)
    {
        log("The count is not even, last object will be left out");
        count--;
    }
    
    for(int i = 0; i < count; i += 2)
    {
        //Run through the array, retrieve each type and set it in a CCDictionary
        jobject element = pEnv->GetObjectArrayElement(array, i);
        if(element != NULL)
        {
            jobject nextObjectElement = pEnv->GetObjectArrayElement(array, i+1);
            result.insert({
                JniHelper::jstring2string((jstring)element),
                ValueFromString(JniHelper::jstring2string((jstring)nextObjectElement))
            });
            pEnv->DeleteLocalRef(nextObjectElement);
            pEnv->DeleteLocalRef(element);
        }
    }
#if VERBOSE_JNI_CONVERSION
    log("Converted!");
#endif
    return result;
}

std::vector<std::string> StringVectorFromjobjectArray(JNIEnv *pEnv, jobjectArray array)
{
#if VERBOSE_JNI_CONVERSION
    log("Converting jobjectArray to std::vector<std::string> ....");
#endif
    jsize count = 0;
    std::vector<std::string> result;
    
    if(array == NULL)
    {
        log("jobjectArray is null.");
        return result;
    }
    count = pEnv->GetArrayLength(array);
    if(count <= 0) {
        log("jobjectArray is empty.");
        return result;
    }
    
    jboolean flag = false;
    
    for(int i = 0;i < count;i++)
    {
        //Run through the array, retrieve each type and set it in a CCArray
        jobject element = pEnv->GetObjectArrayElement(array, i);
        result.push_back(JniHelper::jstring2string((jstring)element));
        pEnv->DeleteLocalRef(element);
    }
#if VERBOSE_JNI_CONVERSION
    log("Converted!");
#endif
    return result;
}

