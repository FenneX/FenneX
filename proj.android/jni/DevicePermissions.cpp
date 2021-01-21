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
#include "cocos2d.h"
#include "FenneX.h"
#include "DevicePermissions.h"

#define CLASS_NAME "com/fennex/modules/DevicePermissions"

USING_NS_CC;

NS_FENNEX_BEGIN

bool DevicePermissions::hasPermissionInternal(Permission permission)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "hasPermission", "(I)Z");
    CCAssert(functionExist, "Function doesn't exist");

    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, (jint)(int)permission);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool DevicePermissions::hasPermissionInternal(const std::string& permission)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "hasPermission", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPermission = minfo.env->NewStringUTF(permission.c_str());

    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, jPermission);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPermission);
    return result;
}

bool DevicePermissions::requestPermission(Permission permission)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "requestPermission", "(I)Z");
    CCAssert(functionExist, "Function doesn't exist");
    
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, (jint)(int)permission);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool DevicePermissions::requestPermission(const std::string& permission)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "requestPermission", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPermission = minfo.env->NewStringUTF(permission.c_str());

    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, jPermission);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPermission);
    return result;
}


NS_FENNEX_END

extern "C"
{
    void Java_com_fennex_modules_DevicePermissions_notifyPermissionRequestEnded(JNIEnv* env, jobject thiz, jint permission, jboolean result)
    {
        DevicePermissions::permissionRequestEnded((Permission)(int)permission, (bool)result);
    }

    void Java_com_fennex_modules_DevicePermissions_notifyCustomPermissionRequestEnded(JNIEnv* env, jobject thiz, jstring permission, jboolean result)
    {
        DevicePermissions::permissionRequestEnded(JniHelper::jstring2string(permission), (bool)result);
    }
}
