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

#include "DropDownListImplIOS.h"
#include "DropDownListWrapper.h"
#import "platform/ios/CCEAGLView-ios.h"

#define TYPED_DELEGATE ((DropDownListImplIOS*)delegate)

DropDownListWrapper::DropDownListWrapper()
{
    delegate = [[DropDownListImplIOS alloc] init];
}

DropDownListWrapper::~DropDownListWrapper()
{
    [TYPED_DELEGATE release];
    delegate = nil;
}

void DropDownListWrapper::setPossibleValues(std::vector<std::string> values)
{
    if(delegate != NULL)
    {
        NSMutableArray *array = [NSMutableArray new];
        for(std::string value : values)
        {
            [array addObject:[NSString stringWithUTF8String:value.c_str()]];
        }
        [TYPED_DELEGATE setPossibleValues:[array copy]];
    }
}

void DropDownListWrapper::setTitle(const std::string& title)
{
    if(delegate != NULL)
    {
        [TYPED_DELEGATE setTitle:[NSString stringWithUTF8String:title.c_str()]];
    }

}

void DropDownListWrapper::setIdentifier(int identifier)
{
    if(delegate != NULL)
    {
        [TYPED_DELEGATE setIdentifier:identifier];
    }
}

void DropDownListWrapper::show()
{
    [TYPED_DELEGATE show];
}
