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

#include "Shorteners.h"

NS_FENNEX_BEGIN
CCString* getResourcesPath(const char* file)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return Screate(CCFileUtils::sharedFileUtils()->fullPathForFilename(file));
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    return ScreateF("assets/%s", file);
#endif
}

CCSize* sizeCreate(float width, float height)
{
    CCSize* pRet = new CCSize(width, height);
    pRet->autorelease();
    return pRet;
}

//note : keys have to be passed as CCString, unfortunately. Must be NULL terminated
CCDictionary* createDictionaryWithParameters(CCObject* firstObject, ... )
{
    CCObject* eachObject;
    va_list argumentList;
    bool key = true;
    CCObject* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, CCObject*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
                    CCLOG("Warning : not a key, value ignored");
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return values;
    }
    else
    {
        CCLOG("Warning : createDictionaryWithParameters called with no firstObject");
    }
    return NULL;
}

CCArray* createArrayWithParameters(CCObject* firstObject, ... )
{
    CCObject* eachObject;
    va_list argumentList;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCArray* values = CCArray::create();
        values->addObject(firstObject);
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, CCObject*)) != NULL) // As many times as we can get an argument of type "id"
        {
            values->addObject(eachObject);
        }
        va_end(argumentList);
        return values;
    }
    else
    {
        CCLOG("Warning : createArrayWithParameters called with no firstObject, returning empty Array");
    }
    return Acreate();
}

void performSelectorAfterDelay(CCObject* target, SEL_CallFuncO selector, float delay, CCObject* object)
{
    CCDelayTime *delayAction = CCDelayTime::create(delay);
    CCCallFunc* callSelectorAction = CCCallFuncO::create(target, selector, object);
    CCDirector::sharedDirector()->getNotificationNode()->runAction(CCSequence::create(delayAction, callSelectorAction, NULL));
}

bool cancelSelector(CCObject* target, SEL_CallFuncO selector)
{
    Vector<Action*> actions = CCDirector::sharedDirector()->getNotificationNode()->getActionManager()->getAllActionsForTarget(CCDirector::sharedDirector()->getNotificationNode());
    for(auto action : actions)
    {
        if(isKindOfClass(action, CCSequence))
        {
            CCSequence* sequence = (CCSequence*)action;
            if(isKindOfClass(sequence->getCurrentAction(), CCDelayTime) && isKindOfClass(sequence->getNextAction(), CCCallFuncO))
            {
                CCCallFuncO* callFunc = (CCCallFuncO*)sequence->getNextAction();
                if(callFunc->getSelectorTarget() == selector)
                {
                    CCDirector::sharedDirector()->getNotificationNode()->getActionManager()->removeAction(action);
                    return true;
                }
            }
        }
        else
        {
            CCLOG("Unusable action");
        }
    }
    return false;
}


void performNotificationAfterDelay(const char* name, CCObject* obj, float delay)
{
    CCDictionary* notifInfos = DcreateP(Screate(name), Screate("_NotificationName"), obj, Screate("_Infos"), NULL);
    //Retain the infos, otherwise they may be destroyed by the main thread before being passed to the function
    notifInfos->retain();
    performSelectorAfterDelay(Shorteners::sharedClass(), callfuncO_selector(Shorteners::delayedPostNotification), delay, notifInfos);
}

static Shorteners *s_SharedClass = NULL;

Shorteners* Shorteners::sharedClass(void)
{
    if (!s_SharedClass)
    {
        s_SharedClass = new Shorteners();
    }
    
    return s_SharedClass;
}

void Shorteners::delayedPostNotification(Ref* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    CCAssert(isKindOfClass(infos->objectForKey("_NotificationName"), CCString), "Missing _NotificationName in delayedPostNotification");
    CCAssert(infos->objectForKey("_Infos") != NULL, "Missing _Infos in delayedPostNotification");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(TOCSTRING(infos->objectForKey("_NotificationName")), infos->objectForKey("_Infos"));
    infos->release();
}
NS_FENNEX_END
