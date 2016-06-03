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

#include "cocos2d.h"
#include "FenneXCore.h"
#include "platform/android/jni/JniHelper.h"
#include "FileUtility.h"

#define CLASS_NAME "com/fennex/modules/FileUtility"
USING_NS_CC;
USING_NS_FENNEX;

bool lockFile(std::string filename)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"lockFile", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
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
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getLockedFileContents", "(Ljava/lang/String;)Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
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
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"writeLockedFile", "(Ljava/lang/String;Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
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
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"unlockFile", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID,
                                    jFilename);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jFilename);
}

std::vector<std::string> getFilesInFolder(std::string folderPath)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "getFilesInFolder", "(Ljava/lang/String;)[Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jFolderPath = minfo.env->NewStringUTF(folderPath.c_str());
    jobjectArray dataArray = (jobjectArray)minfo.env->CallStaticObjectMethod(minfo.classID,
                                                                             minfo.methodID,
                                                                             jFolderPath);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jFolderPath);
    std::vector<std::string> info;
    jsize count = count = minfo.env->GetArrayLength(dataArray);
    for(int i = 0;i < count; i++) {
        //Run through the array, retrieve each type and set it in a CCArray
        jobject element = minfo.env->GetObjectArrayElement(dataArray, i);
        info.push_back(JniHelper::jstring2string((jstring)element));
        minfo.env->DeleteLocalRef(element);
    }
    minfo.env->DeleteLocalRef(dataArray);
    return info;
}

void deleteFile(std::string filename)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"deleteFile", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jFilename = minfo.env->NewStringUTF(filename.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID,
                                    minfo.methodID,
                                    jFilename);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jFilename);
}

bool moveFileToLocalDirectory(std::string path)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo, CLASS_NAME,"moveFileToLocalDirectory", "(Ljava/lang/String;)Z");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jPath = minfo.env->NewStringUTF(path.c_str());
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID,
                                                     minfo.methodID,
                                                     jPath);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jPath);
    return result;
}

bool pickFile()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"pickFile", "()Z");
    CCAssert(functionExist, "Function doesn't exist");
    bool result = minfo.env->CallStaticBooleanMethod(minfo.classID,
                                                     minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    return result;
}

extern "C"
{
    //extension for long name : __Ljava_lang_String_2Ljava_lang_String_2
    void Java_com_fennex_modules_FileUtility_notifyFilePicked(JNIEnv* env, jobject thiz, jstring path)
    {
        notifyFilePicked(JniHelper::jstring2string(path));
    }
}
