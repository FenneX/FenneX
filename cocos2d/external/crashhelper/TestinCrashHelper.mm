#include <string.h>
#include "TestinCrashHelper.h"
#include "cocos2d.h"

#define LOG_TAG "TestinCrashHelper"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#include <android/log.h>
	#include <jni.h>
	#include "platform/android/jni/JniHelper.h"

	#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
	#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
	#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

	#define AGENT_CLASS "com/testin/agent/TestinAgent"
	#define AGENT_METHOD_INIT "init"
	#define AGENT_METHOD_INIT_PARAMETER "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V"
	#define AGENT_METHOD_EXCEPTION "reportCustomizedException"
	#define AGENT_METHOD_EXCEPTION_PARAMETER "(ILjava/lang/String;Ljava/lang/String;)V"
	#define AGENT_METHOD_USERINFO "setUserInfo"
	#define AGENT_METHOD_USERINFO_PARAMETER "(Ljava/lang/String;)V"
    #define AGENT_METHOD_ISDEBUG "setLocalDebug"
	#define AGENT_METHOD_ISDEBUG_PARAMETER "(Z)V"
    #define AGENT_METHOD_LEAVEBREADCRUMB "leaveBreadcrumb"
    #define AGENT_METHOD_LEAVEBREADCRUMB_PARAMETER "(Ljava/lang/String;)V"
	#define AGENT_NONE_PARAMETER "()V"
	#define AGENT_CONTEXT_PARAMETER "(Landroid/content/Context;)V"
	#define COCOS_ACTIVITY_CLASS "org/cocos2dx/lib/Cocos2dxActivity"
	#define COCOS_ACTIVITY_METHOD_CONTEXT "getContext"
	#define COCOS_ACTIVITY_METHOD_CONTEXT_PARAMETER "()Landroid/content/Context;"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	#import <UIKit/UIKit.h>
	#define AGENT_CLASS @"TestinAgent"
	#define AGENT_METHOD_INIT @"init:channel:"
	#define AGENT_METHOD_EXCEPTION @"reportCustomizedException:message:stackTrace:"
	#define AGENT_METHOD_USERINFO @"setUserInfo:"
	#define AGENT_METHOD_LEAVEBREADCRUMB @"leaveBreadcrumbWithString:"
#endif


bool TestinCrashHelper::_initialed = false;

void TestinCrashHelper::initTestinAgent(const char* appKey, const char* channel) {
	if (!_initialed) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
		JNIEnv* env = NULL;
		jvm->GetEnv((void**)&env, JNI_VERSION_1_4);

		if (NULL == jvm || NULL == env) {
			LOGE("Could not complete opertion because JavaVM or JNIEnv is null!");
			return;
		}
		jvm->AttachCurrentThread(&env, 0);

		jclass clz = env->FindClass(COCOS_ACTIVITY_CLASS);
		jmethodID method = env->GetStaticMethodID(clz, COCOS_ACTIVITY_METHOD_CONTEXT, COCOS_ACTIVITY_METHOD_CONTEXT_PARAMETER);
		jobject obj = (jobject) env->CallStaticObjectMethod(clz, method);
		if (NULL == obj) {
			LOGD("Could not find Cocos2dxActivity object!");
			return;
		} else {
			LOGD("Found Cocos2dxActivity object!");
			//will throw ClassNotFoundException if Testin crash sdk is not included
			clz = env->FindClass(AGENT_CLASS);
			//will throw NoSuchMethodException if Testin crash sdk is not included
			method = env->GetStaticMethodID(clz, AGENT_METHOD_INIT, AGENT_METHOD_INIT_PARAMETER);

			jstring strParam1 = env->NewStringUTF(appKey);
			jstring strParam2 = (NULL == channel) ? NULL : env->NewStringUTF(channel);
			env->CallStaticVoidMethod(clz, method, obj, strParam1, strParam2);
		}
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		NSString* strParam1 = @(appKey);
		NSString* strParam2 = (NULL == channel) ? nil : @(channel);
		Class cls = NSClassFromString(AGENT_CLASS);
		SEL func = NSSelectorFromString(AGENT_METHOD_INIT);
		NSMethodSignature* signature = [cls methodSignatureForSelector:func];
		NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
		[invocation setTarget:cls];
		[invocation setSelector:func];
		[invocation setArgument:&strParam1 atIndex:2];
		[invocation setArgument:&strParam2 atIndex:3];
		[invocation invoke];
#endif
		_initialed = true;
	}
}


