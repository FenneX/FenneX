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
#include "CameraHandlerWrapper.h"
#include "DevicePermissions.h"

#define CLASS_NAME "com/fennex/modules/CameraHandler"

void startCameraPreview(Vec2 position, cocos2d::Size size, bool front, bool requiresAudio)
{
    DevicePermissions::ensurePermission(Permission::CAMERA, [=](){
        std::function<void(void)> startJavaRecord = [=]() {
            JniMethodInfo minfo;
            bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "startCameraPreview", "(FFFFZ)V");
            CCAssert(functionExist, "Function doesn't exist");
            minfo.env->CallStaticVoidMethod(
                                            minfo.classID,
                                            minfo.methodID,
                                            (jfloat)position.x,
                                            (jfloat)position.y,
                                            (jfloat)size.width,
                                            (jfloat)size.height,
                                            (jboolean)front);
            minfo.env->DeleteLocalRef(minfo.classID);
        };
        if(requiresAudio) {
            DevicePermissions::ensurePermission(Permission::MICROPHONE, [=](){
                startJavaRecord();
            }, [=](){
                notifyRecordingCancelled();
            });
        }
        else {
            startJavaRecord();
        }
    }, [](){
        notifyRecordingCancelled();
    });
}

void stopCameraPreview()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "stopCameraPreview", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool canSwitchCamera()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "canSwitchCamera", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void startVideoRecording()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "startRecording", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void stopVideoRecording()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"stopRecording", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void stopVideoRecordingSaveTo(std::string path, FileLocation location)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"stopRecordingSaveTo", "(Ljava/lang/String;I)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPath = minfo.env->NewStringUTF(path.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jPath, (jint)location);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPath);
}

bool cancelRecording(bool notify)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "cancelRecording", "(Z)Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, (jboolean)notify);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void capturePicture()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "capturePicture", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID);
}

void switchCamera()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "switchCamera", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

extern "C"
{
    void Java_com_fennex_modules_CameraHandler_notifyPictureTaken(JNIEnv* env, jobject thiz, jstring path)
    {
        notifyPictureTaken(JniHelper::jstring2string(path));
    }

    void Java_com_fennex_modules_CameraHandler_notifyRecordingCancelled(JNIEnv* env, jobject thiz)
    {
        notifyRecordingCancelled();
    }

    void Java_com_fennex_modules_CameraHandler_notifyCameraSwitched(JNIEnv* env, jobject thiz)
    {
        notifyCameraSwitched();
    }
}
