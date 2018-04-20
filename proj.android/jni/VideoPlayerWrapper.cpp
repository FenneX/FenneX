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
#include "VideoPlayer.h"

#define CLASS_NAME "com/fennex/modules/VideoPlayer"

VideoPlayer::VideoPlayer(std::string file, Vec2 position, cocos2d::Size size, bool front, bool loop)
{
    JniMethodInfo minfo;
    
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"initVideoPlayer", "(Ljava/lang/String;FFFFZZ)V");
    CCAssert(functionExist, "Function doesn't exist");
    
    log("VideoPlayer: file = %s, position.x = %f, position.y = %f, size.height = %f, position.width = %f, front = %i", file.c_str(), position.x, position.y, size.height, size.width, front);
    jstring jFile = minfo.env->NewStringUTF(file.c_str());
    
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID,
                                    jFile,
                                    (jfloat) position.x,
                                    (jfloat) position.y,
                                    (jfloat) size.height,
                                    (jfloat) size.width,
                                    (jboolean) front,
                                    (jboolean) loop);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jFile);
}

VideoPlayer::~VideoPlayer()
{
    delegate = NULL;
}

void VideoPlayer::setUseVLC(bool useVLC)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setUseVLC", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)useVLC);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::setPlayerPosition(Vec2 position, cocos2d::Size size, bool animated)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPlayerPosition", "(FFFFZ)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID,
                                    (jfloat) position.x,
                                    (jfloat) position.y,
                                    (jfloat) size.height,
                                    (jfloat) size.width,
                                    (jboolean) animated);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::play()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"play", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::pause()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pause", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::stop()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stop", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float VideoPlayer::getPlaybackRate()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getPlaybackRate", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void VideoPlayer::setPlaybackRate(float rate)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPlaybackRate", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)rate);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::setHideOnPause(bool hide)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setHideOnPause", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)hide);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::setFullscreen(bool fullscreen, bool animated)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setFullscreen", "(ZZ)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)fullscreen, (jboolean)animated);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool VideoPlayer::isFullscreen()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isFullscreen", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float VideoPlayer::getDuration()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getDuration", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float VideoPlayer::getPosition()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getPosition", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void VideoPlayer::setPosition(float position)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPosition", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)position);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::setMuted(bool muted)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setMuted", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)muted);
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string VideoPlayer::getThumbnail(const std::string& path, FileLocation videoLocation, const std::string& thumbnailPath, FileLocation thumbnailLocation)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getThumbnail", "(Ljava/lang/String;ILjava/lang/String;I)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring stringArg = minfo.env->NewStringUTF(path.c_str());
    jstring thumbnailStringArg = minfo.env->NewStringUTF(thumbnailPath.c_str());
    jstring result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, stringArg, (jint)videoLocation, thumbnailStringArg, (jint)thumbnailLocation);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(stringArg);
    minfo.env->DeleteLocalRef(thumbnailStringArg);
    
    std::string thumbnailPathRet = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(result);
    
    return thumbnailPathRet;
}

cocos2d::Size VideoPlayer::getVideoSize(const std::string& path, FileLocation location)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getVideoSize", "(Ljava/lang/String;I)[F");
    CCAssert(functionExist, "Function doesn't exist");
    log("path is: %s", path.c_str());
    jstring stringArg = minfo.env->NewStringUTF(path.c_str());
    jfloatArray result = (jfloatArray)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, stringArg, (jint)location);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(stringArg);
    CCAssert(minfo.env->GetArrayLength(result) == 2, "getVideoSize: result should have 2 values");
    cocos2d::Size size;

    jfloat* array = minfo.env->GetFloatArrayElements(result, 0);
    size.width = array[0];
    size.height = array[1];
    minfo.env->ReleaseFloatArrayElements(result, array, 0);
    minfo.env->DeleteLocalRef(result);
    return size;
}

bool VideoPlayer::videoExists(const std::string& file)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"videoExists", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring stringArg = minfo.env->NewStringUTF(file.c_str());
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, stringArg);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(stringArg);
    
    return result;
}

extern "C"
{
    void Java_com_fennex_modules_VideoPlayer_notifyVideoDurationAvailable(JNIEnv* env, jobject thiz, jstring path, jfloat duration)
    {
        notifyVideoDurationAvailable(JniHelper::jstring2string(path), (float)duration);
    }
    void Java_com_fennex_modules_VideoPlayer_notifyVideoEnded(JNIEnv* env, jobject thiz, jstring path)
    {
        notifyVideoEnded(JniHelper::jstring2string(path));
    }
    void Java_com_fennex_modules_VideoPlayer_notifyVideoError(JNIEnv* env, jobject thiz, jstring path)
    {
        notifyVideoError(JniHelper::jstring2string(path));
    }
}
