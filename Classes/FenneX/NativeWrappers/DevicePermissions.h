/****************************************************************************
 Copyright (c) 2013-2016 Auticiel SAS
 
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

#ifndef DevicePermissions_h
#define DevicePermissions_h

#include "FenneX.h"

NS_FENNEX_BEGIN

//Syncronized with Java getPermissionString()
enum class Permission {
    STORAGE = 0,  // Use the devices storage
    MICROPHONE = 1, // Use the device microphone
    CAMERA = 2, // Use the device Camera
    PHONE_STATE = 3, // get the phone state information
};

/**
 * This class is made to help you with dealing with new android permission (API 26)
 **/
class DevicePermissions
{
public:
    // Return wether or not the permission is granted. Does not launch any request (return true on iOS)
    static bool hasPermission(Permission permission);
    static bool hasPermission(const std::string& permission);
    
    /**
     * This function will ensure the permission is granted before launching the functions in parameters
     * If the permission is not granted yet, it'll launch a request for permission and when resulting :
     * - If the user granted it, it'll launch funcOnSuccess
     * - If the user denied it, it'll launch funcOnDeny
     * It the permission is already granted, it'll launch funcOnSuccess synchronously
     * The request will pause the app so be carefull not to use it during initialisation
     * on iOS the function will always be launched synchronously and no request will be launched
     */
    static void ensurePermission(Permission permission, const std::function<void()> funcOnSuccess, const std::function<void()> funcOnDeny);
    /**
     * This function works the same way as the function above, it only grant the possibility to use custom permission
     */
    static void ensurePermission(const std::string& permission, const std::function<void()> funcOnSuccess, const std::function<void()> funcOnDeny);
    
    // Internal use only, need to be public for Android notify C-function
    static void permissionRequestEnded(Permission permission, bool result);
    static void permissionRequestEnded(const std::string& permission, bool result);
private:
    static bool hasPermissionInternal(Permission permission);
    static bool hasPermissionInternal(const std::string& permission);
    // This will return true if the permission is already granted. I'll return false and launch the asking popup otherwise
    static bool requestPermission(Permission permission);
    static bool requestPermission(const std::string& permission);
    
};

NS_FENNEX_END

#endif /* DevicePermissions_h */
