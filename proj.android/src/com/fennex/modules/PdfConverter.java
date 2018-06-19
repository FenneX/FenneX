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

/*
 * This file package should be com.fennex.modules, however it is required to be part of android.print so we can use the object PrintDocumentAdapter
 */

package android.print;

import android.app.Activity;
import android.content.Context;
import android.os.ParcelFileDescriptor;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import com.fennex.modules.HtmlToPdf;
import com.fennex.modules.NativeUtility;

import java.io.File;
import java.io.IOException;


/**
 * Converts HTML to PDF.
 * Can convert only one task at a time, any requests to do more conversions before
 * ending the current task are ignored.
 */

public class PdfConverter {
    //Dots per inch is set to 300, which is standard for printing PDF with images with high enough resolution on most printers
    private static final int DPI = 300;

    private static void printToPdf(Context context, final File file, String htmlString) {
        WebView mWebView = new WebView(context);
        mWebView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                final PrintDocumentAdapter documentAdapter = view.createPrintDocumentAdapter();
                documentAdapter.onStart();
                documentAdapter.onLayout(null, getPdfPrintAttrs(), null, new PrintDocumentAdapter.LayoutResultCallback() {
                    @Override
                    public void onLayoutFinished(PrintDocumentInfo info, boolean changed)  {
                        try {
                            documentAdapter.onWrite(new PageRange[]{PageRange.ALL_PAGES}, PdfConverter.getOutputFileDescriptor(file), null, new PrintDocumentAdapter.WriteResultCallback() {
                                @Override
                                public void onWriteFinished(PageRange[] pages) {
                                    NativeUtility.getMainActivity().runOnGLThread(new Runnable()
                                    {
                                        public void run()
                                        {
                                            HtmlToPdf.notifyPdfCreationSuccess(file.getName());
                                        }
                                    });
                                    documentAdapter.onFinish();
                                }                                
                                @Override
                                public void onWriteFailed(CharSequence error) {
                                    NativeUtility.getMainActivity().runOnGLThread(new Runnable()
                                    {
                                        public void run()
                                        {
                                            HtmlToPdf.notifyPdfCreationFailure(file.getName(), "WritePDFFailed");
                                        }
                                    });
                                    documentAdapter.onFinish();
                                }

                            });
                        }
                        catch (IOException e)
                        {
                            NativeUtility.getMainActivity().runOnGLThread(new Runnable()
                            {
                                public void run()
                                {
                                    HtmlToPdf.notifyPdfCreationFailure(file.getName(), "RenderFailed");
                                }
                            });
                            documentAdapter.onFinish();
                        }
                    }
                    @Override
                    public void  onLayoutFailed(CharSequence error)   {
                        NativeUtility.getMainActivity().runOnGLThread(new Runnable()
                        {
                            public void run()
                            {
                                HtmlToPdf.notifyPdfCreationFailure(file.getName(), "RenderFailed");
                            }
                        });
                        documentAdapter.onFinish();
                    }
                }, null);
            }
        });

        mWebView.loadDataWithBaseURL("file:///", htmlString, "text/html", "utf-8", null);
    }

    private static PrintAttributes getPdfPrintAttrs() {
        //PrintAttributes.Margins m = new PrintAttributes.Margins(0, 0, 0, 0);
        return new PrintAttributes.Builder()
                    .setMediaSize(PrintAttributes.MediaSize.ISO_A4)
                    .setResolution(new PrintAttributes.Resolution("RESOLUTION_ID", "RESOLUTION_ID", DPI, DPI))
                    .setMinMargins(PrintAttributes.Margins.NO_MARGINS)
                    .build();

    }

    public static void convert(final Activity activity, final String htmlString, final File file) {
        if (activity == null)
            throw new IllegalArgumentException("PdfConverter.convert: activity can't be null");
        if (file == null)
            throw new IllegalArgumentException("PdfConverter.convert: file can't be null");
        if (htmlString == null)
            throw new IllegalArgumentException("PdfConverter.convert: htmlString can't be null");

        // printToPdf method has to be runned on UI Thread in order to allocate a WebView
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                PdfConverter.printToPdf(activity, file, htmlString);
            }
        });
    }

    private static ParcelFileDescriptor getOutputFileDescriptor(File pdfFile) throws IOException {
        if (!pdfFile.createNewFile()) {
            throw new IOException("File.createNewFile() in PdfConverter.getOutputFileDescriptor has failed");
        }
        else {
            return ParcelFileDescriptor.open(pdfFile, ParcelFileDescriptor.MODE_WRITE_ONLY);
        }
    }
}
