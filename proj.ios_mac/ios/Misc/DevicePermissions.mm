//
//  DevicePermissions.m
//  Voice iOS
//
//  Created by Pierre Bertinet on 15/11/2018.
//

#include "DevicePermissions.h"


bool DevicePermissions::hasPermissionInternal(Permission permission)
{
    return true;
}

bool DevicePermissions::hasPermissionInternal(const std::string& permission)
{
    return true;
}

bool DevicePermissions::requestPermission(Permission permission)
{
    return true;
}

bool DevicePermissions::requestPermission(const std::string& permission)
{
    return true;
}
