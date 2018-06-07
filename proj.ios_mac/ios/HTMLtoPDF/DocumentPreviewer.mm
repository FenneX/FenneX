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

#import <Foundation/Foundation.h>
#import "DocumentPreviewer.h"
#import "AppController.h"
#import "RootViewController.h"
#import <string>
#import "FenneX.h"

@implementation DocumentPreviewer

- (bool) previewDocument:(NSString*)pdfPath {
    NSURL *URL = [NSURL fileURLWithPath:pdfPath];
    CCASSERT(URL != nil, "DocumentPreviewer: Could not create URL from pdf path");
    
    self.documentInteractionController = [UIDocumentInteractionController interactionControllerWithURL:URL];
    UIViewController* rootVC = [AppController sharedController].viewController;
    
    return [self.documentInteractionController presentOpenInMenuFromRect:CGRectZero inView:rootVC.view animated:YES];
}
@end


