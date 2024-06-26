/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
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
 ****************************************************************************/
#import <UIKit/UIKit.h>
#import "AppController.h"
#import "cocos2d.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "AppDelegate.h"
#import "NSCCConverter.h"

#import "RootViewController.h"

#import "ImagePicker.h"
#import "FenneX.h"
#import "AppMacros.h"
#include "NativeUtility.h"
#include "Shorteners.h"
#import "Firebase.h"

static AppController* _sharedController = nullptr;

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

@synthesize window;
@synthesize viewController;
@synthesize openUrl;

// cocos2d application instance
static AppDelegate s_sharedApplication;

+ (AppController*) sharedController
{
    return _sharedController;
}

- (void) sendMail:(NSString*)address subject:(NSString*)subject message:(NSString*)message attachment:(NSString*)attachment
{
    [self.viewController setModalTransitionStyle: UIModalTransitionStyleCoverVertical];
    
    
    MFMailComposeViewController *mailer = [[MFMailComposeViewController alloc] init];
    //If mailer is nullptr, it means the user didn't set up a mail account, and he will be warned by iOS
    if(mailer != nullptr)
    {
        mailer.mailComposeDelegate = self;
        
        [mailer setSubject:subject];
        
        NSArray *toRecipients = [NSArray arrayWithObject:address];
        [mailer setToRecipients:toRecipients];
        
        [mailer setMessageBody:message isHTML:NO];
        if(attachment != nil)
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString *documentsPath = [paths objectAtIndex:0];
            NSLog(@"Documents path : %@", documentsPath);
            NSLog(@"with getenv : %s/Documents/%@", getenv("HOME"), attachment);
            NSString* path= [NSString stringWithFormat:@"%s/Documents/%@", getenv("HOME"), attachment];
            
            NSLog(@"Path : %@", path);
            
            [mailer addAttachmentData:[NSData dataWithContentsOfFile:path] mimeType:@"application/xml" fileName:@"results.plist"];
        }
        [self.viewController presentViewController:mailer animated:YES completion:nil];
        
        [mailer release];
    }
}

- (void) mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error
{
    switch (result)
    {
        case MFMailComposeResultCancelled:
            NSLog(@"Mail cancelled: you cancelled the operation and no email message was queued.");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("MailCanceled");
            break;
        case MFMailComposeResultSaved:
            NSLog(@"Mail saved: you saved the email message in the drafts folder.");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("MailSaved");
            break;
        case MFMailComposeResultSent:
            NSLog(@"Mail send: the email message is queued in the outbox. It is ready to send.");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("MailSent");
            break;
        case MFMailComposeResultFailed:
            NSLog(@"Mail failed: the email message was not saved or queued, possibly due to an error.");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("MailFailed");
            break;
        default:
            NSLog(@"Mail not sent.");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("MailNotSent");
            break;
    }
	
	// Remove the mail view
    [self.viewController dismissViewControllerAnimated:YES completion:nil];
}

void uncaughtExceptionHandler(NSException *exception)
{
    NSLog(@"CRASH: %@", exception);
    NSLog(@"Stack Trace: %@", [exception callStackSymbols]);
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);
    // Override point for customization after application launch.
    _sharedController = self;
    
    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup:nil
                                   multiSampling:NO
                                 numberOfSamples:0];
    [eaglView setMultipleTouchEnabled:YES];
    eaglView.opaque = NO;
    
    // Use RootViewController manage EAGLView
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    // Note: there used to be wantsFullScreenLayout here. It was deprecated in iOS7, and it is now the default on iOS, as views are supposed to include status bar with the right style, or none if they want full screen.  
    viewController.view = eaglView;
    
    // Set RootViewController to window
    [window setRootViewController:viewController];
    
    
    [window makeKeyAndVisible];
    
    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    
    //Having the openUrl before launching cocos2d-x is important so that C++ code can check for an openUrl on launch without notification
    openUrl = @"";
    if([launchOptions objectForKey:UIApplicationLaunchOptionsURLKey] != nullptr)
    {
        openUrl = [launchOptions objectForKey:UIApplicationLaunchOptionsURLKey];
    }
    [openUrl retain];
    
    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);
    [FIRApp configure];
    cocos2d::Application::getInstance()->run();

    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    center.delegate = self;
    
    NSLog(@"app launch");
    return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    if(openUrl)
    {
        [openUrl autorelease];
    }
    openUrl = [url absoluteString];
    [openUrl retain];
    notifyUrlOpened([[url absoluteString] UTF8String]);
    return YES;
}

- (void) dealWithNotification:(UNNotification*) notification {
    if([notification.request.content.userInfo objectForKey:@"CallbackEvent"] != nil)
    {
        NSLog(@"local notif with callback event");
        Value val = Value([NSCCConverter valueMapFromNSDictionary:notification.request.content.userInfo]);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent([[notification.request.content.userInfo objectForKey:@"CallbackEvent"] UTF8String], &val);
    }
    else if([notification.request.content.userInfo objectForKey:@"OpenUrl"] != nil)
    {
        if(openUrl)
        {
            [openUrl autorelease];
        }
        openUrl = [notification.request.content.userInfo objectForKey:@"OpenUrl"];
        [openUrl retain];
        notifyUrlOpened([openUrl UTF8String]);
    }
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
didReceiveNotificationResponse:(UNNotificationResponse *)response
         withCompletionHandler:(void (^)())completionHandler {
    // custom code to handle push while app is in the background or closed
    [self dealWithNotification:response.notification];
    completionHandler();
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions))completionHandler {
    // custom code to handle push while app is in the foreground
    [self dealWithNotification:notification];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    //cocos2d::Director::getInstance()->pause();
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("AppWillResignActive");
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    //cocos2d::Director::getInstance()->resume();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    Application::getInstance()->applicationDidEnterBackground();
    cocos2d::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent);
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    Application::getInstance()->applicationWillEnterForeground();
    cocos2d::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent);
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)w {
    /*
     Avoid crash with UIImagePicker because UIImagePicker doesn't support UIInterfaceOrientationLandscape
     */
    return (NSUInteger)[application supportedInterfaceOrientationsForWindow:w] | (1<<UIInterfaceOrientationPortrait);
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    notifyMemoryWarning();
}


- (void)dealloc {
    _sharedController = nullptr;
    [super dealloc];
}

@end

