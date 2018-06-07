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

#import "HTMLToPDF.h"
#import <Foundation/Foundation.h>
#import "HtmlToPDFWrapper.h"
#import "AppController.h"

@interface HTMLtoPDF ()

@property (nonatomic, strong) NSString* PDFpath;
@property (nonatomic, strong) UIWebView* webview;
@property (nonatomic, assign) CGSize pageSize;

- (id) initWithHTML:(NSString*)HTML inDirectory:(NSURL*)directory savePDFTo:(NSString*)PDFpath pageSize:(CGSize)pageSize;
- (void) webViewDidFinishLoad:(UIWebView*)webView;
- (void) terminateWebTask;

@end

@interface UIPrintPageRenderer (PDF)

- (NSData*) printToPDF;

@end

@implementation HTMLtoPDF

+ (id) createPDFWithHTML:(NSString*)HTML
             inDirectory:(NSURL*)directory
               savePDFTo:(NSString*)PDFpath
                pageSize:(CGSize)pageSize
{
    return [[[HTMLtoPDF alloc] initWithHTML:HTML inDirectory:directory  savePDFTo:PDFpath pageSize:pageSize] autorelease];
}

- (id) initWithHTML:(NSString*)HTML
        inDirectory:(NSURL*)directory
          savePDFTo:(NSString*)PDFpath
           pageSize:(CGSize)pageSize
{
    if (self = [super init])
    {
        self.PDFpath = PDFpath;
        self.pageSize = pageSize;
        
        //self must be held somewhere. Add it to Root View Controller for the PDF creation time
        [[AppController sharedController].viewController addChildViewController:self];
        
        //Make UIViewController main view transparent and a pixel to avoid interaction
        self.view.frame = CGRectMake(0, 0, 1, 1);
        self.view.alpha = 0.0;
        
        //Webview is not supposed to be visible, it's only there to draw the HTML
        self.webview = [[[UIWebView alloc] initWithFrame:self.view.frame] autorelease];
        self.webview.delegate = self;
        [self.webview loadHTMLString:HTML baseURL:directory];
    }
    return self;
}

- (void) webViewDidFinishLoad:(UIWebView*)webView
{
    //Render the WebView to a PDF
    UIPrintPageRenderer* render = [[[UIPrintPageRenderer alloc] init] autorelease];
    NSData* PDFdata = [render printToPDF:self.webview pageSize:self.pageSize];
    
    //Ensure PDF is created, write it to a File, then open it in via an external app
    if (!PDFdata)
    {
        notifyPdfCreationFailure(getStdString(self.PDFpath), "RenderFailed");
        [self terminateWebTask];
        return;
    }
    if (![PDFdata writeToFile: self.PDFpath  atomically: YES])
    {
        notifyPdfCreationFailure(getStdString(self.PDFpath), "WritePDFFailed");
        [self terminateWebTask];
        return;
    }
    
    DocumentPreviewer *creator = [[[DocumentPreviewer alloc] init] autorelease];
    if(![creator previewDocument:self.PDFpath])
    {
        notifyPdfCreationFailure(getStdString(self.PDFpath), "OpenPreviewFailed");
        [self terminateWebTask];
        return;
    }
    notifyPdfCreationSuccess(getStdString(self.PDFpath));
    [self terminateWebTask];
}

- (void) terminateWebTask
{
    [self.webview stopLoading];
    self.webview = nil;
    [self removeFromParentViewController];
}

@end

@implementation UIPrintPageRenderer (PDF)

- (NSData*) printToPDF:(UIWebView*)view pageSize:(CGSize)pageSize
{
    [self addPrintFormatter:view.viewPrintFormatter startingAtPageAtIndex:0];
    // MS Word standard margins of 2.5cm for a page body, converted to point
    // At the moment, they are all the same, but we might change them: MS Word allows 1.5cm margin on top and bottom for footer/header
#define MARGIN 70.87
    UIEdgeInsets pageMargins = {.left = MARGIN, .right = MARGIN, .top = MARGIN, .bottom = MARGIN};
    CGRect printableRect = CGRectMake(pageMargins.left,
                                      pageMargins.top,
                                      pageSize.width - pageMargins.left - pageMargins.right,
                                      pageSize.height - pageMargins.top - pageMargins.bottom);
    
    CGRect paperRect = CGRectMake(0, 0, pageSize.width, pageSize.height);
    [self setValue:[NSValue valueWithCGRect:paperRect] forKey:@"paperRect"];
    [self setValue:[NSValue valueWithCGRect:printableRect] forKey:@"printableRect"];
    
    NSMutableData *pdfData = [NSMutableData data];
    UIGraphicsBeginPDFContextToData(pdfData, self.paperRect, nil);
    [self prepareForDrawingPages:NSMakeRange(0, self.numberOfPages)];
    CGRect bounds = UIGraphicsGetPDFContextBounds();
    for (int i = 0; i < self.numberOfPages; i++)
    {
        UIGraphicsBeginPDFPage();
        [self drawPageAtIndex:i inRect:bounds];
    }
    UIGraphicsEndPDFContext();
    return pdfData;
}

@end
