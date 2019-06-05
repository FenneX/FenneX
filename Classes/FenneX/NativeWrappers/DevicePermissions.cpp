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

#include "DevicePermissions.h"
#include "cocos2d.h"

static std::map<Permission, std::vector<std::pair<std::function<void()>, std::function<void()>>>> onGoingRequest;
// Don't ask for permission if you already have the information
static std::vector<Permission> acceptedPermission = {};

bool DevicePermissions::hasPermission(Permission permission)
{
    if(std::find(acceptedPermission.begin(), acceptedPermission.end(), permission) != acceptedPermission.end())
    {
        return true;
    }
    if(hasPermissionInternal(permission))
    {
        acceptedPermission.push_back(permission);
        return true;
    }
    return false;
}

void DevicePermissions::ensurePermission(Permission permission, const std::function<void()> funcOnSuccess, const std::function<void()> funcOnDeny)
{
    if(hasPermission(permission))
    {
        funcOnSuccess();
    }
    else if(onGoingRequest.find(permission) != onGoingRequest.end())
    {
        // We are already requesting this permission, just add the function and launch all of them on respond
        onGoingRequest.at(permission).push_back({std::make_pair(funcOnSuccess, funcOnDeny)});
    }
    else if(requestPermission(permission))
    {
        funcOnSuccess();
        acceptedPermission.push_back(permission);
    }
    else
    {
        onGoingRequest.insert({permission, {std::make_pair(funcOnSuccess, funcOnDeny)}});
    }
}

void DevicePermissions::permissionRequestEnded(Permission permission, bool result)
{
    Director::getInstance()->getRunningScene()->resume();
    if(onGoingRequest.find(permission) != onGoingRequest.end())
    {
        if(result)
        {
            for(auto func : onGoingRequest.at(permission)) func.first();
            acceptedPermission.push_back(permission);
        }
        else
        {
            for(auto func : onGoingRequest.at(permission)) func.second();
        }
        onGoingRequest.erase(permission);
    }
}
