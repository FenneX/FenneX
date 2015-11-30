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

#include "AudioPickerWrapper.h"
#include "platform/android/jni/JniHelper.h"

#define CLASS_NAME "com/fennex/modules/AudioPicker"

bool isAudioPickerExporting()
{
    return false;
}

bool pickSound(const std::string& promptText, const std::string& saveName, const std::string& identifier)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "pickSound", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPromptText = minfo.env->NewStringUTF(promptText.c_str());
    jstring jSaveName = minfo.env->NewStringUTF(saveName.c_str());
    jstring jIdentifier = minfo.env->NewStringUTF(identifier.c_str());
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, jPromptText, jSaveName, jIdentifier);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPromptText);
    minfo.env->DeleteLocalRef(jSaveName);
    minfo.env->DeleteLocalRef(jIdentifier);
    return result;
}

std::string audioPickerCurrentExport()
{
    //TODO : not strictly necessary on Android, since it's close to instant
    return "";
}

void stopAudioPickerExport()
{
    //TODO
}

extern "C"
{
    void Java_com_fennex_modules_AudioPicker_notifySoundPickedWrap(JNIEnv* env, jobject thiz, jstring name, jstring identifier)
    {
        notifySoundPicked(JniHelper::jstring2string(name), JniHelper::jstring2string(identifier));
    }
}
