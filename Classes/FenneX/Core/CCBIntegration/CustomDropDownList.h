//
//  CustomDropDownList.h
//  FenneX
//
//  Created by Pierre Bertinet on 22/09/15.
//
//

#ifndef __FenneX__CustomDropDownList__
#define __FenneX__CustomDropDownList__


#include "cocos2d.h"
#include "../extensions/cocos-ext.h"
#include "CustomBaseNode.h"
#include "editor-support/cocosbuilder/CocosBuilder.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;


NS_FENNEX_BEGIN
class CustomDropDownList : public Sprite, public CustomBaseNode, public ActionTweenDelegate
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(CustomDropDownList, create);
    virtual void updateTweenAction(float value, const std::string& key);
};

class CustomDropDownListLoader : public SpriteLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CustomDropDownListLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CustomDropDownList);
};
NS_FENNEX_END


#endif /* defined(__FenneX__CustomDropDownList__) */
