/****************************************************************************
 Copyright (c) 2013-2019 Auticiel SAS
 
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

#ifndef FileLogger_h
#define FileLogger_h

#include "cocos2d.h"

/*
 FileLogger is used to log some informations directly to a file,
 so that logs can be analyzed after the fact.
 
 A folder is created for current app package, and there is a log file per session, identified by the setup date and time
 If a log is sent while FileLogger isn't setup, it will be logged to console, but not to file
 
 Any file older than the configured delay is removed
 */
#define FILE_LOGGER_NEVER_DELETE -1
class FileLogger
{
public:
    static void setup(std::string logsPath, int deleteAfter = FILE_LOGGER_NEVER_DELETE);
    enum class Severity
    { // Mirror Django severity levels
        DEBUG, // Low level system information for debugging purposes
        INFO, // General system information
        WARNING, // Information describing a minor problem that has occurred
        ERROR, // Information describing a major problem that has occurred
        CRITICAL, // Information describing a critical problem that has occurred
    };
    static void debug(std::string module, std::string message);
    static void info(std::string module, std::string message);
    static void warning(std::string module, std::string message);
    static void error(std::string module, std::string message);
    static void critical(std::string module, std::string message);
    
    static std::string getLogsPath();
    static std::string currentFilename();
private:
    static void removeOldLogs();
    static std::string getFilePath();
    static std::string severityPrint(Severity severity);
    static std::string currentDatePrint();
    static void _log(Severity severity, std::string module, std::string message);
};


#endif /* FileLogger_h */
