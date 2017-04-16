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

#include "CustomInput.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN
bool CustomInput::init()
{
    maxChar = -1;
    fontSize = -1;
    inputMode = -1;
    return ui::Scale9Sprite::init();
}

bool CustomInput::onAssignCCBCustomProperty(Ref* pTarget, const char* pMemberVariableName, const cocos2d::Value& pCCBValue)
{
    CustomInput* target = dynamic_cast<CustomInput*>(pTarget);
    if(pCCBValue.getType() == cocos2d::Value::Type::INTEGER && strcmp(pMemberVariableName, "maxChar") == 0)
    {
        target->setMaxChar(pCCBValue.asInt());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::INTEGER && strcmp(pMemberVariableName, "fontSize") == 0)
    {
        target->setFontSize(pCCBValue.asInt());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::INTEGER && strcmp(pMemberVariableName, "InputMode") == 0)
    {
        target->setInputMode(pCCBValue.asInt());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::STRING && strcmp(pMemberVariableName, "placeHolder") == 0)
    {
        target->setPlaceHolder(pCCBValue.asString());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::STRING && strcmp(pMemberVariableName, "fontName") == 0)
    {
        target->setFontName(pCCBValue.asString());
    }
    else
    {
        CustomBaseNode::onAssignCCBCustomProperty(pTarget, pMemberVariableName, pCCBValue);
    }
    return false;
}
NS_FENNEX_END
