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

#include <stdio.h>
#include <string>
#include "AppController.h"

void openUrl(const char* url)
{
    [[UIApplication sharedApplication] openURL:
     [NSURL URLWithString:[[NSString stringWithFormat:@"%s", url] stringByAddingPercentEscapesUsingEncoding:
                           NSUTF8StringEncoding]]];
}

void sendMail(const char* address, const char* subject, const char* message, const char* attachmentPlist)
{
    /* OLD alternate method to send a mail using mailto (which goes outside of the app)
    if(attachmentPlist == NULL)
    {
        NSString *mailurl=[NSString stringWithFormat:
                           @"mailto:%s?subject=%s&body=%s", address, subject, message];
        
        [[UIApplication sharedApplication] openURL:
         [NSURL URLWithString:[mailurl stringByAddingPercentEscapesUsingEncoding:
                               NSUTF8StringEncoding]]];
    }
    else
    {*/
    [[AppController sharedController] sendMail:[NSString stringWithUTF8String:address] subject:[NSString stringWithUTF8String:subject] message:[NSString stringWithUTF8String:message] attachment:attachmentPlist != NULL ? [NSString stringWithUTF8String:attachmentPlist] : NULL];
    //}
}

void sendBackgroundMail(std::string from, std::string password, std::string to, std::string subject, std::string message)
{
    NSLog(@"Send Background Mail not implemented on iOS. Requested mail:\nFrom: %s\nTo: %s\nSubject: %s\n%s", from.c_str(), to.c_str(), subject.c_str(), message.c_str());
}