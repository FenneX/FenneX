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
#include "ImagePickerWrapper.h"
#include "platform/android/jni/JniHelper.h"
#include "DevicePermissions.h"

#define CLASS_NAME "com/fennex/modules/ImagePicker"

USING_NS_FENNEX;

void pickImageWithWidget()
{
    DevicePermissions::ensurePermission(Permission::STORAGE, [=](){
        JniMethodInfo minfo;
        bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pickImageWithWidget", "()V");
        CCAssert(functionExist, "Function doesn't exist");
        minfo.env->CallStaticVoidMethod(minfo.classID,
                                        minfo.methodID);
    }, [](){
        notifyImagePickCancelled();
    });
}

bool isCameraAvailable()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isCameraAvailable", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

extern "C"
{
    //extension for long name : __Ljava_lang_String_2Ljava_lang_String_2
    void Java_com_fennex_modules_ImagePicker_notifyImagePickedWrap(JNIEnv* env, jobject thiz, jstring path)
    {
        notifyImagePicked(JniHelper::jstring2string(path));
    }

    void Java_com_fennex_modules_ImagePicker_notifyImagePickCancelled(JNIEnv* env, jobject thiz)
    {
        notifyImagePickCancelled();
    }
}
