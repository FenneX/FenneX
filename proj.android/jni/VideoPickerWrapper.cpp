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
#include "VideoPickerWrapper.h"
#include "DevicePermissions.h"

#define CLASS_NAME "com/fennex/modules/VideoPicker"

void pickVideoFromLibrary(const std::string& saveName, FileLocation location)
{
    DevicePermissions::ensurePermission(Permission::STORAGE, [=](){
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "pickVideoFromLibrary", "(Ljava/lang/String;I)V");
        CCAssert(functionExist, "Function doesn't exist");
        jstring jSaveName = minfo.env->NewStringUTF(saveName.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jSaveName, (jint)location);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(jSaveName);
    }, [](){
        notifyVideoPickCancelled();
    });
}

void pickVideoFromCamera(const std::string& saveName, FileLocation location)
{
    DevicePermissions::ensurePermission(Permission::CAMERA, [=](){
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "pickVideoFromCamera", "(Ljava/lang/String;I)V");
        CCAssert(functionExist, "Function doesn't exist");
        jstring jSaveName = minfo.env->NewStringUTF(saveName.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jSaveName, (jint)location);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(jSaveName);
    }, [](){
        notifyRecordingCancelled();
    });
}

void getAllVideos()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getAllVideos", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

extern "C"
{
    //extension for long name : __Ljava_lang_String_2Ljava_lang_String_2
    void Java_com_fennex_modules_VideoPicker_notifyVideoPickedWrap(JNIEnv* env, jobject thiz, jstring name, jint location)
    {
        notifyVideoPicked(JniHelper::jstring2string(name), (FileLocation)location);
    }
    
    void Java_com_fennex_modules_VideoPicker_notifyVideoFound(JNIEnv* env, jobject thiz, jstring name)
    {
        notifyVideoFound(JniHelper::jstring2string(name));
    }
    
    void Java_com_fennex_modules_VideoPicker_notifyVideoName(JNIEnv* env, jobject thiz, jstring path, jstring name)
    {
        notifyVideoName(JniHelper::jstring2string(path), JniHelper::jstring2string(name));
    }
    
    void Java_com_fennex_modules_VideoPicker_notifyGetAllVideosFinished(JNIEnv* env, jobject thiz)
    {
        notifyGetAllVideosFinished();
    }

    void Java_com_fennex_modules_VideoPicker_notifyVideoPickCancelled(JNIEnv* env, jobject thiz)
    {
        notifyVideoPickCancelled();
    }
}
