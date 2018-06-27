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

#ifndef DocumentPreviewer_h
#define DocumentPreviewer_h

/* this class purpose is to launch the iOS native pop up for previewing document in other app and place it in the middle of the screen
 * In order to be properly used with HTMLtoPDF, you must first have an app that support PDF reading installed on your device, otherwise the PDF won't even be created by the HTMLtoPDF class as there's no way to access it with a file explorer
 */
@interface DocumentPreviewer : UIViewController <UIDocumentInteractionControllerDelegate>

@property (nonatomic, strong) UIDocumentInteractionController* documentInteractionController;

- (bool) previewDocument:(NSString*)pdfPath;

@end

#endif /* DocumentPreviewer_h */
