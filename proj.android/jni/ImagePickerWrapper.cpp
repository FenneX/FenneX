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
#include "ImagePickerWrapper.h"
#include "platform/android/jni/JniHelper.h"

#define CLASS_NAME "com/fennex/modules/ImagePicker"

USING_NS_FENNEX;

bool pickImageFrom(const char* saveName, bool useCamera, int width, int height, const char* identifier, bool rescale, float thumbnailScale)
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pickImageFrom", "(Ljava/lang/String;ZIILjava/lang/String;FZ)Z"), "Function doesn't exist");
	jstring jSaveName = minfo.env->NewStringUTF(saveName);
	jstring jIdentifier = minfo.env->NewStringUTF(identifier);
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID,
													 minfo.methodID,
													 jSaveName,
													 (jboolean)useCamera,
													 (jint)width,
													 (jint)height,
													 jIdentifier,
													 (jfloat)thumbnailScale,
													 (jboolean)rescale);
    minfo.env->DeleteLocalRef(minfo.classID);
	minfo.env->DeleteLocalRef(jSaveName);
	minfo.env->DeleteLocalRef(jIdentifier);
    return result;
}

bool isCameraAvailable()
{
	JniMethodInfo minfo;
	CCAssert(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "isCameraAvailable", "()Z"), "Function doesn't exist");
	bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

extern "C"
{
	//extension for long name : __Ljava_lang_String_2Ljava_lang_String_2
	void Java_com_fennex_modules_ImagePicker_notifyImagePickedWrap(JNIEnv* env, jobject thiz, jstring name, jstring identifier)
	{
		notifyImagePicked(env->GetStringUTFChars(name, 0), env->GetStringUTFChars(identifier, 0));
	}
}
