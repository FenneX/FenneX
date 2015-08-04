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

#include "Logs.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "VideoPlayer.h"

#define CLASS_NAME "com/fennex/modules/VideoPlayer"

VideoPlayer::VideoPlayer(std::string file, CCPoint position, CCSize size, bool front, bool loop)
{
	JniMethodInfo minfo;

    CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"initVideoPlayer", "(Ljava/lang/String;FFFFZZ)V"), "Function doesn't exist");

	CCLOG("VideoPlayer: file = %s, position.x = %f, position.y = %f, size.height = %f, position.width = %f, front = %i", file.c_str(), position.x, position.y, size.height, size.width, front);
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
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setUseVLC", "(Z)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)useVLC);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::play()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"play", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::pause()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pause", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::stop()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stop", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float VideoPlayer::getPlaybackRate()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getPlaybackRate", "()F"), "Function doesn't exist");
	float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
	return result;
}

void VideoPlayer::setPlaybackRate(float rate)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPlaybackRate", "(F)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)rate);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void VideoPlayer::setHideOnPause(bool hide)
{
#warning TODO : not implemented
}

void VideoPlayer::setFullscreen(bool fullscreen, bool animated)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setFullscreen", "(ZZ)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)fullscreen, (jboolean)animated);
    minfo.env->DeleteLocalRef(minfo.classID);
}

bool VideoPlayer::isFullscreen()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isFullscreen", "()Z"), "Function doesn't exist");
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float VideoPlayer::getDuration()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getDuration", "()F"), "Function doesn't exist");
	float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

float VideoPlayer::getPosition()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getPosition", "()F"), "Function doesn't exist");
	float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void VideoPlayer::setPosition(float position)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPosition", "(F)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)position);
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string VideoPlayer::getThumbnail(const std::string& path)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getThumbnail", "(Ljava/lang/String;)Ljava/lang/String;"), "Function doesn't exist");

	jstring stringArg = minfo.env->NewStringUTF(path.c_str());
	jstring result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, stringArg);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg);

    std::string thumbnailPath = JniHelper::jstring2string(result);
    minfo.env->DeleteLocalRef(result);

	return thumbnailPath;
}

bool VideoPlayer::isValidVideo(const std::string& filePath)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isValidVideo", "(Ljava/lang/String;)Z"), "Function doesn't exist");

	jstring stringArg = minfo.env->NewStringUTF(filePath.c_str());
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, stringArg);
	minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(stringArg);

	return result;
}

bool VideoPlayer::videoExists(const std::string& file)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"videoExists", "(Ljava/lang/String;)Z"), "Function doesn't exist");

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
