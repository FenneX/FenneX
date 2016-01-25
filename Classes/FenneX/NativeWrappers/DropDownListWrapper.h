/****************************************************************************
 Copyright (c) 2013-2015 Auticiel SAS
 
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

#ifndef FenneX_DropDownListWrapper_h
#define FenneX_DropDownListWrapper_h

#include "Shorteners.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_FENNEX;

class DropDownListWrapper: public Ref
{
public:
    DropDownListWrapper();
    ~DropDownListWrapper();
    void setPossibleValues(std::vector<std::string> values);
    void setTitle(const std::string& title);
    void setIdentifier(int identifier);
    void show();
    

private:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Android Object
    jobject delegate;
#else
    // iOS Object
    void* delegate;
#endif
};

static inline void notifySelectionDone(int identifier, std::string value)
{
    if(identifier != -1)
    {
        DelayedDispatcher::eventAfterDelay("DropDownListSelectionDone", DcreateP(Icreate(identifier), Screate("Identifier"), Screate(value), Screate("SelectedValue"), NULL), 0.01);
    }
}

#endif
