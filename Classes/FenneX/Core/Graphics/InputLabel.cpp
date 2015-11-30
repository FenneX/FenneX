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

static std::vector<int> locks;

void InputLabel::setEventName(const char* var)
{
    CCLOG("Warning : changing eventName for InputLabel, may not be able to open keyboard");
    if(var == NULL) eventName.clear();
    else if (eventName.compare(var))
        eventName = var;
}

Rect InputLabel::getBoundingBox()
{
    return Rect(this->getNode()->getPositionX(), this->getNode()->getPositionY(), this->getNode()->getContentSize().width, this->getNode()->getContentSize().height);
}

Node* InputLabel::getNode()
{
    CCAssert(delegate != NULL, "InputLabel getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}

void InputLabel::setPlaceHolderColor(Color3B color)
{
    delegate->setPlaceholderFontColor(color);
}

void InputLabel::setLabelValue(const char* value)
{
    delegate->setText(value);
}

const char* InputLabel::getLabelValue()
{
    return delegate->getText();
}

InputLabel::InputLabel() : delegate(NULL)
{
    isOpened = false;
    originalInfos = NULL;
    fontSize = -1;
    fontName = "";
}

InputLabel::InputLabel(ui::Scale9Sprite* sprite)
{
    isOpened = false;
    textDirty = false;
    originalInfos = NULL;
    fontSize = -1;
    fontName = "";
    name = "CustomInputLabel";
    sprite->retain();
    sprite->removeFromParentAndCleanup(true);
    Vec2 position = sprite->getPosition();
    //CCLOG("prefered size : %f, %f, content size : %f, %f, insets : %f, %f, %f, %f, position : %f, %f", sprite->getPreferredSize().width, sprite->getPreferredSize().height, sprite->getContentSize().width, sprite->getContentSize().height, sprite->getInsetBottom(), sprite->getInsetTop(), sprite->getInsetLeft(), sprite->getInsetRight(), position.x, position.y);
    CCLOG("sprite position before : %f, %f", position.x, position.y);
    //The content size is fucked up (way higher than it's supposed to be, probably a problem with cocosBuilder), so always use the prefered size, which is the real sprite size
    sprite->setContentSize(sprite->getPreferredSize());
    
    delegate = ui::EditBox::create(Size(sprite->getContentSize().width,
                                            sprite->getContentSize().height),
                                 sprite);
    delegate->retain();
    delegate->setFontColor(sprite->getColor());
    delegate->setColor(Color3B::WHITE);
    delegate->setDelegate(this);
    delegate->setOpacity(0);
    delegate->setInputFlag(ui::EditBox::InputFlag::INITIAL_CAPS_SENTENCE);

    //You HAVE to set the contentSize again, because CCControlButton do some weird thing on the UIEditbox content size which makes it work only on 1024x768
    //delegate->setContentSize(sprite->getPreferredSize());
    this->setPosition(position);
    CCLOG("delegate position after : %f, %f", delegate->getPosition().x, delegate->getPosition().y);
    delegate->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    delegate->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("OpenKeyboard", std::bind(&InputLabel::openKeyboard, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("CloseKeyboard", std::bind(&InputLabel::closeKeyboard, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("DisableInputs", std::bind(&InputLabel::disableInputs, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("EnableInputs", std::bind(&InputLabel::enableInputs, this, std::placeholders::_1)));
    
    CustomInput* input = dynamic_cast<CustomInput*>(sprite);
    if(input != NULL)
    {
        if(input->getPlaceHolder() != NULL)
        {
            delegate->setPlaceHolder(input->getPlaceHolder()->getCString());
            this->setInitialText(input->getPlaceHolder()->getCString());
        }
        numbersOnly = input->getNumbersOnly();
        if(input->getMaxChar() != -1)
        {
            delegate->setMaxLength(input->getMaxChar());
        }
        if(input->getFontSize() > 0 && input->getFontName() != NULL)
        {
            this->setFontName(input->getFontName()->getCString());
            this->setFontSize(input->getFontSize());
        }
        else if(input->getFontSize() > 0 || input->getFontName() != NULL)
        {
            CCLOG("WARNING, you cannot use font size or font name alone, you need both of them on an InputLabel");
        }
        originalInfos = input;
    }
}

InputLabel::InputLabel(const char* placeHolder, const char* fontName, int fontSize, Vec2 location, ui::EditBox::InputMode inputMode, int maxChar, Size dimensions, TextHAlignment format)
{
    isOpened = false;
    textDirty = false;
    originalInfos = NULL;
    fontSize = -1;
    fontName = "";
    name = placeHolder;
    this->setFontSize(fontSize);
    ui::Scale9Sprite* sprite = ui::Scale9Sprite::create("green_edit.png", Rect(0, 0, 43, 38), Rect(4, 3, 35, 32));
    sprite->setPreferredSize(Size(43, 38));
    sprite->setOpacity(0);
    delegate = ui::EditBox::create(dimensions, sprite);
    delegate->retain();
    if(strlen(placeHolder) > 0)
    {
        std::string placeholderWithBrackets = "<" + std::string(placeHolder) + ">";
        delegate->setPlaceHolder(placeholderWithBrackets.c_str());
        this->setInitialText(placeholderWithBrackets);
    }
    delegate->setFontColor(Color3B::BLACK);
    delegate->setDelegate(this);
    
    this->setPosition(location);
    delegate->setInputMode(inputMode);
    numbersOnly = inputMode ==  ui::EditBox::InputMode::DECIMAL;
    delegate->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    delegate->setInputFlag(ui::EditBox::InputFlag::INITIAL_CAPS_SENTENCE);
    if(maxChar != -1)
    {
        delegate->setMaxLength(maxChar);
    }
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("OpenKeyboard", std::bind(&InputLabel::openKeyboard, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("CloseKeyboard", std::bind(&InputLabel::closeKeyboard, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("DisableInputs", std::bind(&InputLabel::disableInputs, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("EnableInputs", std::bind(&InputLabel::enableInputs, this, std::placeholders::_1)));
    
}

InputLabel::~InputLabel()
{
    if(isOpened)
    {
        isOpened = false; //set the flag before so the TextFieldDetach isn't processed (it's a cancel)
        delegate->detachWithIME();
    }
    for(EventListenerCustom* listener : listeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }
    listeners.clear();
    delegate->setDelegate(NULL);
    delegate->release();
    if(originalInfos != NULL)
    {
        originalInfos->release();
    }
#if VERBOSE_DEALLOC
    CCLOG("Dealloc InputLabel %s", name.c_str());
#endif
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

void InputLabel::openKeyboard(EventCustom* event)
{
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    if(locks.size() == 0
       && ((isKindOfClass(infos->objectForKey("Sender"), CCInteger)
            && TOINT(infos->objectForKey("Sender")) == identifier)
           || (isKindOfClass(infos->objectForKey("Target"), CCInteger)
               && TOINT(infos->objectForKey("Target")) == identifier)))
    {
        CCLOG("Open InputLabel keyboard");
        delegate->touchDownAction(this, cocos2d::ui::Widget::TouchEventType::ENDED);//open keyboard by simulating a touch inside
        isOpened = true;
    }
}

void InputLabel::closeKeyboard(EventCustom* event)
{
    CCDictionary* infos = (CCDictionary*)event->getUserData();
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

void InputLabel::disableInputs(EventCustom* event)
{
    delegate->setEnabled(false);
}

void InputLabel::enableInputs(EventCustom* event)
{
    delegate->setEnabled(true);
}

void InputLabel::editBoxEditingWillBegin(ui::EditBox* editBox)
{
    /*
    if(locks.size() == 0 && !isOpened && delegate->isEnabled())
    {
        CCLOG("editing will begin InputLabel");
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
            linkTo->setVisible(false);
        }
    }*/
}


void InputLabel::editBoxEditingDidBegin(ui::EditBox* editBox)
{
    if(locks.size() > 0)
    {
        closeKeyboard(EventCustom::create("CloseKeyboard", DcreateP(Icreate(this->getID()), Screate("Sender"), NULL)));
        return;
    }
    if(!isOpened && delegate->isEnabled())
    {
        isOpened = true;
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InputLabelBeginEdit", DcreateP(Icreate(identifier), Screate("Sender"), NULL));
    }
}


void InputLabel::editBoxReturn(ui::EditBox* editBox)
{
    CCLOG("ui::EditBoxReturn : Close InputLabel keyboard");
    DelayedDispatcher::eventAfterDelay("InputLabelReturn", this->getEventInfos(), 0.01);
}


void InputLabel::editBoxEditingDidEnd(ui::EditBox* editBox)
{
    CCLOG("Edit Box editing did end");
    if(isOpened)
    { //on dealloc, the isOpened flag will be false to prevent this code from being executed (could be used for a cancel method too)
        isOpened = false;
        CCDictionary* param = this->getEventInfos();
        param->setObject(Screate(this->getLabelValue()), "Text");
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("TextAdded", param);
    }
    delegate->detachWithIME();
}

void InputLabel::editBoxTextChanged(ui::EditBox* editBox, const std::string& text)
{
    const char* label = this->getLabelValue();
    CCLOG("ui::EditBoxTextChanged called, value : %s", text.c_str());
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
    return strlen(delegate->getText()) == 0;
}

void InputLabel::setInitialText(const std::string& text)
{
    delegate->setPlaceHolder(text.c_str());
}

void InputLabel::setIsPassword()
{
    delegate->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
    //Re-apply fontSize and fontName because some devices will happily reset it (Nexus 7 on Android 5.1 for example).
    setFontSize(fontSize);
    setFontName(fontName);
}

int InputLabel::preventKeyboardOpen()
{
    int key = 0;
    while(std::find(locks.begin(), locks.end(), key) != locks.end())
    {
        key++;
    }
    locks.push_back(key);
    return key;
}

void InputLabel::releaseKeyboardLock(int key)
{
    locks.erase(std::remove(locks.begin(), locks.end(), key), locks.end());
}

void InputLabel::releaseAllKeyboardLocks()
{
    locks.clear();
}

void InputLabel::setFontSize(int size)
{
    fontSize = size;
    delegate->setFontSize(size);
    delegate->setPlaceholderFontSize(size);
}

void InputLabel::setFontName(std::string name)
{
    fontName = name;
    delegate->setFontName(name.c_str());
    delegate->setPlaceholderFontName(name.c_str());
}

int InputLabel::getFontSize()
{
    return fontSize;
}

std::string InputLabel::getFontName()
{
    return fontName;
}

NS_FENNEX_END
