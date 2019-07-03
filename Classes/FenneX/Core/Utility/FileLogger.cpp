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

#include "FileLogger.h"
#include "FenneX.h"
#include "NativeUtility.h"
#include <iomanip>

USING_NS_CC;
USING_NS_FENNEX;
USING_NS_AC;

static std::string currentFileDate = "";
static std::string logsPath = "";
static int logFilePreservationDelay = FILE_LOGGER_NEVER_DELETE;
static const std::string logFileExtension = ".log";
static const std::string logFileDateFormat = "%d_%m_%Y_%H_%M";

void FileLogger::setup(std::string _logsPath, int deleteAfter)
{
    if(!currentFileDate.empty()) {
        CCASSERT(1, "Trying to setup FileLogger several time. Previous setup date: " + currentFileDate);
    }
    
    time_t now = time(nullptr);
    char buff[100];
    strftime(buff, 100, logFileDateFormat.c_str(), localtime(&now));
    currentFileDate = buff;
    
    logsPath = _logsPath;
    if(!stringEndsWith(logsPath, "/")) logsPath += "/";
    logFilePreservationDelay = deleteAfter;
    
    removeOldLogs();
}

void FileLogger::debug(std::string module, std::string message)
{
    _log(Severity::DEBUG, module, message);
}

void FileLogger::info(std::string module, std::string message)
{
    _log(Severity::INFO, module, message);
}

void FileLogger::warning(std::string module, std::string message)
{
    _log(Severity::WARNING, module, message);
}

void FileLogger::error(std::string module, std::string message)
{
    _log(Severity::ERROR, module, message);
}

void FileLogger::critical(std::string module, std::string message)
{
    _log(Severity::CRITICAL, module, message);
}

std::string FileLogger::getLogsPath()
{
    return logsPath;
}

std::string FileLogger::currentFilename()
{
    return currentFileDate + logFileExtension;
}

void FileLogger::removeOldLogs()
{
    if(logFilePreservationDelay == FILE_LOGGER_NEVER_DELETE) return;
    
    std::vector<std::string> logFiles = getFilesInFolder(logsPath + getPackageIdentifier() + "/");
    time_t now = time(nullptr);
    for(std::string file : logFiles)
    {
        std::string fileFullPath = logsPath + getPackageIdentifier() + "/" + file;
        if(now > getFileLastModificationDate(fileFullPath) + logFilePreservationDelay)
        {
            FileUtils::getInstance()->removeFile(fileFullPath);
        }
    }
}

std::string FileLogger::getFilePath()
{
    FileUtils::getInstance()->createDirectory(logsPath);
    FileUtils::getInstance()->createDirectory(logsPath + getPackageIdentifier() + "/");
    return logsPath + getPackageIdentifier() + "/" + currentFilename();
}

std::string FileLogger::severityPrint(Severity severity)
{
    switch(severity)
    {
        case Severity::DEBUG:
            return "DEBUG";
        case Severity::INFO:
            return "INFO";
        case Severity::WARNING:
            return "WARNING";
        case Severity::ERROR:
            return "ERROR";
        case Severity::CRITICAL:
            return "CRITICAL";
    }
}

std::string FileLogger::currentDatePrint()
{
    time_t now = time(nullptr);
    char buff[100];
    strftime(buff, 100, "%d/%m/%Y %T", localtime(&now));
    return buff;
}

//Unsaved logs are kept in there until they can actually be written to file.
//Sometimes, the log file just won't open right when needed
std::vector<std::string> unsavedLogs;

void FileLogger::_log(Severity severity, std::string module, std::string message)
{
    log("%s: %s", module.c_str(), message.c_str());
    
    //FileLogger isn't setup, don't try to save to file
    if(currentFileDate.empty()) return;
    
    std::string fullPath = getFilePath();
    
    std::string logStr = currentDatePrint() + " - " + severityPrint(severity) + " - " + module + " - " + message + "\n";
    
    //Compose the full string that needs to be saved to file with logs that couldn't be saved first
    std::string dataStr;
    for(std::string unsaved : unsavedLogs)
    {
        dataStr += unsaved;
    }
    dataStr += logStr;
    // Code copied and adapted from cocos2dx FileUtils: we want to append, and we don't need the result
    Data data;
    data.fastSet((unsigned char*)dataStr.c_str(), dataStr.size());
    // Since this is a log file, we only append
    const char* mode = "ab";
    FILE *fp = fopen(FileUtils::getInstance()->getSuitableFOpen(fullPath).c_str(), mode);
    if(fp != nullptr)
    {
        fwrite(data.getBytes(), data.getSize(), 1, fp);
        fclose(fp);
        unsavedLogs.clear();
    }
    else
    {
        //If we cannot open the file, save the log for future saving.
        unsavedLogs.push_back(logStr);
        log("FileLogger could not open log file");
    }
    data.fastSet(nullptr, 0);
}
