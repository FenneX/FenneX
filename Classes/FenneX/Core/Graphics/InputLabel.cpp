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

#include "InputLabel.h"
#include "Shorteners.h"
#include "CustomInput.h"
#include "AppMacros.h"

NS_FENNEX_BEGIN
void InputLabel::setEventName(const char* var)
{
    CCLOG("Warning : changing eventName for InputLabel, may not be able to open keyboard");
    if(var == NULL) eventName.clear();
    else if (eventName.compare(var))
        eventName = var;
}

CCRect InputLabel::getBoundingBox()
{
    return CCRect(this->getNode()->getPositionX(), this->getNode()->getPositionY(), this->getNode()->getContentSize().width, this->getNode()->getContentSize().height);
}

CCNode* InputLabel::getNode()
{
    CCAssert(delegate != NULL, "InputLabel getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}

void InputLabel::setPlaceHolderColor(ccColor3B color)
{
    delegate->setPlaceholderFontColor(color);
}

void InputLabel::setLabelValue(const char* value)
{
    if(linkTo != NULL)
    {
        linkTo->setLabelValue(value);
        if(!linkTo->getNode()->isVisible() && delegate->isVisible())
        {
            delegate->setText(value);
        }
    }
    else
    {
        delegate->setText(value);
    }
}

const char* InputLabel::getLabelValue()
{
    return isPassword && passwordText != NULL ? passwordText->getCString() : linkTo != NULL ? (isUnedited() ? "" : linkTo->getLabelValue()) : delegate->getText();
}

InputLabel::InputLabel() : delegate(NULL)
{
    linkTo = NULL;
    isOpened = false;
    originalInfos = NULL;
    isPassword = false;
    passwordText = NULL;
}

InputLabel::InputLabel(Scale9Sprite* sprite)
{
    linkTo = NULL;
    isOpened = false;
    textDirty = false;
    initialText = NULL;
    originalInfos = NULL;
    isPassword = false;
    passwordText = NULL;
    name = "CustomInputLabel";
    sprite->retain();
    sprite->removeFromParentAndCleanup(true);
    CCPoint position = sprite->getPosition();
    //CCLOG("prefered size : %f, %f, content size : %f, %f, insets : %f, %f, %f, %f, position : %f, %f", sprite->getPreferredSize().width, sprite->getPreferredSize().height, sprite->getContentSize().width, sprite->getContentSize().height, sprite->getInsetBottom(), sprite->getInsetTop(), sprite->getInsetLeft(), sprite->getInsetRight(), position.x, position.y);
    CCLOG("sprite position before : %f, %f", position.x, position.y);
    //The content size is fucked up (way higher than it's supposed to be, probably a problem with cocosBuilder), so always use the prefered size, which is the real sprite size
    sprite->setContentSize(sprite->getPreferredSize());
    
    delegate = EditBox::create(CCSizeMake(sprite->getContentSize().width,
                                            sprite->getContentSize().height),
                                 sprite);
    delegate->retain();
    delegate->setFontColor(sprite->getColor());
    delegate->setColor(ccWHITE);
    delegate->setDelegate(this);
    delegate->setOpacity(0);
    
    //You HAVE to set the contentSize again, because CCControlButton do some weird thing on the UIEditbox content size which makes it work only on 1024x768
    //delegate->setContentSize(sprite->getPreferredSize());
    this->setPosition(position);
    CCLOG("delegate position after : %f, %f", delegate->getPosition().x, delegate->getPosition().y);
    delegate->setInputMode(EditBox::InputMode::SINGLE_LINE);
    delegate->setReturnType(EditBox::KeyboardReturnType::DONE);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::openKeyboard), "OpenKeyboard", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::closeKeyboard), "CloseKeyboard", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::disableInputs), "DisableInputs", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::enableInputs), "EnableInputs", NULL);
    
    CustomInput* input = dynamic_cast<CustomInput*>(sprite);
    if(input != NULL)
    {
        if(input->getPlaceHolder() != NULL)
        {
            delegate->setPlaceHolder(input->getPlaceHolder()->getCString());
            this->setInitialText(input->getPlaceHolder());
        }
        numbersOnly = input->getNumbersOnly();
        if(input->getMaxChar() != -1)
        {
            delegate->setMaxLength(input->getMaxChar());
        }
        if(input->getFontSize() > 0 && input->getFontName() != NULL)
        {
            delegate->setFontName(input->getFontName()->getCString());
            delegate->setFontSize(input->getFontSize());
        }
        originalInfos = input;
    }
}

