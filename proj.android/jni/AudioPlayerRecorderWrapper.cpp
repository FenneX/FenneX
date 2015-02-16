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
#include "FenneX.h"
#include "AudioPlayerRecorder.h"
#include "platform/android/jni/JniHelper.h"

USING_NS_FENNEX;

#define  CLASS_NAME "com/fennex/modules/AudioPlayerRecorder"

//Cache sounds duration here because a call to Java getSoundDuration requires a MediaPlayer prepare, which is slow
CCDictionary* soundsDuration = NULL;


void AudioPlayerRecorder::init()
{
#warning not implemented
}

bool AudioPlayerRecorder::isRecording()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isRecording", "()Z"), "Function doesn't exist");
	bool result = (link != NULL && minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID));
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool AudioPlayerRecorder::isPlaying()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isPlaying", "()Z"), "Function doesn't exist");
	bool result =  (link != NULL && minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID));
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}


void AudioPlayerRecorder::record(const std::string& file, CCObject* linkTo)
{
	JniMethodInfo minfo;
	std::string withExtension = file + ".3gp";

    if(linkTo == link && this->isRecording())
    {
        this->stopRecording();
    }
    else
    {
        if(linkTo == NULL)
        {
            linkTo = noLinkObject;
        }
        this->setLink(linkTo);
        this->setPath(withExtension);

    	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"startRecording", "(Ljava/lang/String;)V"), "Function doesn't exist");

    	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(withExtension.c_str()));
    	minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AudioPlayerRecorder::stopRecording()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stopRecording", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

float AudioPlayerRecorder::play(const std::string& file, CCObject* linkTo, bool independent)
{
	JniMethodInfo minfo;

	if(independent)
	{
		CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"playIndependent", "(Ljava/lang/String;)V"), "Function doesn't exist");
    	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,minfo.env->NewStringUTF(file.c_str()));
    	minfo.env->DeleteLocalRef(minfo.classID);
	}
	else
	{
	    if(linkTo == link && this->isPlaying())
	    {
	        this->stopPlaying();
	        CCLOG(" END AudioPlayerRecorder::play");
	        return 0;
	    }
	    else
	    {
	    	if(linkTo == NULL)
	    	{
	    		linkTo = noLinkObject;
	    	}
	        this->stopPlaying();
	        this->setLink(linkTo);
	        this->setPath(file);
	        CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"startPlaying", "(Ljava/lang/String;)V"), "Function doesn't exist");
	        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,minfo.env->NewStringUTF(file.c_str()));
	    	minfo.env->DeleteLocalRef(minfo.classID);
	    }
	}
	CCLOG("sound %s duration : %f", file.c_str(), getSoundDuration(file));
    return getSoundDuration(file);
}

void AudioPlayerRecorder::stopPlaying(EventCustom* event)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stopPlaying", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

void AudioPlayerRecorder::deleteFile(const std::string& file)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"deleteFile", "(Ljava/lang/String;)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,minfo.env->NewStringUTF(file.c_str()));
	minfo.env->DeleteLocalRef(minfo.classID);
}


void AudioPlayerRecorder::play()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"play", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::pause()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pause", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::restart()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"restart", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::fadeVolumeOut()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"fadeVolumeOut", "()V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::setNumberOfLoops(int loops)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setNumberOfLoops", "(I)V"), "Function doesn't exist");
	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)loops);
	minfo.env->DeleteLocalRef(minfo.classID);
}

float AudioPlayerRecorder::getSoundDuration(std::string file)
{
	JniMethodInfo minfo;

	if(soundsDuration == NULL)
	{
		soundsDuration = (CCDictionary*)loadObjectFromFile("__SoundsDuration.plist");
		if(soundsDuration == NULL)
		{
			soundsDuration = new CCDictionary();
		}
		else
		{
			soundsDuration->retain();
		}
	}
	CCObject* result = soundsDuration->objectForKey(file.c_str());
	//If the saved result is at 0, there was probably a problem during last try
	if(result == NULL || TOFLOAT(result) == 0)
	{
		CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getSoundDuration", "(Ljava/lang/String;)F"), "Function doesn't exist");

		float duration = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(file.c_str()));
		minfo.env->DeleteLocalRef(minfo.classID);
		soundsDuration->setObject(Fcreate(duration), file.c_str());
		saveObjectToFile(soundsDuration, "__SoundsDuration.plist");
		return duration;
	}
	return TOFLOAT(result);
}

std::string AudioPlayerRecorder::getSoundsSavePath()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, "com/fennex/modules/NativeUtility", "getLocalPath", "()Ljava/lang/String;"), "Function doesn't exist");

	jstring directory = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);

	std::string nativeString = minfo.env->GetStringUTFChars(directory, 0);
    return nativeString;
}

void AudioPlayerRecorder::setRecordEnabled(bool enabled)
{
	JniMethodInfo minfo;

	//On Android, there is currently no problem with recording being enabled or not, since the microphone is a permission
	//Check the permission and make a Toast if it's missing
    if(recordEnabled != enabled)
    {
    	if(enabled)
    	{
    		CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "checkMicrophonePermission", "()Z"), "Function doesn't exist");
    		bool permissionOK = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    		minfo.env->DeleteLocalRef(minfo.classID);

    		if(!permissionOK)
    		{
    			CCLOG("Warning : microphone permission missing, the app may crash on next record");
    		}
    	}
        recordEnabled = enabled;
    }
}

CCDictionary* AudioPlayerRecorder::getFileMetadata(const std::string& path)
{
	JniMethodInfo minfo;
	CCDictionary* metadata = Dcreate();
	metadata->setObject(Icreate(getSoundDuration(path)), "Duration");
	
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getAuthor", "(Ljava/lang/String;)Ljava/lang/String;"), "Function doesn't exist");
	jstring jPath = minfo.env->NewStringUTF(path.c_str());

    jstring result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jPath);
	minfo.env->DeleteLocalRef(minfo.classID);
	if(result != NULL)
	{
		const char* nativeString = minfo.env->GetStringUTFChars(result, 0);
		metadata->setObject(Screate(std::string(nativeString)), "Author");
		minfo.env->ReleaseStringUTFChars(result, nativeString);
	}
	
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getTitle", "(Ljava/lang/String;)Ljava/lang/String;"), "Function doesn't exist");

	result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jPath);
	minfo.env->DeleteLocalRef(minfo.classID);
	if(result != NULL)
	{
		const char* nativeString = minfo.env->GetStringUTFChars(result, 0);
		metadata->setObject(Screate(std::string(nativeString)), "Title");
		minfo.env->ReleaseStringUTFChars(result, nativeString);
	}
	
	minfo.env->DeleteLocalRef(jPath);
	return metadata;
}

extern "C"
{
	void Java_com_fennex_modules_AudioPlayerRecorder_notifyPlayingSoundEnded(JNIEnv* env, jobject thiz)
	{
		notifyPlayingSoundEnded();
	}
}
