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
#include "platform/android/jni/JniHelper.h"

#define  CLASS_NAME "com/fennex/modules/MailUrlManager"

USING_NS_FENNEX;

void openUrl(const char* url)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "openUrl", "(Ljava/lang/String;)V"), "Function doesn't exist");
	minfo.env->CallStaticBooleanMethod(minfo.env->FindClass(CLASS_NAME), minfo.env->GetStaticMethodID(minfo.env->FindClass(CLASS_NAME), "openUrl", "(Ljava/lang/String;)V"), minfo.env->NewStringUTF(url));
	minfo.env->DeleteLocalRef(minfo.classID);
}

void sendMail(const char* address, const char* subject, const char* message, const char* attachmentPlist)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "sendMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"), "Function doesn't exist");
	if(attachmentPlist == NULL)
	{
		minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(address), minfo.env->NewStringUTF(subject), minfo.env->NewStringUTF(message));
	}
	else
	{
		minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(address), minfo.env->NewStringUTF(subject), minfo.env->NewStringUTF(message), minfo.env->NewStringUTF(attachmentPlist));
	}
	minfo.env->DeleteLocalRef(minfo.classID);
}

void sendBackgroundMail(std::string from, std::string password, std::string to, std::string subject, std::string message)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "sendBackgroundMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"), "Function doesn't exist");
    minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID, minfo.env->NewStringUTF(from.c_str()), minfo.env->NewStringUTF(password.c_str()), minfo.env->NewStringUTF(to.c_str()), minfo.env->NewStringUTF(subject.c_str()), minfo.env->NewStringUTF(message.c_str()));
	minfo.env->DeleteLocalRef(minfo.classID);
}
