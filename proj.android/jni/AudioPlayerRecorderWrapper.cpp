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
ValueMap soundsDuration = ValueMap();

void AudioPlayerRecorder::setUseVLC(bool useVLC)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setUseVLC", "(Z)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jboolean)useVLC);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::init()
{
#warning not implemented
}

bool AudioPlayerRecorder::isRecording()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isRecording", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = (link != NULL && minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID));
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool AudioPlayerRecorder::isPlaying()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"isPlaying", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result =  (link != NULL && minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID));
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}


void AudioPlayerRecorder::record(const std::string& file, FileLocation location, Ref* linkTo)
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
        
        bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"startRecording", "(Ljava/lang/String;I)V");
        CCAssert(functionExist, "Function doesn't exist");
        jstring string0 = minfo.env->NewStringUTF(withExtension.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, string0, (jint)location);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(string0);
    }
}

void AudioPlayerRecorder::stopRecording()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stopRecording", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

float AudioPlayerRecorder::play(const std::string& file, Ref* linkTo, bool independent, float volume)
{
    JniMethodInfo minfo;
    
    if(independent)
    {
        bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"playIndependent", "(Ljava/lang/String;F)V");
        CCAssert(functionExist, "Function doesn't exist");
        jstring string0 = minfo.env->NewStringUTF(file.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, string0, (jfloat)volume);
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(string0);
    }
    else
    {
        interruptLoop = true;
        if(linkTo == link && this->isPlaying())
        {
            this->stopPlaying();
            log(" END AudioPlayerRecorder::play");
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
            bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"startPlaying", "(Ljava/lang/String;F)V");
            CCAssert(functionExist, "Function doesn't exist");
            jstring string0 = minfo.env->NewStringUTF(file.c_str());
            minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, string0, (jfloat)volume);
            minfo.env->DeleteLocalRef(minfo.classID);
            minfo.env->DeleteLocalRef(string0);
        }
    }
    log("sound %s duration : %f", file.c_str(), getSoundDuration(file));
    return getSoundDuration(file);
}

void AudioPlayerRecorder::stopPlaying(EventCustom* event)
{
    interruptLoop = true;
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"stopPlaying", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

void AudioPlayerRecorder::play()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"play", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::pause()
{
    interruptLoop = true;
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pause", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::restart()
{
    interruptLoop = true;
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"restart", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void AudioPlayerRecorder::fadeVolumeOut()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"fadeVolumeOut", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float AudioPlayerRecorder::getPlaybackRate()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getPlaybackRate", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void AudioPlayerRecorder::setPlaybackRate(float rate)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"setPlaybackRate", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jfloat)rate);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float AudioPlayerRecorder::getSoundDuration(const std::string& file)
{
    JniMethodInfo minfo;
    Value soundDurationValue = Value();
    if(soundsDuration.empty())
    {
        soundDurationValue = loadValueFromFile("__SoundsDuration.plist");
        soundsDuration = soundDurationValue.getType() == Value::Type::MAP ? soundDurationValue.asValueMap() : ValueMap();
    }
    Value result = soundsDuration[file.c_str()];
    //If the saved result is at 0, there was probably a problem during last try
    if(!isValueOfType(result, FLOAT))
    {
        bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getSoundDuration", "(Ljava/lang/String;)F");
        CCAssert(functionExist, "Function doesn't exist");
        
        jstring string0 = minfo.env->NewStringUTF(file.c_str());
        float duration = minfo.env->CallStaticFloatMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(file.c_str()));
        minfo.env->DeleteLocalRef(minfo.classID);
        minfo.env->DeleteLocalRef(string0);
        soundsDuration[file.c_str()] = Value(duration);
        soundDurationValue = Value(soundsDuration);
        saveValueToFile(soundDurationValue, "__SoundsDuration.plist");
        return duration;
    }
    return result.asFloat();
}

std::string AudioPlayerRecorder::getSoundsSavePath()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, "com/fennex/modules/NativeUtility", "getLocalPath", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    jstring directory = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    std::string result = JniHelper::jstring2string(directory);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(directory);
    return result;
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
            bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "checkMicrophonePermission", "()Z");
            CCAssert(functionExist, "Function doesn't exist");
            bool permissionOK = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
            minfo.env->DeleteLocalRef(minfo.classID);
            
            if(!permissionOK)
            {
                log("Warning : microphone permission missing, the app may crash on next record");
            }
        }
        recordEnabled = enabled;
    }
}

ValueMap AudioPlayerRecorder::getFileMetadata(const std::string& path)
{
    JniMethodInfo minfo;
    ValueMap metadata = ValueMap();
    metadata["Duration"] = Value(getSoundDuration(path));
    
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getAuthor", "(Ljava/lang/String;)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPath = minfo.env->NewStringUTF(path.c_str());
    
    jstring result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jPath);
    minfo.env->DeleteLocalRef(minfo.classID);
    if(result != NULL)
    {
        metadata["Author"] = Value(JniHelper::jstring2string(result));
        minfo.env->DeleteLocalRef(result);
    }
    
    functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getTitle", "(Ljava/lang/String;)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jPath);
    minfo.env->DeleteLocalRef(minfo.classID);
    if(result != NULL)
    {
        metadata["Title"] = Value(JniHelper::jstring2string(result));
        minfo.env->DeleteLocalRef(result);
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
