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

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "DropDownListWrapper.h"

USING_NS_FENNEX;

#define CLASS_NAME "com/fennex/modules/DropDownList"

DropDownListWrapper::DropDownListWrapper()
{
    JniMethodInfo minfo;
    
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "create", "()Lcom/fennex/modules/DropDownList;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jobject tmp = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    delegate = cocos2d::JniHelper::getEnv()->NewGlobalRef(tmp);
    minfo.env->DeleteLocalRef(minfo.classID);
}

DropDownListWrapper::~DropDownListWrapper()
{
    cocos2d::JniHelper::getEnv()->DeleteGlobalRef(delegate);
    delegate = NULL;
}

void DropDownListWrapper::setPossibleValues(std::vector<std::string> values)
{
    if(delegate != NULL)
    {
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getMethodInfo(minfo, CLASS_NAME, "setPossibleValues", "([Ljava/lang/String;)V");
        CCAssert(functionExist, "Function doesn't exist");
        
        jclass jStringCls = 0;
        
        jStringCls = minfo.env->FindClass("java/lang/String");
        if(minfo.env->ExceptionCheck())
        {
            minfo.env->ExceptionDescribe();
            log("crashed when looking for String Class");
            return ;
        }
        
        jobjectArray vector;
        
        vector = minfo.env->NewObjectArray(values.size(), jStringCls, NULL);
        minfo.env->DeleteLocalRef(jStringCls);
        
        if(minfo.env->ExceptionCheck())
        {
            minfo.env->ExceptionDescribe();
            log("crashed when creating array");
        }
        if (vector == NULL) {
            log("failed to create a new jobjectArray");
            return ;
        }
        
        for (int i = 0; i < values.size(); i++) {
            jstring objectString = minfo.env->NewStringUTF(values[i].c_str());
            minfo.env->SetObjectArrayElement(vector, i, objectString);
            minfo.env->DeleteLocalRef(objectString);
        }
        
        minfo.env->CallVoidMethod(delegate, minfo.methodID, vector);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(vector);
    }
}

void DropDownListWrapper::show()
{
    if(delegate != NULL)
    {
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getMethodInfo(minfo, CLASS_NAME, "show", "()V");
        CCAssert(functionExist, "Function doesn't exist");
        minfo.env->CallVoidMethod(delegate, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void DropDownListWrapper::setTitle(const std::string& title)
{
    if(delegate != NULL)
    {
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getMethodInfo(minfo, CLASS_NAME, "setTitle", "(Ljava/lang/String;)V");
        CCAssert(functionExist, "Function doesn't exist");
        jstring jtitle = minfo.env->NewStringUTF(title.c_str());
        minfo.env->CallVoidMethod(delegate, minfo.methodID, jtitle);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(jtitle);
    }
}

void DropDownListWrapper::setIdentifier(int identifier)
{
    if(delegate != NULL)
    {
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getMethodInfo(minfo, CLASS_NAME, "setIdentifier", "(I)V");
        CCAssert(functionExist, "Function doesn't exist");
        minfo.env->CallVoidMethod(delegate, minfo.methodID, (jint)identifier);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

extern "C"
{
    void Java_com_fennex_modules_DropDownList_notifySelectionDone(JNIEnv* env, jobject thiz, jint identifier, jstring value)
    {
        notifySelectionDone((int)identifier, JniHelper::jstring2string(value));
    }
}
