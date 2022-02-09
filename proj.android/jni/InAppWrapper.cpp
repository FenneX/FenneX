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

#include "FenneX.h"
#include <jni.h>
//Currently use In-app Billing Version 3 for Google Play Store
//follow tutorial at http://developer.android.com/google/play/billing/billing_integrate.html when copying to another project
#include "InAppWrapper.h"

#include "platform/android/jni/JniHelper.h"

#include "CCJniConversion.h"

#include <android/log.h>

#define  LOG_TAG    "InAppWrapper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  CLASS_NAME "com/fennex/modules/InAppManager"

void initializePayements()
{
    LOGD("Getting initialize payements method info");
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"initialize", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    if(minfo.env->ExceptionCheck())
    {
        LOGD("Problem with initialize payements:");
        minfo.env->ExceptionDescribe();
    }
    LOGD("initializing payements ...");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    LOGD("payements initialized!");
    if(minfo.env->ExceptionCheck())
    {
        LOGD("Problem with initialize payements (after close):");
        minfo.env->ExceptionDescribe();
    }
}

void inAppPurchaseProduct(const std::string& productID)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"buyProductIdentifier", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jproductID = minfo.env->NewStringUTF(productID.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jproductID);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jproductID);
}

void acknowledgePurchase(const std::string& token)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"acknowledgePurchase", "(Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jstring jToken = minfo.env->NewStringUTF(token.c_str());
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jToken);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(jToken);
}

void restoreTransactions()
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"restoreTransactions", "()V");
    CCAssert(functionExist, "Function doesn't exist");
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
}

void requestProductsData(std::vector<std::string> products)
{
    JniMethodInfo minfo;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"requestProductsData", "([Ljava/lang/String;)V");
    CCAssert(functionExist, "Function doesn't exist");
    jobjectArray array = jobjectArrayFromStringVector(minfo.env, products);
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, array);
    minfo.env->DeleteLocalRef(minfo.classID);
    minfo.env->DeleteLocalRef(array);
}

//Cache productsInfos, because they may be erased by another request (for example, buying a product)
ValueMap productsInfos;

ValueMap getProductsInfos()
{
    JniMethodInfo minfo, minfo2;
    bool functionExist = JniHelper::getStaticMethodInfo(minfo,CLASS_NAME,"getProductsIds", "()[Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    log("Starting getProductsInfos ...");
    jobjectArray productsIdNative = (jobjectArray)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
    minfo.env->DeleteLocalRef(minfo.classID);
    std::vector<std::string> productsIds = StringVectorFromjobjectArray(minfo.env, productsIdNative);
    minfo.env->DeleteLocalRef(productsIdNative);
    
    functionExist = JniHelper::getStaticMethodInfo(minfo2,CLASS_NAME,"getProductsInfos", "(Ljava/lang/String;)[Ljava/lang/String;");
    CCAssert(functionExist, "Function doesn't exist");
    
    for(std::string productId : productsIds)
    {
        if(productsInfos.find(productId) == productsInfos.end())
        {
            log("getting product \"%s\" infos", productId.c_str());
            jstring jproductID = minfo2.env->NewStringUTF(productId.c_str());
            jobjectArray nativeArray = (jobjectArray)minfo2.env->CallStaticObjectMethod(minfo2.classID, minfo2.methodID, jproductID);
            minfo2.env->DeleteLocalRef(jproductID);
        
            ValueMap infos = MapFromjobjectArray(minfo.env, nativeArray);
            minfo2.env->DeleteLocalRef(nativeArray);
            productsInfos.insert({productId, Value(infos) });
        }
        else
        {
            log("product \"%s\" infos are already in C++ code", productId.c_str());
        }
    }
    minfo2.env->DeleteLocalRef(minfo2.classID);
    log("Returning product infos successfully");
    return productsInfos;
}

void notifyFailure(std::string eventType, std::string code, std::string reason)
{
    if(code == "BillingUnavailable")
    {
        DelayedDispatcher::eventAfterDelay("CantPayTransactionFailure", Value(ValueMap({{"Reason", Value(reason)}})), 0.01);
    }
    else if(code == "PayementCanceled")
    {
        DelayedDispatcher::eventAfterDelay("PayementCanceledTransactionFailure", Value(ValueMap({{"Reason", Value(reason)}})), 0.01);
    }
    else if(code == "ServiceDisconnected")
    {
        DelayedDispatcher::eventAfterDelay("NoConnectionTransactionFailure", Value(ValueMap({{"Reason", Value(reason)}})), 0.01);
    }
    else if(code == "NoPurchases")
    {
        DelayedDispatcher::eventAfterDelay("NoPurchasesFailure", Value(ValueMap({{"Reason", Value(reason)}})), 0.01);
    }
    else
    {
        DelayedDispatcher::eventAfterDelay("InApp" + eventType + "Failure", Value(ValueMap({{"Code", Value(code)},{"Reason", Value(reason)}})), 0.01);
    }
}

void notifySuccess(std::string eventType, std::string sku, std::string token, std::string orderId, bool needAcknowledgment)
{
    DelayedDispatcher::eventAfterDelay("InApp" + eventType + "Success", Value(ValueMap({{"ProductID", Value(sku)},{"PurchaseToken", Value(token)}, {"OrderId", Value(orderId)}, {"NeedAcknowledgment", Value(needAcknowledgment)}})), 0.01);
}

void notifyProductsInfosFetched(bool success)
{
    DelayedDispatcher::eventAfterDelay(success ? "ProductsInfosFetched" : "FailFetchProductsInfos", Value(), 0.01);
}

void notifyLicenseStatusNative(bool authorized)
{
    log("Notifying license status : %s", authorized ? "Authorized" : "Locked");
    DelayedDispatcher::eventAfterDelay("LicenseStatusUpdate", Value(ValueMap({{"Authorized", Value(authorized)}})), 0.01);
}

extern "C"
{
    void Java_com_fennex_modules_InAppManager_notifyFailure(JNIEnv* envParam, jobject thiz, jstring eventType, jstring code, jstring reason)
    {
        notifyFailure(JniHelper::jstring2string(eventType), JniHelper::jstring2string(code), JniHelper::jstring2string(reason));
    }

    void Java_com_fennex_modules_InAppManager_notifySuccess(JNIEnv* envParam, jobject thiz, jstring eventType, jstring sku, jstring token, jstring orderId, jboolean needAcknowledgment)
    {
        notifySuccess(JniHelper::jstring2string(eventType), JniHelper::jstring2string(sku), JniHelper::jstring2string(token), JniHelper::jstring2string(orderId), (bool)needAcknowledgment);
    }

    void Java_com_fennex_licensing_LicenseInspector_notifyLicenseStatus(JNIEnv* envParam, jobject thiz, jboolean authorized)
    {
        notifyLicenseStatusNative((bool)authorized);
    }
}
