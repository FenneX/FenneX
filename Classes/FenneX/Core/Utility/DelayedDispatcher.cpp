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

static DelayedDispatcher* temporaryInstance = nullptr;
static Scene* temporaryInstanceScene = nullptr;
static EventListenerCustom* temporaryListener = nullptr;

static std::vector<FuncParamTuple> backgroundFuncsWithParam;
static std::vector<FuncNoParamTuple> backgroundFuncsWithoutParam;

void DelayedDispatcher::ensureInit()
{
    getInstance();
}

DelayedDispatcher::~DelayedDispatcher()
{
    temporaryInstance = nullptr;
    temporaryInstanceScene = nullptr;
}

void DelayedDispatcher::eventAfterDelay(std::string eventName, Value userData, float delay)
{
    DelayedDispatcher* instance = getInstance();
    instance->events.push_back(EventTuple(delay, eventName, userData));
}

void DelayedDispatcher::funcAfterDelay(std::function<void(EventCustom*)> func, Value userData, float delay, std::string eventName)
{
    DelayedDispatcher* instance = getInstance();
    instance->funcsWithParam.push_back(FuncParamTuple(delay, func, userData, eventName));
}

void DelayedDispatcher::funcAfterDelay(std::function<void(void)> func, float delay, std::string eventName)
{
    DelayedDispatcher* instance = getInstance();
    instance->funcsWithoutParam.push_back(FuncNoParamTuple(delay, func, eventName));
}

void DelayedDispatcher::funcAfterDelayBackground(std::function<void(EventCustom*)> func, Value userData, float delay, std::string eventName)
{
    backgroundFuncsWithParam.push_back(FuncParamTuple(delay, func, userData, eventName));
}

void DelayedDispatcher::funcAfterDelayBackground(std::function<void()> func, float delay, std::string eventName)
{
    backgroundFuncsWithoutParam.push_back(FuncNoParamTuple(delay, func, eventName));
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
    long before = instance->funcsWithParam.size() + instance->funcsWithoutParam.size() + backgroundFuncsWithParam.size() + backgroundFuncsWithoutParam.size();
    if(before == 0) return false;
    instance->funcsWithParam.erase(std::remove_if(instance->funcsWithParam.begin(), instance->funcsWithParam.end(), [&](const FuncParamTuple& tuple) { return std::get<3>(tuple) == eventName; }), instance->funcsWithParam.end());
    instance->funcsWithoutParam.erase(std::remove_if(instance->funcsWithoutParam.begin(), instance->funcsWithoutParam.end(), [&](const FuncNoParamTuple& tuple) { return std::get<2>(tuple) == eventName; }), instance->funcsWithoutParam.end());
    backgroundFuncsWithParam.erase(std::remove_if(backgroundFuncsWithParam.begin(), backgroundFuncsWithParam.end(), [&](const FuncParamTuple& tuple) { return std::get<3>(tuple) == eventName; }), backgroundFuncsWithParam.end());
    backgroundFuncsWithoutParam.erase(std::remove_if(backgroundFuncsWithoutParam.begin(), backgroundFuncsWithoutParam.end(), [&](const FuncNoParamTuple& tuple) { return std::get<2>(tuple) == eventName; }), backgroundFuncsWithoutParam.end());
    return before != instance->funcsWithParam.size() + instance->funcsWithoutParam.size() + backgroundFuncsWithParam.size() + backgroundFuncsWithoutParam.size();
}

