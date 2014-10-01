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
#include "ExpansionSupport.h"

#define CLASS_NAME "com/fennex/modules/ExpansionSupport"

USING_NS_FENNEX;

bool checkExpansionFiles()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"checkExpansionFiles", "()Z"), "Function doesn't exist");
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

std::string getExpansionFileFullPath(bool main)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getExpansionFileFullPath", "(Z)Ljava/lang/String;"), "Function doesn't exist");
	jstring result = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, (jboolean)main);
	minfo.env->DeleteLocalRef(minfo.classID);

	const char *nativeResult = minfo.env->GetStringUTFChars(result, 0);
	std::string absolutePath = std::string(nativeResult);
	minfo.env->ReleaseStringUTFChars(result, nativeResult);
	return absolutePath;
}

extern "C"
{
	void Java_com_fennex_modules_ExpansionSupport_notifyServiceConnected(JNIEnv* env, jobject thiz)
	{
		notifyServiceConnected();
	}
	void Java_com_fennex_modules_ExpansionSupport_notifyDownloadStateChanged(JNIEnv* env, jobject thiz, jstring status, jint code)
	{
		const char* statusC = env->GetStringUTFChars(status, 0);
		notifyDownloadStateChanged(statusC != NULL ? statusC : "", (int) code);
		env->ReleaseStringUTFChars(status, statusC);
	}
	void Java_com_fennex_modules_ExpansionSupport_notifyDownloadCompleted(JNIEnv* env, jobject thiz)
	{
		notifyDownloadCompleted();
	}
	void Java_com_fennex_modules_ExpansionSupport_notifyDownloadProgress(JNIEnv* env, jobject thiz, jfloat percent, jlong totalSize)
	{
		notifyDownloadProgress((float) percent, (long) totalSize);
	}
}