InputLabel::InputLabel(const char* placeHolder, const char* fontName, int fontSize, CCPoint location, EditBox::InputMode inputMode, int maxChar, CCSize dimensions, CCTextAlignment format)
{
    linkTo = NULL;
    isOpened = false;
    textDirty = false;
    originalInfos = NULL;
    isPassword = false;
    passwordText = NULL;
    name = placeHolder;
    Scale9Sprite* sprite = Scale9Sprite::create("green_edit.png", CCRect(0, 0, 43, 38), CCRect(4, 3, 35, 32));
    sprite->setPreferredSize(CCSize(43, 38));
    sprite->setOpacity(0);
    delegate = EditBox::create(dimensions, sprite);
    delegate->retain();
    if(strlen(placeHolder) > 0)
    {
        CCString* placeholderWithBrackets = ScreateF("<%s>", placeHolder);
        delegate->setPlaceHolder(placeholderWithBrackets->getCString());
        this->setInitialText(placeholderWithBrackets);
    }
    delegate->setFontColor(ccBLACK);
    delegate->setDelegate(this);
    
    this->setPosition(location);
    delegate->setInputMode(inputMode);
    numbersOnly = inputMode ==  EditBox::InputMode::DECIMAL;
    delegate->setReturnType(EditBox::KeyboardReturnType::DONE);
    if(maxChar != -1)
    {
        delegate->setMaxLength(maxChar);
    }
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::openKeyboard), "OpenKeyboard", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::closeKeyboard), "CloseKeyboard", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::disableInputs), "DisableInputs", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InputLabel::enableInputs), "EnableInputs", NULL);
    
}

InputLabel::~InputLabel()
{
    if(isOpened)
    {
        isOpened = false; //set the flag before so the TextFieldDetach isn't processed (it's a cancel)
        delegate->detachWithIME();
    }
    if(initialText != NULL)
    {
        initialText->release();
        initialText = NULL;
    }
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
    delegate->release();
    if(originalInfos != NULL)
    {
        originalInfos->release();
    }
    if(passwordText != NULL) passwordText->release();
#if VERBOSE_DEALLOC
    CCLog("Dealloc InputLabel %s", name.c_str());
#endif
}


void InputLabel::setLinkTo(LabelTTF* var)
{
    linkTo = var;
    delegate->setFontColor(((Label*)var->getNode())->getColor());
    delegate->setPlaceHolder("");
    delegate->setText("");
    this->setInitialText(Screate(var->getLabelValue()));
}


void InputLabel::update(float deltaTime)
{
    if(textDirty && numbersOnly)
    {
        //remove non-numeric characters
        CCLOG("checking characters ...");
        const char* label = this->getLabelValue();
        char* result = (char*)malloc((strlen(label)+1) * sizeof(char));
        int resultPos = 0;
        for (int i = 0; i < strlen(label); i++)
        {
            char ch = label[i];
            if(ch >= '0' && ch <= '9')
            {
                result[resultPos] = ch;
                resultPos++;
            }
        }
        result[resultPos] = '\0';
        this->setLabelValue(result);
        free(result);
        CCLOG("done");
    }
    textDirty = false;
}

void InputLabel::openKeyboard(CCObject* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    if((isKindOfClass(infos->objectForKey("Sender"), CCInteger)
        && TOINT(infos->objectForKey("Sender")) == identifier)
       || (isKindOfClass(infos->objectForKey("Target"), CCInteger)
           && TOINT(infos->objectForKey("Target")) == identifier))
    {
        CCLOG("Open InputLabel keyboard");
        delegate->touchDownAction(this, cocos2d::ui::Widget::TouchEventType::ENDED);//open keyboard by simulating a touch inside
        isOpened = true;
    }
}

