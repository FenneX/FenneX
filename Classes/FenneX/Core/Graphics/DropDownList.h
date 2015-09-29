//
//  DropDownList.h
//  FenneX
//
//  Created by Pierre Bertinet on 22/09/15.
//
//

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
