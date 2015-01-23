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

#ifndef __FenneX__CustomBaseNode__
#define __FenneX__CustomBaseNode__

#include "cocos2d.h"
#include "../extensions/cocos-ext.h"
#include "FenneXMacros.h"
#include "editor-support/cocosbuilder/CocosBuilder.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

NS_FENNEX_BEGIN
/* For convenience, the base of custom nodes is in there so all custom objects just have to derive from CustomNode to have the basic informations :
 name as CCString
 eventName as CCString
 scene as int (will auto-set eventName to PlanSceneSwitch)
 */
class CustomBaseNode : public CCBSelectorResolver, public CCBMemberVariableAssigner, public NodeLoaderListener
{
    CC_SYNTHESIZE_RETAIN(CCString*, name, Name);
    CC_SYNTHESIZE_RETAIN(CCString*, eventName, EventName);
    CC_SYNTHESIZE(int, scene, Scene);
    CC_SYNTHESIZE(int, zindex, Zindex);
    
private:
    CCDictionary* parameters;
public:
    CustomBaseNode();
    virtual CCDictionary* getParameters();
public:
    virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName);
    
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(Ref * pTarget, const char * pSelectorName);
    
    virtual bool onAssignCCBMemberVariable(Ref* pTarget, const char* pMemberVariableName, Node* pNode);
    
    virtual void onNodeLoaded(Node * pNode, NodeLoader * pNodeLoader);
    
    virtual bool onAssignCCBCustomProperty(Ref* pTarget, const char* pMemberVariableName, const cocos2d::Value& pCCBValue);
    
    ~CustomBaseNode();
};

NS_FENNEX_END

#endif /* defined(__FenneX__CustomBaseNode__) */
