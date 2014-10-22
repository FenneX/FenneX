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
#include "platform/android/jni/JniHelper.h"
#include "TTSWrapper.h"

#define CLASS_NAME "com/fennex/modules/TTS"

void initTTS()
{
	getInstance();
}

jobject getInstance()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getInstance", "()Lcom/fennex/modules/TTS;"), "Function doesn't exist");
	jobject result = (jobject) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

bool isSpeaking()
{
	JniMethodInfo minfo;
	jobject instance = getInstance();
	CCAssert(JniHelper::getMethodInfo(minfo,CLASS_NAME,"isSpeaking", "()Z"), "Function doesn't exist");
	bool result = minfo.env->CallBooleanMethod(instance, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

#warning TODO: pass this ID with speakText to get it back when that particular speech ends
static int speechIDStatic = -1;
bool speakText(std::vector<std::string> text, int speechID)
{
	if(!isSpeaking())
	{
		speechIDStatic = speechID;
		JniMethodInfo minfo;
		jobject instance = getInstance();
		CCAssert(JniHelper::getMethodInfo(minfo,CLASS_NAME,"speakText", "([Ljava/lang/String;)Z"), "Function doesn't exist");

		jobjectArray ret= (jobjectArray)minfo.env->NewObjectArray(text.size(),
				minfo.env->FindClass("java/lang/String"),
				minfo.env->NewStringUTF("")
		);
	    minfo.env->DeleteLocalRef(minfo.classID);

        for(int i = 0; i < text.size(); i++)
		{
			minfo.env->SetObjectArrayElement(ret, i, minfo.env->NewStringUTF(text[i].c_str()));
		}

		minfo.env->CallBooleanMethod(instance, minfo.methodID, ret);
		minfo.env->DeleteLocalRef(ret);
		minfo.env->DeleteLocalRef(instance);
	}

	return false;
}

void stopSpeakText()
{
	JniMethodInfo minfo;
	jobject instance = getInstance();
	CCAssert(JniHelper::getMethodInfo(minfo,CLASS_NAME,"stopSpeakText", "()V"), "Function doesn't exist");
	minfo.env->CallVoidMethod(instance, minfo.methodID);
	minfo.env->DeleteLocalRef(instance);
    minfo.env->DeleteLocalRef(minfo.classID);
}

extern "C"
{
	void Java_com_fennex_modules_TTS_onTTSEnd(JNIEnv* env, jobject thiz)
	{
		notifyTTSDone(speechIDStatic);
	}
}
