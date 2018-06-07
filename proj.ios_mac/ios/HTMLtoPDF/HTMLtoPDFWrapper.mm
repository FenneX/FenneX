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

#include "HtmlToPDFWrapper.h"
#include "FileUtility.h"
#include "Utility.h"
#import "HTMLToPDF.h"

NSString* applicationDocumentsDirectory()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = paths.firstObject;
    return basePath;
}

// Find out if there's an app able to open a PDF on the user device
bool canOpenDocumentWithURL(NSURL *url)
{
    UIView *v = [[UIView alloc] init];
    BOOL canOpen = NO;
    UIDocumentInteractionController* docController = [UIDocumentInteractionController interactionControllerWithURL:url];
    if (docController)
    {
        canOpen = [docController presentOpenInMenuFromRect:CGRectZero inView:v animated:NO];
        [docController dismissMenuAnimated:NO];
    }
    [v release];
    return canOpen;
}

void createPdfFromHtml(std::string htmlString, std::string pdfName)
{
    // This chunk of code is to ensure that the created PDF will be readable by one of the installed app on the device, if not, the PDF won't be created
    CCAssert(canOpenDocumentWithURL([NSURL fileURLWithPath:getNSString(FenneX::getLocalPath(".pdf"))]), "HTMLtoPDF: No appropriate app to open PDF files has been found on your device, please make sure you have iOS version > 10.10 installed or any PDF reading app before calling this method");
    CCAssert(!htmlString.empty(), "HTMLtoPDF: The HTML string layout to create a PDF cannot be empty");
    CCAssert(!htmlString.empty(), "HTMLtoPDF: The PDF name cannot be empty");
    
    NSString* pdfPath = [NSString stringWithFormat:@"%@/%@", applicationDocumentsDirectory(), getNSString(pdfName)];
    NSURL* directory = [NSURL URLWithString:getNSString(FenneX::getLocalPath(""))];
    
    HTMLtoPDF *pdfCreator = [HTMLtoPDF createPDFWithHTML:getNSString(htmlString)
                                             inDirectory:directory
                                               savePDFTo:pdfPath
                                                pageSize:kPaperSizeA4];
}