void TestinCrashHelper::reportException(int type, const char* reason, const char* traceback) { 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
	JNIEnv* env = NULL;
	jvm->GetEnv((void**)&env, JNI_VERSION_1_4);

	if (NULL == jvm || NULL == env) {
		LOGE("Could not complete opertion because JavaVM or JNIEnv is null!");
		return;
	}
	jvm->AttachCurrentThread(&env, 0);
	
	//will throw ClassNotFoundException if Testin crash sdk is not included
	jclass clz = env->FindClass(AGENT_CLASS);
	//will throw NoSuchMethodException if Testin crash sdk is not included
	jmethodID method = env->GetStaticMethodID(clz, AGENT_METHOD_EXCEPTION, AGENT_METHOD_EXCEPTION_PARAMETER);

	jstring strParam1 = env->NewStringUTF(reason);
	jstring strParam2 = env->NewStringUTF(traceback);
	env->CallStaticVoidMethod(clz, method, type, strParam1, strParam2);
	//env->DeleteLocalRef(strParam1);
	//env->DeleteLocalRef(strParam2);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	NSNumber* num = @(type);
	NSString* strParam1 = @(reason);
	NSString* strParam2 = @(traceback);
	Class cls = NSClassFromString(AGENT_CLASS);
	SEL func = NSSelectorFromString(AGENT_METHOD_EXCEPTION);
	NSMethodSignature* signature = [cls methodSignatureForSelector:func];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
	[invocation setTarget:cls];
	[invocation setSelector:func];
	[invocation setArgument:&num atIndex:2];
	[invocation setArgument:&strParam1 atIndex:3];
	[invocation setArgument:&strParam2 atIndex:4];
	[invocation invoke];
#endif
} 


void TestinCrashHelper::setUserInfo(const char* userInfo) { 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
	JNIEnv* env = NULL;
	jvm->GetEnv((void**)&env, JNI_VERSION_1_4);

	if (NULL == jvm || NULL == env) {
		LOGE("Could not complete opertion because JavaVM or JNIEnv is null!");
		return;
	}
	jvm->AttachCurrentThread(&env, 0);
	
	//will throw ClassNotFoundException if Testin crash sdk is not included
	jclass clz = env->FindClass(AGENT_CLASS);
	//will throw NoSuchMethodException if Testin crash sdk is not included
	jmethodID method = env->GetStaticMethodID(clz, AGENT_METHOD_USERINFO, AGENT_METHOD_USERINFO_PARAMETER);

	jstring strParam = env->NewStringUTF(userInfo);
	env->CallStaticVoidMethod(clz, method, strParam);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	NSString* strParam = @(userInfo);
	Class cls = NSClassFromString(AGENT_CLASS);
	SEL func = NSSelectorFromString(AGENT_METHOD_USERINFO);
	NSMethodSignature* signature = [cls methodSignatureForSelector:func];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
	[invocation setTarget:cls];
	[invocation setSelector:func];
	[invocation setArgument:&strParam atIndex:2];
	[invocation invoke];
#endif
}

void TestinCrashHelper::setLocalDebug(bool isDebug) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
    JNIEnv* env = NULL;
    jvm->GetEnv((void**)&env, JNI_VERSION_1_4);
    
    if (NULL == jvm || NULL == env) {
        LOGE("Could not complete opertion because JavaVM or JNIEnv is null!");
        return;
    }
    jvm->AttachCurrentThread(&env, 0);
    
    //will throw ClassNotFoundException if Testin crash sdk is not included
    jclass clz = env->FindClass(AGENT_CLASS);
    //will throw NoSuchMethodException if Testin crash sdk is not included
    jmethodID method = env->GetStaticMethodID(clz, AGENT_METHOD_ISDEBUG, AGENT_METHOD_ISDEBUG_PARAMETER);
    
    env->CallStaticVoidMethod(clz, method, isDebug);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#endif
}

void TestinCrashHelper::leaveBreadcrumb(const char* Breadcrumb) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
    JNIEnv* env = NULL;
    jvm->GetEnv((void**)&env, JNI_VERSION_1_4);
    
    if (NULL == jvm || NULL == env) {
        LOGE("Could not complete opertion because JavaVM or JNIEnv is null!");
        return;
    }
    jvm->AttachCurrentThread(&env, 0);
    
    //will throw ClassNotFoundException if Testin crash sdk is not included
    jclass clz = env->FindClass(AGENT_CLASS);
    //will throw NoSuchMethodException if Testin crash sdk is not included
    jmethodID method = env->GetStaticMethodID(clz, AGENT_METHOD_LEAVEBREADCRUMB, AGENT_METHOD_LEAVEBREADCRUMB_PARAMETER);
    jstring strParam = env->NewStringUTF(Breadcrumb);
    env->CallStaticVoidMethod(clz, method, strParam);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSString* strParam = @(Breadcrumb);
    Class cls = NSClassFromString(AGENT_CLASS);
    SEL func = NSSelectorFromString(AGENT_METHOD_LEAVEBREADCRUMB);
    NSMethodSignature* signature = [cls methodSignatureForSelector:func];
    NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
    [invocation setTarget:cls];
    [invocation setSelector:func];
    [invocation setArgument:&strParam atIndex:2];
    [invocation invoke];
#endif
}









