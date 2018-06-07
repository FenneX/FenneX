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

#ifndef HtmlToPDFWrapper_h
#define HtmlToPDFWrapper_h

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;

/*
* Inspired from https://github.com/christopherdro/react-native-html-to-pdf/
* Create a PDF file from a HTML string passed as parameters, ex: <p>Hello, world!</p>
* pdfName is the name of the PDF with the .pdf extension, ex: "myPDF.pdf"
* On iOS it will trigger a pop-up that will allow the user to choose an app to open the PDF, therefore an app able to open a PDF file must be installed on the device, which is not always the case on iOS <= 10.10. Please check before calling this method
* On Android, the PDF file will be stored in the external storage Documents directory
* If a file with the same name already exists, PDF file will be stored as pdf name + (n).pdf, ex: "myPDF(1).pdf"
* Error Management:
 * CreateDirectoryFailed: (Android) Failed creating directory where the PDF should be saved
 * RenderFailed: (iOS/Android) Failed rendering the PDF
 * WritePDFFailed: (iOS/Android) writting in the PDF file
 * OpenPreviewFailed: (iOS) Failed opening the document previewer menu
* Errors listed above will call notifyPdfCreationFailure with the specific failureCause from the list
*/
void createPdfFromHtml(std::string htmlString, std::string pdfName);

static inline void notifyPdfCreationSuccess(std::string pdfName)
{
    Value toSend = Value(ValueMap({ {"PdfName", Value(pdfName)} }));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PDFCreated", &toSend);
}

static inline void notifyPdfCreationFailure(std::string pdfName, std::string failureCause)
{
    Value toSend = Value(ValueMap({ {"PdfName", Value(pdfName)}, {"Cause", Value(failureCause)} }));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PDFCreationFailure", &toSend);
}

#endif /* HTMLtoPDFWrapper_h */
