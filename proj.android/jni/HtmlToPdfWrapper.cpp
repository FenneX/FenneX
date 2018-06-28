/****************************************************************************
Copyright (c) 2013-2018 Auticiel SAS

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

#include "HtmlToPdfWrapper.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

#define CLASS_NAME "com/fennex/modules/HtmlToPdf"

void createPdfFromHtml(std::string htmlString, std::string pdfName, std::string pageSize)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "convert", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jpath = minfo.env->NewStringUTF(pdfName.c_str());
    jstring jhtml = minfo.env->NewStringUTF(htmlString.c_str());
    jstring jpageSize = minfo.env->NewStringUTF(pageSize.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jhtml, jpath, jpageSize);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jpath);
    minfo.env->DeleteLocalRef(jhtml);
    minfo.env->DeleteLocalRef(jpageSize);
}

extern "C"
{
    void Java_com_fennex_modules_HtmlToPdf_notifyPdfCreationFailure(JNIEnv* env, jobject thiz, jstring pdfName, jstring failureCause)
    {
        notifyPdfCreationFailure(JniHelper::jstring2string(pdfName), JniHelper::jstring2string(failureCause));
    }

    void Java_com_fennex_modules_HtmlToPdf_notifyPdfCreationSuccess(JNIEnv* env, jobject thiz, jstring pdfName)
    {
        notifyPdfCreationSuccess(JniHelper::jstring2string(pdfName));
    }
}
