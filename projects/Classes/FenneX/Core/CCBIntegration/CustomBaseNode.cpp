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
    parameters = Dcreate();
    parameters->retain();
}

CCDictionary* CustomBaseNode::getParameters()
{
    return parameters;
}

SEL_MenuHandler CustomBaseNode::onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}

SEL_CCControlHandler CustomBaseNode::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}

bool CustomBaseNode::onAssignCCBMemberVariable(CCObject * pTarget, const char* pMemberVariableName, CCNode * pNode)
{
    return false;
}


void CustomBaseNode::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
{
    
}

bool CustomBaseNode::onAssignCCBCustomProperty(CCObject* pTarget, const char* pMemberVariableName, CCBValue* pCCBValue)
{
    CustomBaseNode* target = dynamic_cast<CustomBaseNode*>(pTarget);
    if(pCCBValue->getType() == kIntValue)
    {
        if(strcmp(pMemberVariableName, "scene") == 0)
        {
            target->setScene(pCCBValue->getIntValue());
            target->setEventName(Screate("PlanSceneSwitch"));
        }
        else if(strcmp(pMemberVariableName, "zindex") == 0)
        {
            target->setZindex(pCCBValue->getIntValue());
        }
        else
        {
            target->getParameters()->setObject(Icreate(pCCBValue->getIntValue()), pMemberVariableName);
        }
    }
    else if(pCCBValue->getType() == kFloatValue)
    {
        target->getParameters()->setObject(Fcreate(pCCBValue->getFloatValue()), pMemberVariableName);
    }
    else if(pCCBValue->getType() == kStringValue)
    {
        if(strcmp(pMemberVariableName, "event") == 0)
        {
            target->setEventName(Screate(pCCBValue->getStringValue()));
        }
        else if(strcmp(pMemberVariableName, "name") == 0)
        {
            target->setName(Screate(pCCBValue->getStringValue()));
        }
        else
        {
            target->getParameters()->setObject(Screate(pCCBValue->getStringValue()), pMemberVariableName);
        }
    }
    else if(pCCBValue->getType() == kBoolValue)
    {
        target->getParameters()->setObject(Bcreate(pCCBValue->getBoolValue()), pMemberVariableName);
    }
    return CCBMemberVariableAssigner::onAssignCCBCustomProperty(pTarget, pMemberVariableName, pCCBValue);
}

CustomBaseNode::~CustomBaseNode()
{
    parameters->release();
    if(name != NULL)
        name->release();
    if(eventName != NULL)
        eventName->release();
}

NS_FENNEX_END