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

#ifndef HTMLtoPDF_h
#define HTMLtoPDF_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "DocumentPreviewer.h"
#import "HtmlToPDFWrapper.h"
#import "Utility.h"


/*
 * Sizes are defined in points, with 72 points corresponding to one inch or about 2.54cm. A4 is 21cm x 29,7cm.
 * Other formats will be supported in the future.
 */
#define kPaperSizeA4 CGSizeMake(595.2,841.8)

@class HTMLtoPDF;

@interface HTMLtoPDF : UIViewController <UIWebViewDelegate>

@property (nonatomic, strong, readonly) NSString *PDFpath;
@property (nonatomic, strong, readonly) NSData *PDFdata;

+ (id) createPDFWithHTML:(NSString*)HTML inDirectory:(NSURL*)directory savePDFTo:(NSString*)PDFpath pageSize:(CGSize)pageSize;

@end

#endif /* HTMLtoPDF_h */
