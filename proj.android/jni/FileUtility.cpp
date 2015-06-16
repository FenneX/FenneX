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
#include "cocos2d.h"
#include "FenneXCore.h"
#include "platform/android/jni/JniHelper.h"

#define CLASS_NAME "com/fennex/modules/FileUtility"
USING_NS_CC;
USING_NS_FENNEX;

bool lockFile(std::string filename)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"lockFile", "(Ljava/lang/String;)Z"), "Function doesn't exist");
	jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID,
													 minfo.methodID,
													 jFilename);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jFilename);
    return result;
}

std::string getLockedFileContents(std::string filename)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getLockedFileContents", "(Ljava/lang/String;)Ljava/lang/String;"), "Function doesn't exist");
	jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
	jstring jResult = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID,
													            minfo.methodID,
													            jFilename);
	std::string result = JniHelper::jstring2string(jResult);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jFilename);
	minfo.env->DeleteLocalRef(jResult);
    return result;
    
}

bool writeLockedFile(std::string filename, std::string content)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"writeLockedFile", "(Ljava/lang/String;Ljava/lang/String;)Z"), "Function doesn't exist");
	jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
	jstring jContent = minfo.env->NewStringUTF(content.c_str());
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID,
													 minfo.methodID,
													 jFilename,
													 jContent);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jFilename);
	minfo.env->DeleteLocalRef(jContent);
    return result;
}

void unlockFile(std::string filename)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"unlockFile", "(Ljava/lang/String;)V"), "Function doesn't exist");
	jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
	minfo.env->CallStaticVoidMethod(minfo.classID,
									minfo.methodID,
									jFilename);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jFilename);
}