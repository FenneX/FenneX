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
//Throw a TextAdded event when keyboard is closed and KeyboardOpened when it's opened for this label
//Note: since V3, the InputLabel must not have an OpenKeyboard event. This is handled by its delegate directly (as a CCControlButton). Otherwise, there will be a bug on Android where the keyboard is opened 2 times
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
