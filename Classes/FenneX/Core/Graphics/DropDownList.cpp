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

#include "DropDownList.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN

void DropDownList::setLinkTo(LabelTTF* var)
{
    linkTo = var;
    dropList->setIdentifier(linkTo->getID());
    if(initialText == "")
    {
        initialText = linkTo->getLabelValue();
    }
}

DropDownList::DropDownList()
    :Image()
{
    init();
}

DropDownList::DropDownList(std::string filename, Vec2 location)
    :Image(filename, location)
{
    init();
}

DropDownList::DropDownList(std::string filename, Vec2 location, int capacity)
:Image(filename, location, capacity)
{
    init();
}

DropDownList::DropDownList(Sprite* sprite)
    :Image(sprite)
{
    init();
}

 void DropDownList::init()
{
    linkTo = NULL;
    isOpened = false;
    initialText = "";
    dropList = new DropDownListWrapper();
    this->setEventActivated(true);
    this->setEventName("ShowSelectDropDownList");
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("ShowSelectDropDownList", std::bind(&DropDownList::showDropDownList, this)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("DropDownListSelectionDone", std::bind(&DropDownList::setSelectedValue, this, std::placeholders::_1)));
}

DropDownList::~DropDownList()
{
    dropList->release();
    dropList = NULL;
    for(EventListenerCustom* listener : listeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }
    listeners.clear();
}

std::string DropDownList::getSelectedValue()
{
    if(linkTo == NULL || std::find(possibleValues.begin(), possibleValues.end(),  linkTo->getLabelValue()) != possibleValues.end())
    {
        return "";
    }
    else
    {
        return linkTo->getLabelValue();
    }
}

void DropDownList::setSelectedValue(EventCustom* event)
{
    CCDictionary* infos =  event != NULL ? (CCDictionary*) event->getUserData() : NULL;
    std::string value = initialText;
    if(infos != NULL && linkTo != NULL && linkTo->getID() == TOINT(infos->objectForKey("Identifier")))
    {
        value = TOCSTRING(infos->objectForKey("SelectedValue"));
    }
    setLabelSelectedValue(value);
}

void DropDownList::setLabelSelectedValue(std::string selectedValue)
{
    if(linkTo != NULL)
    {
        linkTo->setLabelValue(selectedValue.c_str());
    }
}

void DropDownList::setValues(std::vector<std::string> possibleValues)
{
    dropList->setPossibleValues(possibleValues);
}

void DropDownList::setTitle(std::string title)
{
    dropList->setTitle(title);
}

void DropDownList::showDropDownList()
{
    dropList->show();
}

NS_FENNEX_END
