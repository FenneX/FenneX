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

#include "DelayedDispatcher.h"
#include "SceneSwitcher.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN

static DelayedDispatcher* temporaryInstance = NULL;
static SceneName temporaryInstanceScene = None;
static EventListenerCustom* temporaryListener = NULL;


DelayedDispatcher::~DelayedDispatcher()
{
    temporaryInstance = NULL;
    temporaryInstanceScene = None;
}

void DelayedDispatcher::eventAfterDelay(std::string eventName, Ref* userData, float delay)
{
    DelayedDispatcher* instance = getInstance();
    IFEXIST(userData)->retain();
    instance->events.push_back(EventTuple(delay, eventName, userData));
}

void DelayedDispatcher::funcAfterDelay(std::function<void(cocos2d::EventCustom*)> func, Ref* userData, float delay, std::string eventName)
{
    DelayedDispatcher* instance = getInstance();
    IFEXIST(userData)->retain();
    instance->funcs.push_back(FuncTuple(delay, func, userData, eventName));
}

bool DelayedDispatcher::cancelEvents(std::string eventName)
{
    DelayedDispatcher* instance = getInstance();
    long before = instance->events.size();
    if(before == 0) return false;
    instance->events.erase(std::remove_if(instance->events.begin(), instance->events.end(), [&](const EventTuple& tuple) { return std::get<1>(tuple) == eventName; }), instance->events.end());
    return before != instance->events.size();
}

bool DelayedDispatcher::cancelFuncs(std::string eventName)
{
    DelayedDispatcher* instance = getInstance();
    long before = instance->funcs.size();
    if(before == 0) return false;
    instance->funcs.erase(std::remove_if(instance->funcs.begin(), instance->funcs.end(), [&](const FuncTuple& tuple) { return std::get<3>(tuple) == eventName; }), instance->funcs.end());
    return before != instance->funcs.size();
}

void DelayedDispatcher::update(float deltaTime)
{
    for(EventTuple& tuple : events)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(std::get<1>(tuple), std::get<2>(tuple));
            IFEXIST(std::get<2>(tuple))->release();
        }
    }
    if(events.size() > 0)
    {
        events.erase(std::remove_if(events.begin(), events.end(), [](const EventTuple& tuple) { return std::get<0>(tuple) < 0; }), events.end());
    }
    
    for(FuncTuple& tuple : funcs)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            cocos2d::EventCustom* event = EventCustom::create(std::get<3>(tuple), std::get<2>(tuple));
#if VERBOSE_GENERAL_INFO
            CCLOG("Launching event %s", std::get<3>(tuple).c_str());
#endif
            std::get<1>(tuple)(event);
            IFEXIST(std::get<2>(tuple))->release();
        }
    }
    if(funcs.size() > 0)
    {
        funcs.erase(std::remove_if(funcs.begin(), funcs.end(), [](const FuncTuple& tuple) { return std::get<0>(tuple) < 0; }), funcs.end());
    }
}


DelayedDispatcher* DelayedDispatcher::getInstance()
{
    //A DelayedDispatcher must be linked to a scene to keep old behavior (delayed funcs/events don't last more than the scene they were created on)
    if(SceneSwitcher::sharedSwitcher()->getCurrentScene() == NULL)
    {
        if(temporaryInstance != NULL && temporaryInstanceScene == None) return temporaryInstance;
        //Hack around the fact that DelayedDispatcher can be called during Scene init: create a temporaryInstance that will be added to the Scene when the switch ends
        temporaryInstance = new DelayedDispatcher();
        temporaryInstanceScene = None;
        temporaryListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("SceneSwitched", [](EventCustom*)
                                                                              {
                                                                                  SceneSwitcher::sharedSwitcher()->getCurrentScene()->addUpdatable(temporaryInstance);
                                                                                  temporaryInstance->release();
                                                                                  Director::getInstance()->getEventDispatcher()->removeEventListener(temporaryListener);
                                                                                  temporaryListener = NULL;
                                                                                  temporaryInstance = NULL;
                                                                              });
        return temporaryInstance;
    }
    const std::vector<Pausable*>& candidates = SceneSwitcher::sharedSwitcher()->getCurrentScene()->getUpdateList();
    for(Pausable* candidate : candidates)
    {
        if(isKindOfClass(candidate, DelayedDispatcher))
        {
            temporaryInstance = NULL;
            return (DelayedDispatcher*)candidate;
        }
    }
    //Hack around the fact the addUpdatable is not instant, it is necessary to avoid recreating several DelayedDispatcher until it's accessible using getUpdateList
    if(temporaryInstance != NULL && temporaryInstanceScene == SceneSwitcher::sharedSwitcher()->getCurrentSceneName()) return temporaryInstance;
    DelayedDispatcher* newInstance = new DelayedDispatcher();
    SceneSwitcher::sharedSwitcher()->getCurrentScene()->addUpdatable(newInstance);
    newInstance->release();
    temporaryInstance = newInstance;
    temporaryInstanceScene = SceneSwitcher::sharedSwitcher()->getCurrentSceneName();
    return newInstance;
}

NS_FENNEX_END