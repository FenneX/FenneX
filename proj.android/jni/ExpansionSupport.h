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

//See Java class in expansion-src for instructions about how to setup your project to enable ExpansionSupport
//If you don't do it, your project will crash because of missing Java classes

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

/* Get the expansion file name
 * main or patch : pass true for main, false for patch expansion
 * return the file name if everything went correctly
 * return NULL if the expansion was not declared in the app
 */

std::string getExpansionFileName(bool main);

/* Get the full path of the expansion
 * main or patch : pass true for main, false for patch expansion
 * return the absolute path of the file if everything went correctly
 * return NULL if the expansion was not declared in the app
 */

std::string getExpansionFileFullPath(bool main);

/* Get true if the expansion exists and is downloaded, otherwise false
 * main or patch : pass true for main, false for patch expansion
 */
bool expansionExists(bool main);

static inline void notifyServiceConnected()
{
    DelayedDispatcher::eventAfterDelay("DownloadServiceConnected", Value(), 0.01);
}

static inline void notifyDownloadStateChanged(std::string status, int code)
{
    DelayedDispatcher::eventAfterDelay("DownloadStateChanged", Value(ValueMap({{"Status", Value(status)}, {"Code", Value(code)}})), 0.01);
}

static inline void notifyDownloadCompleted()
{
    DelayedDispatcher::eventAfterDelay("DownloadCompleted", Value(), 0.01);
}

static inline void notifyDownloadProgress(float percent, long totalSize)
{
    DelayedDispatcher::eventAfterDelay("DownloadProgressUpdate", Value(ValueMap({{"Percent", Value(percent)}, {"TotalSize", Value((int)totalSize)}})), 0.01);
}

#endif
