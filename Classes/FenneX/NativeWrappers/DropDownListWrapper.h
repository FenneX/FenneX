//
//  DropDownListWrapper.h
//  FenneX
//
//  Created by Pierre Bertinet on 21/09/15.
//
//

#ifndef FenneX_DropDownListWrapper_h
#define FenneX_DropDownListWrapper_h

#include "Shorteners.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_FENNEX;

class DropDownListWrapper: public Ref
{
public:
    DropDownListWrapper();
    ~DropDownListWrapper();
    void setPossibleValues(std::vector<std::string> values);
    void setTitle(const std::string& title);
    void setIdentifier(int identifier);
    void show();
    

private:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Android Object
    jobject delegate;
#else
    // iOS Object
    void* delegate;
#endif
};

static inline void notifySelectionDone(int identifier, std::string value)
{
    if(identifier != -1)
    {
        DelayedDispatcher::eventAfterDelay("DropDownListSelectionDone", DcreateP(Icreate(identifier), Screate("Identifier"), Screate(value), Screate("SelectedValue"), NULL), 0.01);
    }
}

#endif
