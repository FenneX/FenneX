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

#include "platform/android/jni/JniHelper.h"
#include "TTSWrapper.h"

#define CLASS_NAME "com/fennex/modules/TTS"

void initTTS()
{
    getInstance();
}

void initTTS(std::string ttsEngine)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "initInstance", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jEngineName = minfo.env->NewStringUTF(ttsEngine.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jEngineName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jEngineName);
}

jobject getInstance()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getInstance", "()Lcom/fennex/modules/TTS;");
    CCAssert(functionExist, "Function doesn't exist");
    jobject result = (jobject) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool isSpeaking()
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME,"isSpeaking", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallBooleanMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(instance);
    return result;
}

#warning TODO: pass this ID with speakText to get it back when that particular speech ends
static int speechIDStatic = -1;
bool speakText(std::vector<std::string> text, int speechID, float volume)
{
    speechIDStatic = speechID;
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo, CLASS_NAME, "speakText", "([Ljava/lang/String;F)Z");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->DeleteLocalRef(minfo.classID); //Not required, we call the method directly on the instance

    jclass jStringCls = minfo.env->FindClass("java/lang/String");
    jstring string = minfo.env->NewStringUTF("");
    jobjectArray ret = (jobjectArray)minfo.env->NewObjectArray(text.size(),
                                                               jStringCls,
                                                               string);
    minfo.env->DeleteLocalRef(jStringCls);
    minfo.env->DeleteLocalRef(string);
    
    for(int i = 0; i < text.size(); i++)
    {
        string = minfo.env->NewStringUTF(text[i].c_str());
        minfo.env->SetObjectArrayElement(ret, i, string);
        minfo.env->DeleteLocalRef(string);
    }
    
    minfo.env->CallBooleanMethod(instance, minfo.methodID, ret, (jfloat)volume);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(ret);
    return false;
}

void stopSpeakText()
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME,"stopSpeakText", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallVoidMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
}

float getCurrentVoiceWPM()
{
    //Right now, we don't have any WPM values for speech
    return DEFAULT_SPEECH_WPM;
}

float getTTSPlayRate()
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME,"getTTSPlayRate", "()F");
    CCAssert(functionExist, "Function doesn't exist");
    float result = minfo.env->CallFloatMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

void setTTSPlayRate(float rate)
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME,"setTTSPlayRate", "(F)V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallVoidMethod(instance, minfo.methodID, (jfloat)rate);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
}

std::string getTTSEngineName()
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME,"getTTSEngineName", "()Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    jstring str = (jstring) minfo.env->CallObjectMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
    std::string ret = JniHelper::jstring2string(str);
    minfo.env->DeleteLocalRef(str);
    return ret;
}

 std::map<std::string, std::string> getTTSEngines()
{
    JniMethodInfo minfo;
    jobject instance = getInstance();
    bool functionExist = JniHelper::getMethodInfo(minfo,CLASS_NAME, "getTTSEngines", "()[[Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    jobjectArray dataArray = (jobjectArray)minfo.env->CallObjectMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
    std::map<std::string, std::string> info;
    jsize count = count = minfo.env->GetArrayLength(dataArray);
    for(int i = 0;i < count; i++) {
        //Run through the array, retrieve each type and set it in a vector
        jobjectArray elementArray = (jobjectArray)minfo.env->GetObjectArrayElement(dataArray, i);
        jsize elementCount = minfo.env->GetArrayLength(elementArray);
        std::vector<std::string> elementString;
        for(int j = 0;j < elementCount; j++) {
            jobject element = minfo.env->GetObjectArrayElement(elementArray, j);
            elementString.push_back(JniHelper::jstring2string((jstring)element));
            minfo.env->DeleteLocalRef(element);
        }
        if(elementString.size() > 1) info.insert({elementString.at(0), elementString.at(1)});
        minfo.env->DeleteLocalRef(elementArray);
    }
    minfo.env->DeleteLocalRef(dataArray);
    return info;
}

void setTTSEngine(std::string engineName)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"setTTSEngine", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jEngineName = minfo.env->NewStringUTF(engineName.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID,
                                    jEngineName);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jEngineName);
}

extern "C"
{
    void Java_com_fennex_modules_TTS_onTTSEnd(JNIEnv* env, jobject thiz)
    {
        notifyTTSDone(speechIDStatic);
    }

    void Java_com_fennex_modules_TTS_onTTSError(JNIEnv* env, jobject thiz, jstring error)
    {
        notifyTTSError(JniHelper::jstring2string(error));
    }
}
