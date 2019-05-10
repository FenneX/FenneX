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


package com.fennex.modules;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.os.StrictMode;
import android.print.PdfConverter;
import java.io.File;

public class HtmlToPdf {
    public native static void notifyPdfCreationFailure(String pdfName, String failureCause);
    public native static void notifyPdfCreationSuccess(String pdfName);


    //Checking if a file with the same name already exists, if it does, adding "(n).pdf" to avoid replacing the existing file
    private static String findAvailableName(String fileName)
    {
        File file = new File(Environment.getExternalStoragePublicDirectory(android.os.Environment.DIRECTORY_DOCUMENTS), fileName);
        if (file.exists()) {
            String duplicateFileName = fileName;
            int i = 1;
            while (file.exists()) {
                String n = "(" + Integer.toString(i) + ")";
                duplicateFileName = fileName.replace(".pdf", n + ".pdf");
                file = new File(Environment.getExternalStoragePublicDirectory(android.os.Environment.DIRECTORY_DOCUMENTS), duplicateFileName);
                i++;
            }
            fileName = duplicateFileName;
        }
        return fileName;
    }

    @SuppressWarnings("unused")
    private static void convert(String htmlString, final String fileName, int pageSize) {
        File destinationFile;
        File path = new File(Environment.getExternalStorageDirectory(), Environment.DIRECTORY_DOCUMENTS);
        if (!path.exists()) {
            if (!path.mkdirs()) {
                NativeUtility.getMainActivity().runOnGLThread(() -> 
                    HtmlToPdf.notifyPdfCreationFailure(fileName, "CreateDirectoryFailed"));
                return;
            }
        }
        destinationFile = new File(path, findAvailableName(fileName));
        PdfConverter.convert(NativeUtility.getMainActivity(),
                    htmlString,
                    destinationFile,
                    pageSize);
        }

    @SuppressWarnings("unused")
    private static void openPDFWithApp(String fileName)
        {
            // TODO : Change to use a FileProvider
            StrictMode.VmPolicy.Builder builder = new StrictMode.VmPolicy.Builder();
            StrictMode.setVmPolicy(builder.build());
            Intent target = new Intent(Intent.ACTION_VIEW);
            target.setDataAndType(Uri.fromFile(new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS) + "/" + fileName)),"application/pdf");

            Intent intent = Intent.createChooser(target, "Open File");
            try {
                NativeUtility.getMainActivity().startActivity(intent);
            } catch (ActivityNotFoundException e) {
                NativeUtility.getMainActivity().runOnGLThread(() -> {
                    notifyPdfCreationFailure(fileName, "OpenPreviewFailed");
                });
            }
        }
}
