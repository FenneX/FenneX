//
//  DropDownListWrapper.m
//  FenneX
//
//  Created by Pierre Bertinet on 21/09/15.
//
//

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