void DelayedDispatcher::update(float deltaTime)
{
    //Use a separate vector for calling, as called events can modify this vector
    
    //First, call events
    std::vector<EventTuple> eventsToCall;
    for(EventTuple& tuple : events)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            eventsToCall.push_back(tuple);
        }
    }
    for(EventTuple& tuple : eventsToCall)
    {
#if VERBOSE_GENERAL_INFO
        log("Launching event %s", std::get<1>(tuple).c_str());
#endif
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(std::get<1>(tuple), &std::get<2>(tuple));
    }
    if(events.size() > 0)
    {
        events.erase(std::remove_if(events.begin(), events.end(), [](const EventTuple& tuple) { return std::get<0>(tuple) < 0; }), events.end());
    }
    
    //Then call funcs with param
    std::vector<FuncParamTuple> funcsToCall;
    for(FuncParamTuple& tuple : funcsWithParam)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            funcsToCall.push_back(tuple);
        }
    }
    for(FuncParamTuple& tuple : backgroundFuncsWithParam)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            funcsToCall.push_back(tuple);
        }
    }
    for(FuncParamTuple& tuple : funcsToCall)
    {
        EventCustom* event = EventCustom::create(std::get<3>(tuple), &std::get<2>(tuple));
#if VERBOSE_GENERAL_INFO
            log("Launching func named %s", std::get<3>(tuple).c_str());
#endif
        std::get<1>(tuple)(event);
    }
    if(funcsWithParam.size() > 0)
    {
        funcsWithParam.erase(std::remove_if(funcsWithParam.begin(), funcsWithParam.end(), [](const FuncParamTuple& tuple) { return std::get<0>(tuple) < 0; }), funcsWithParam.end());
    }
    if(backgroundFuncsWithParam.size() > 0)
    {
        backgroundFuncsWithParam.erase(std::remove_if(backgroundFuncsWithParam.begin(), backgroundFuncsWithParam.end(), [](const FuncParamTuple& tuple) { return std::get<0>(tuple) < 0; }), backgroundFuncsWithParam.end());
    }
    
    //Last, call funcs without param
    std::vector<FuncNoParamTuple> funcsToCallNoParam;
    for(FuncNoParamTuple& tuple : funcsWithoutParam)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            funcsToCallNoParam.push_back(tuple);
        }
    }
    for(FuncNoParamTuple& tuple : backgroundFuncsWithoutParam)
    {
        std::get<0>(tuple) -= deltaTime;
        if(std::get<0>(tuple) < 0)
        {
            funcsToCallNoParam.push_back(tuple);
        }
    }
    for(FuncNoParamTuple& tuple : funcsToCallNoParam)
    {
#if VERBOSE_GENERAL_INFO
        log("Launching func named %s", std::get<2>(tuple).c_str());
#endif
        std::get<1>(tuple)();
    }
    if(funcsWithoutParam.size() > 0)
    {
        funcsWithoutParam.erase(std::remove_if(funcsWithoutParam.begin(), funcsWithoutParam.end(), [](const FuncNoParamTuple& tuple) { return std::get<0>(tuple) < 0; }), funcsWithoutParam.end());
    }
    if(backgroundFuncsWithoutParam.size() > 0)
    {
        backgroundFuncsWithoutParam.erase(std::remove_if(backgroundFuncsWithoutParam.begin(), backgroundFuncsWithoutParam.end(), [](const FuncNoParamTuple& tuple) { return std::get<0>(tuple) < 0; }), backgroundFuncsWithoutParam.end());
    }
}


DelayedDispatcher* DelayedDispatcher::getInstance()
{
    //A DelayedDispatcher must be linked to a scene to keep old behavior (delayed funcs/events don't last more than the scene they were created on)
    if(SceneSwitcher::sharedSwitcher()->getCurrentScene() == nullptr)
    {
        if(temporaryInstance != nullptr && temporaryInstanceScene == nullptr) return temporaryInstance;
        //Hack around the fact that DelayedDispatcher can be called during Scene init: create a temporaryInstance that will be added to the Scene when the switch ends
        temporaryInstance = new DelayedDispatcher();
        temporaryInstanceScene = nullptr;
        temporaryListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("SceneSwitched", [](EventCustom*)
          {
              int refCount = temporaryInstance->getReferenceCount();
              SceneSwitcher::sharedSwitcher()->getCurrentScene()->addUpdatable(temporaryInstance);
              Director::getInstance()->getEventDispatcher()->removeEventListener(temporaryListener);
              if(temporaryInstance->getReferenceCount() > refCount)
                  temporaryInstance->release(); // If it was already added, it won't retain again, so the ref count won't be incremented
              
              temporaryListener = nullptr;
              temporaryInstance = nullptr;
          });
        return temporaryInstance;
    }
    const std::vector<Pausable*>& candidates = SceneSwitcher::sharedSwitcher()->getCurrentScene()->getUpdateList();
    for(Pausable* candidate : candidates)
    {
        if(isKindOfClass(candidate, DelayedDispatcher))
        {
            temporaryInstance = nullptr;
            return (DelayedDispatcher*)candidate;
        }
    }
    //Hack around the fact the addUpdatable is not instant, it is necessary to avoid recreating several DelayedDispatcher until it's accessible using getUpdateList
    if(temporaryInstance != nullptr && temporaryInstanceScene == SceneSwitcher::sharedSwitcher()->getCurrentScene()) return temporaryInstance;
    DelayedDispatcher* newInstance = new DelayedDispatcher();
    SceneSwitcher::sharedSwitcher()->getCurrentScene()->addUpdatable(newInstance);
    newInstance->release();
    if(temporaryInstance != nullptr)
    {
        SceneSwitcher::sharedSwitcher()->getCurrentScene()->removeUpdatable(temporaryInstance);
        temporaryInstance = nullptr;
    }
    temporaryInstance = newInstance;
    temporaryInstanceScene = SceneSwitcher::sharedSwitcher()->getCurrentScene();
    return newInstance;
}

NS_FENNEX_END
