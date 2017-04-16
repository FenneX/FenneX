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

#include "CustomBaseNode.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN
CustomBaseNode::CustomBaseNode()
{
}

ValueMap& CustomBaseNode::getParameters()
{
    return parameters;
}

SEL_MenuHandler CustomBaseNode::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName)
{
    return NULL;
}

cocos2d::extension::Control::Handler CustomBaseNode::onResolveCCBCCControlSelector(Ref * pTarget, const char* pSelectorName)
{
    return NULL;
}

bool CustomBaseNode::onAssignCCBMemberVariable(Ref * pTarget, const char* pMemberVariableName, Node * pNode)
{
    return false;
}


void CustomBaseNode::onNodeLoaded(Node * pNode, NodeLoader * pNodeLoader)
{
    
}

bool CustomBaseNode::onAssignCCBCustomProperty(Ref* pTarget, const char* pMemberVariableName, const cocos2d::Value& pCCBValue)
{
//#error this method is never called => seems that the super is called instead
    CustomBaseNode* target = dynamic_cast<CustomBaseNode*>(pTarget);
    if(pCCBValue.getType() == cocos2d::Value::Type::INTEGER && strcmp(pMemberVariableName, "scene") == 0)
    {
        target->setScene(pCCBValue.asInt());
        target->setEventName("PlanSceneSwitch");
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::INTEGER && strcmp(pMemberVariableName, "zindex") == 0)
    {
        target->setZindex(pCCBValue.asInt());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::STRING && strcmp(pMemberVariableName, "event") == 0)
    {
        target->setEventName(pCCBValue.asString());
    }
    else if(pCCBValue.getType() == cocos2d::Value::Type::STRING && strcmp(pMemberVariableName, "name") == 0)
    {
        target->setName(pCCBValue.asString());
    }
    else
    {
        target->getParameters()[pMemberVariableName] = pCCBValue;
    }
    return CCBMemberVariableAssigner::onAssignCCBCustomProperty(pTarget, pMemberVariableName, pCCBValue);
}

NS_FENNEX_END
