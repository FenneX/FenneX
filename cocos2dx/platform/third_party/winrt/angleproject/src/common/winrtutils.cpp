//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.Microsoft::WR::
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// debug.cpp: Debugging utilities.

#include "common/winrtutils.h"


#include <wrl\implements.h>
#include <wrl\module.h>
#include <wrl\event.h>
#include <wrl\wrappers\corewrappers.h>
#include <windows.applicationmodel.core.h>
#include <math.h>

// check if WinRT
#if defined(WINAPI_FAMILY)
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#define ANGLE_PLATFORM_WINRT
#endif
#endif // !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#endif // #if defined(WINAPI_FAMILY)

// check if Windows Phone 8
#if defined(WINAPI_FAMILY)
#if defined(WINAPI_PARTITION_PHONE) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE)
#define ANGLE_PLATFORM_WP8
#ifndef ANGLE_PLATFORM_WINRT
#define ANGLE_PLATFORM_WINRT
#endif
#endif // #if defined(WINAPI_PARTITION_PHONE) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE)
#endif // #if defined(WINAPI_FAMILY)


#if !defined(ANGLE_PLATFORM_WP8)
#include <windows.ui.xaml.media.dxinterop.h>
#endif // !defined(ANGLE_PLATFORM_WP8)

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Windows::Graphics::Display;
using namespace ABI::Windows::UI::Core;

namespace winrt 
{

DisplayOrientations GetDisplayOrientation()
{
    ComPtr<IDisplayPropertiesStatics> dp;
    DisplayOrientations orientation = DisplayOrientations_None;

    if (SUCCEEDED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayProperties).Get(), dp.GetAddressOf()))) 
    {
        if (SUCCEEDED(dp->get_CurrentOrientation(&orientation))) 
        {
            return orientation;
        }
    }
    return DisplayOrientations_None;
}

static float GetLogicalDpi()
{
    ComPtr<IDisplayPropertiesStatics> dp;
    FLOAT dpi = 1.0;

    if (SUCCEEDED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayProperties).Get(), dp.GetAddressOf()))) 
    {
        if (SUCCEEDED(dp->get_LogicalDpi(&dpi)))
        {
            return dpi;
        }
    }
    return 1.0;
}


// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float convertDipsToPixels(float dips)
{
   static const float dipsPerInch = 96.0f;
   return floor(dips * GetLogicalDpi() / dipsPerInch + 0.5f); // Round to nearest integer.
}

bool isSwapChainBackgroundPanel(ComPtr<IUnknown> window)
{
#if defined(ANGLE_PLATFORM_WP8)
    return FALSE;
#else
    ComPtr<ISwapChainBackgroundPanelNative> panelNative;
    return S_OK == (window.Get())->QueryInterface(IID_PPV_ARGS(&panelNative));
#endif // #if defined(ANGLE_PLATFORM_WP8)
}


ComPtr<ICoreWindow> getCurrentWindowForThread()
{
    HRESULT result = S_OK;
    ComPtr<ICoreWindow> window;
    ComPtr<ICoreWindowStatic> staticWindow;
    result = GetActivationFactory(HStringReference(L"Windows.UI.Core.CoreWindow").Get(), staticWindow.GetAddressOf());
    if(SUCCEEDED(S_OK))
    {
        result = staticWindow->GetForCurrentThread(window.GetAddressOf());
    } 
    if(SUCCEEDED(S_OK))
    {
        return window;
    }
    return nullptr;
}

HRESULT getWindowDimensions(ComPtr<ICoreWindow> window, int& width, int& height)
{
    width = 0;
    height = 0;
    Rect bounds;
    HRESULT result = window->get_Bounds(&bounds);

    if(SUCCEEDED(result))
    {
        width = static_cast<int>(convertDipsToPixels(bounds.Width));    
        height = static_cast<int>(convertDipsToPixels(bounds.Height));   
    }
    return result;
}

HRESULT getCurrentWindowDimensions(int& width, int& height)
{
    width = 0;
    height = 0;
    HRESULT result = -1;

    ComPtr<ICoreWindow> window = getCurrentWindowForThread();
    if(window.Get() != nullptr)
    {
        result = getWindowDimensions(window,width, height);
    }

    return result;
}

std::string getTemporaryFilePath()
{
    HString hstrAppData;
    hstrAppData.Set(RuntimeClass_Windows_ApplicationModel_Package);
    ComPtr<IActivationFactory> pActivationFactory;
    HRESULT hr = GetActivationFactory(hstrAppData.Get(), &pActivationFactory);
    std::string result = "";

    ComPtr<IPackageStatics> packageStatics;
    hr = pActivationFactory.As(&packageStatics);	
    if(SUCCEEDED(hr))
    {
        ComPtr<IPackage> package;
        hr = packageStatics->get_Current(&package);
        if(SUCCEEDED(hr))
        {
            ComPtr<IStorageFolder> storageFolder;
            hr = package->get_InstalledLocation(&storageFolder);
            if(SUCCEEDED(hr))
            {
                ComPtr<IStorageItem> storageItem;
                hr = storageFolder.As(&storageItem);
                if(SUCCEEDED(hr))
                {
                    HSTRING hsPath;
                    storageItem->get_Path(&hsPath);
                    std::wstring t = std::wstring(WindowsGetStringRawBuffer(hsPath,0));
                    result = std::string(t.begin(),t.end());
                }
            }
        }
    }

    return result;
}

}
