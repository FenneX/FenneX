//
//  FRAppAgent.h
//
//  Copyright (c) 2014 Testin. All rights reserved.
//

#import <Foundation/Foundation.h>

#define TLOG_BUFFER_MAX_LEN        256

static const NSUInteger _crumbLogStringMaxLen   = 199;  //日志最大长度
static const NSUInteger _crumbLogRecordMaxCount = 100;  //日志最大纪录数

@interface TestinAgent : NSObject

+ (void)init:(NSString *)appId;
+ (void)init:(NSString *)appId channel:(NSString *)channel;

+ (void)setUserInfo:(NSString *)userInfo;

+ (void)reportCustomizedException:(NSException *)exception message:(NSString *)message;
+ (void)reportCustomizedException:(NSNumber *)type message:(NSString *)message stackTrace:(NSString *)stackTrace;

+ (void)leaveBreadcrumbWithString:(NSString*)string;
+ (void)leaveBreadcrumbWithFormat:(NSString *)format, ...;

+ (void)terminate;

@end

