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

package com.fennex.modules;

import android.net.Uri;

public interface MainActivityUtility
{
	//Those responder will be called on Activity result
	public void addResponder(ActivityResultResponder responder);
	public void removeResponder(ActivityResultResponder responder);
	
	//Return the public key for InApps
	public String getPublicKey();
	
	//return a resource ID in the application's package of the drawble to use, for the NotificationService
	public int getSmallIcon();
	public Uri getUriFromFileName(String fileName);
	
	//Returns true if the in-app product should be consumed before being returned. Usually, it's false, because you want to keep premium state
	public boolean isConsumable(String productID);

    //Return the ID of the splash screen layout, or -1 for no splash screen
    public int getSplashScreenLayout();
    //Return the ID of the splash screen image, or -1 for no splash screen
    public int getSplashScreenImageID();
}
