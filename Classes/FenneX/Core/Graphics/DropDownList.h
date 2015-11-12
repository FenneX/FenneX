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

#ifndef __FenneX__DropDownList__
#define __FenneX__DropDownList__

#include "cocos2d.h"
USING_NS_CC;
#include "RawObject.h"
#include "Image.h"
#include "LabelTTF.h"
#include "../extensions/cocos-ext.h"
#include "FenneXMacros.h"
#include "DropDownListWrapper.h"

NS_FENNEX_BEGIN
/**
 * This class is a wrapper for a UIPickerView in iOS and a Spinner for Android
 * It need to be initialized with setValues and setTitle
 * It launch a "DropDownListSelectionDone" event when and element is choosen. The event contain an id refering to the label linked to this list.
 * It launch a "ShowSelectDropDownList" before the list is shown.
 **/
class DropDownList : public Image
{
    CC_SYNTHESIZE_READONLY(LabelTTF*, linkTo, LinkTo);
public:
    virtual void setLinkTo(LabelTTF* var);
    DropDownList();
    DropDownList(std::string filename, Vec2 location);
    DropDownList(std::string filename, Vec2 location, int capacity);
    DropDownList(Sprite* sprite);
    ~DropDownList();
    std::string getSelectedValue();
    virtual void setSelectedValue(EventCustom* event);
    void setLabelSelectedValue(std::string selectedValue);
    void setValues(std::vector<std::string> possibleValues);
    void setTitle(std::string title);
private:
    void init();
    void showDropDownList();
    DropDownListWrapper* dropList;
    bool isOpened;
    std::string initialText;
    std::vector<std::string> possibleValues;
    Vector<EventListenerCustom*> listeners;
};

NS_FENNEX_END

#endif /* defined(__FenneX__DropDownList__) */
