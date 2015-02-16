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
USING_NS_CC;
#include "RawObject.h"
#include "InputLabelProtocol.h"
#include "LabelTTF.h"
#include "CustomInput.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//Throw a TextAdded event when keyboard is closed and KeyboardOpened when it's opened for this label
//Note: since V3, the InputLabel must not have an OpenKeyboard event. This is handled by its delegate directly (as a CCControlButton). Otherwise, there will be a bug on Android where the keyboard is opened 2 times
class InputLabel : public RawObject, public ui::EditBoxDelegate, public IMEDelegate, public InputLabelProtocol
{
    CC_SYNTHESIZE_READONLY(LabelTTF*, linkTo, LinkTo);
    CC_SYNTHESIZE_READONLY(CustomInput*, originalInfos, OriginalInfos);
public:
    virtual void setLinkTo(LabelTTF* var);
    
    cocos2d::Rect getBoundingBox();
    virtual void setPlaceHolderColor(Color3B color);
    virtual const char* getLabelValue();
    virtual void setLabelValue(const char* value);
    virtual Node* getNode();
    
    InputLabel();
    InputLabel(ui::Scale9Sprite* sprite);
    InputLabel(const char* placeHolder, const char* fontName, int fontSize, Vec2 location, ui::EditBox::InputMode inputMode = ui::EditBox::InputMode::ANY, int maxChar = -1, cocos2d::Size dimensions = cocos2d::Size(0,0), TextHAlignment format = TextHAlignment::CENTER);
    //TODO : add keyboard type and max char
    ~InputLabel();
    
    virtual void update(float deltaTime);
    /*
     virtual bool onTextFieldInsertText(CCTextFieldTTF * sender, const char * text, int nLen);
     virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * sender, const char * delText, int nLen);
     virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * sender);*/
    
    virtual void setEventName(const char* eventName);
    
    virtual void openKeyboard(EventCustom* event);
    virtual void closeKeyboard(EventCustom* event);
    virtual void disableInputs(EventCustom* event);
    virtual void enableInputs(EventCustom* event);
    virtual void exitBoxEditingWillBegin(ui::EditBox* editBox);
    virtual void editBoxEditingDidBegin(ui::EditBox* editBox);
    virtual void editBoxReturn(ui::EditBox* editBox);
    virtual void editBoxEditingDidEnd(ui::EditBox* editBox);
    virtual void editBoxTextChanged(ui::EditBox* editBox, const std::string& text);
    
    
    bool numbersOnly; //false by default
    
    bool isUnedited();
    void setInitialText(CCString* text);
    
    void setIsPassword();
    
    //Prevent the keyboard from opening, return a key to unlock it. The key will always be positive
    static int preventKeyboardOpen();
    //Release a lock on keyboard opening.
    static void releaseKeyboardLock(int key);
    //Release all locks. It should only be used during a Scene Switch or very particular cases
    static void releaseAllKeyboardLocks();
protected:
    ui::EditBox* delegate;
    bool isOpened;
    bool textDirty;
    bool isPassword; //false by default
    CCString* passwordText; //Since a password doesn't show its text, this field is use to keep the actual String value
    
    CCString* initialText; //Used to know if the current text is the placeholder
    /*
     //Used for automatic scaling of font according text
     int maxHeight;
     bool textChanged;
     float originalFontSize;*/
    Vector<EventListenerCustom*> listeners;
};
NS_FENNEX_END

#endif /* defined(__FenneX__InputLabel__) */