void InputLabel::closeKeyboard(CCObject* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    if((isKindOfClass(infos->objectForKey("Sender"), CCInteger)
        && TOINT(infos->objectForKey("Sender")) == identifier)
       || (isKindOfClass(infos->objectForKey("Target"), CCInteger)
           && TOINT(infos->objectForKey("Target")) == identifier))
    {
        CCLOG("Close InputLabel keyboard");
        delegate->detachWithIME();
        delegate->closeKeyboard(); //Force close the keyboard
    }
}

void InputLabel::disableInputs(CCObject* obj)
{
    delegate->setEnabled(false);
}

void InputLabel::enableInputs(CCObject* obj)
{
    delegate->setEnabled(true);
}


void InputLabel::editBoxEditingDidBegin(EditBox* editBox)
{
    if(!isOpened && delegate->isEnabled())
    {
        isOpened = true;
        CCLOG("editing did begin InputLabel");
        if(linkTo != NULL)
        {
            //A password should be cleared when you begin editing. That's the default behavior on iOS, and we can't easily go around anyway, since UITextField.secureEntry force this behavior
            delegate->setText(isUnedited() || isPassword ? "" : linkTo->getLabelValue());
            if(isPassword)
            {
                linkTo->setLabelValue("");
                IFEXIST(passwordText)->autorelease();
                passwordText = new CCString();
            }
            linkTo->getNode()->setVisible(false);
        }
        CCNotificationCenter::sharedNotificationCenter()->postNotification("InputLabelBeginEdit", DcreateP(Icreate(identifier), Screate("Sender"), NULL));
    }
}


void InputLabel::editBoxReturn(EditBox* editBox)
{
    CCLOG("EditBoxReturn : Close InputLabel keyboard");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("InputLabelReturn", this->getEventInfos());
}


void InputLabel::editBoxEditingDidEnd(EditBox* editBox)
{
    CCLOG("Edit Box editing did end");
    if(isOpened)
    { //on dealloc, the isOpened flag will be false to prevent this code from being executed (could be used for a cancel method too)
        isOpened = false;
        if(linkTo != NULL)
        {
            if(strlen(delegate->getText()) > 0)
            {
                if(isPassword)
                {
                    IFEXIST(passwordText)->autorelease();
                    passwordText = new CCString(delegate->getText());
                    std::string bulletString;
                    for(int i = 0; i < (int)strlen(delegate->getText()); i++)
                    {
                        bulletString.append("●");
                    }
                    linkTo->setLabelValue(bulletString.c_str());
                }
                else
                {
                    linkTo->setLabelValue(delegate->getText());
                }
            }
            else
            {
                linkTo->setLabelValue(initialText->getCString());
            }
            delegate->setText("");
            linkTo->getNode()->setVisible(true);
        }
        CCDictionary* param = this->getEventInfos();
        param->setObject(Screate(this->getLabelValue()), "Text");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("TextAdded", param);
    }
    delegate->detachWithIME();
}

void InputLabel::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
    const char* label = this->getLabelValue();
    CCLOG("EditBoxTextChanged called, value : %s", text.c_str());
    for (int i = 0; i < strlen(label); i++)
    {
        char ch = label[i];
        if(!(ch >= '0' && ch <= '9'))
        {
            textDirty = true;
        }
    }
}

bool InputLabel::isUnedited()
{
    const char* compareTo = linkTo != NULL ? linkTo->getLabelValue() : delegate->getText();
    return (initialText == NULL && strlen(compareTo) == 0) || (initialText != NULL && strcmp(compareTo, initialText->getCString()) == 0);
}

void InputLabel::setInitialText(CCString* text)
{
    if(linkTo != NULL)
    {
        linkTo->setLabelValue(text->getCString());
    }
    else
    {
        delegate->setPlaceHolder(text->getCString());
    }
    if(initialText != NULL)
    {
        initialText->release();
    }
    initialText = new CCString(*text);
}

void InputLabel::setIsPassword()
{
    isPassword = true;
    delegate->setInputFlag(EditBox::InputFlag::PASSWORD);
}
NS_FENNEX_END
