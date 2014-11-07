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

#ifndef Expansionsupport_h
#define Expansionsupport_h

#include "FenneX.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

/** check if the expansion files are downloaded
 * if they are, the app can start normally
 * if they aren't the app must show a download screen
 * the ExpansionSupport will start throwing notifications to update the app :
 * - DownloadServiceConnected (which have no usage yet)
 * - DownloadStateChanged, with Status (String) that you can print and Code (int from -1 to 5, excluding 0). Refer to ExpansionSupport.java for more informations
 * - DownloadCompleted when the download is finished and you can start the real app
 * - DownloadProgressUpdate from time to time, with Percent (float) and TotalSize (int) in bytes
 */
bool checkExpansionFiles();

/* Uncompress a file in an expansion
 * main or patch : pass true for main, false for patch expansion
 * return the absolute path of the file if everything went correctly
 * return NULL if there was a problem, "NOTDOWNLOADED" if the expansion is not properly downloaded (in that case, retry checkExpansionFiles)
 */
std::string getExpansionFileFullPath(bool main);

static inline void notifyServiceConnected()
{
    performNotificationAfterDelay("DownloadServiceConnected", Dcreate(), 0.01);
}

static inline void notifyDownloadStateChanged(const char* status, int code)
{
    performNotificationAfterDelay("DownloadStateChanged", DcreateP(Screate(status), Screate("Status"), Icreate(code), Screate("Code"), NULL), 0.01);
}

static inline void notifyDownloadCompleted()
{
    performNotificationAfterDelay("DownloadCompleted", Dcreate(), 0.01);
}

static inline void notifyDownloadProgress(float percent, long totalSize)
{
    performNotificationAfterDelay("DownloadProgressUpdate", DcreateP(Fcreate(percent), Screate("Percent"), Icreate((int)totalSize), Screate("TotalSize"), NULL), 0.01);
}

#endif
