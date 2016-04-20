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

void openUrl(const std::string& url)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "openUrl", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jurl = minfo.env->NewStringUTF(url.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jurl);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jurl);
}

void sendMail(const std::string& address, const std::string& subject, const std::string& message, const std::string& attachmentPlist)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "sendMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jaddress = minfo.env->NewStringUTF(address.c_str());
    jstring jsubject = minfo.env->NewStringUTF(subject.c_str());
    jstring jmessage = minfo.env->NewStringUTF(message.c_str());
    if(attachmentPlist.empty())
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jaddress, jsubject, jmessage);
    }
    else
    {
        jstring jattachmentPlist = minfo.env->NewStringUTF(attachmentPlist.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jaddress, jsubject, jmessage, jattachmentPlist);
        minfo.env->DeleteLocalRef(jattachmentPlist);
    }
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jaddress);
    minfo.env->DeleteLocalRef(jsubject);
    minfo.env->DeleteLocalRef(jmessage);
}
