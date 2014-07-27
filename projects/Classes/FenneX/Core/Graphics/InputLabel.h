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

#ifndef __FenneX__InputLabel__
#define __FenneX__InputLabel__

#include "Logs.h"
#include "cocos2d.h"
#include "../extensions/ExtensionMacros.h"
#include "../extensions/GUI/CCEditBox/CCEditBox.h"
USING_NS_CC;
USING_NS_CC_EXT;
#include "RawObject.h"
#include "InputLabelProtocol.h"
#include "LabelTTF.h"
#include "CustomInput.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//Throw a TextAdded event when keyboard is closed and KeyboardOpened when it's opened for this label
class InputLabel : public RawObject, public CCEditBoxDelegate, public CCIMEDelegate, public InputLabelProtocol
{
    CC_SYNTHESIZE_READONLY(LabelTTF*, linkTo, LinkTo);
    CC_SYNTHESIZE_READONLY(CustomInput*, originalInfos, OriginalInfos);
public:
    virtual void setLinkTo(LabelTTF* var);
    
    CCRect getBoundingBox();
    virtual void setPlaceHolderColor(ccColor3B color);
    virtual const char* getLabelValue();
    virtual void setLabelValue(const char* value);
    virtual CCNode* getNode();
    
    InputLabel();
    InputLabel(CCScale9Sprite* sprite);
    InputLabel(const char* placeHolder, const char* fontName, int fontSize, CCPoint location, EditBoxInputMode inputMode = kEditBoxInputModeAny, int maxChar = -1, CCSize dimensions = CCSizeZero, CCTextAlignment format = kCCTextAlignmentCenter);
    //TODO : add keyboard type and max char
    ~InputLabel();
    
    virtual void update(float deltaTime);
    /*
     virtual bool onTextFieldInsertText(CCTextFieldTTF * sender, const char * text, int nLen);
     virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * sender, const char * delText, int nLen);
     virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * sender);*/
    
    virtual void setEventName(const char* eventName);
    
    virtual void openKeyboard(CCObject* obj);
    virtual void closeKeyboard(CCObject* obj);
    virtual void disableInputs(CCObject* obj);
    virtual void enableInputs(CCObject* obj);
    virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    virtual void editBoxReturn(CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    
    
    bool numbersOnly;
    
    bool isUnedited();
    void setInitialText(CCString* text);
    
protected:
    CCEditBox* delegate;
    bool isOpened;
    bool textDirty;
    
    CCString* initialText; //Used to know if the current text is the placeholder
    /*
     //Used for automatic scaling of font according text
     int maxHeight;
     bool textChanged;
     float originalFontSize;*/
};
NS_FENNEX_END

#endif /* defined(__FenneX__InputLabel__) */
