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

#include "InertiaGenerator.h"
#include "GraphicLayer.h"
#include "Shorteners.h"
#include "AppMacros.h"
#include "Inertia.h"

#define TIME GraphicLayer::sharedLayer()->getClock()
#define ADD_OBSERVER(func, notifName) (listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener(notifName, std::bind(&InertiaGenerator::func, this, std::placeholders::_1))))
#define TIME_BETWEEN_NOTIFICATIONS 0.015

#define MAX_SCROLL 200 * RESOLUTION_MULTIPLIER
#define INERTIA_FRICTION 0.05
#define MAX_OFFSETS_MEMORY 5
#define MIN_SCROLL 1

NS_FENNEX_BEGIN
// singleton stuff
static InertiaGenerator *s_InertiaGenerator = NULL;

InertiaGenerator* InertiaGenerator::sharedInertia(void)
{
    if (!s_InertiaGenerator)
    {
        s_InertiaGenerator = new InertiaGenerator();
        s_InertiaGenerator->init();
    }
    return s_InertiaGenerator;
}

void InertiaGenerator::init()
{    
    currentTime = 0;
    lastInertiaNotificationTime = 0;
    
    ADD_OBSERVER(planSceneSwitch, "PlanSceneSwitch");
    ADD_OBSERVER(scrollingEnded, "ScrollingEnded");
    ADD_OBSERVER(scrolling, "Scrolling");
    ADD_OBSERVER(tapRecognized, "TapRecognized");
}

InertiaGenerator::~InertiaGenerator()
{
    for(EventListenerCustom* listener : listeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }
    listeners.clear();
}

void InertiaGenerator::addPossibleTarget(Ref* target)
{
    if(target != NULL)
    {
        possibleTargets.pushBack(target);
    }
}

void InertiaGenerator::addPossibleTargets(CCArray* targets)
{
    Ref* obj;
    CCARRAY_FOREACH(targets, obj)
    {
        this->addPossibleTarget(obj);
    }
}

void InertiaGenerator::addPossibleTargets(Vector<Ref*> targets)
{
    for(Ref* target : targets)
    {
        addPossibleTarget(target);
    }
}

void InertiaGenerator::planSceneSwitch(EventCustom* event)
{
    inertiaTargets.clear();
    inertiaParameters.clear();
    possibleTargets.clear();
    ignoredTouches.clear();
}

void InertiaGenerator::update(float delta)
{
    if(TIME > lastInertiaNotificationTime + TIME_BETWEEN_NOTIFICATIONS)
    {
        Vector<Ref*> toRemove;
        for(int i = 0; i < inertiaTargets.size(); i++)
        {
            Ref* target = inertiaTargets.at(i);
            Inertia* inertia = inertiaParameters.at(i);
            inertia->retain(); //retain inertia in case stopInertia is called during the notification
            inertia->setOffset(inertia->getOffset() * (1-INERTIA_FRICTION));
            CCDictionary* arguments = DcreateP(Pcreate(inertia->getOffset()), Screate("Offset"),
                                               Icreate(0), Screate("TouchesCount"),
                                               Pcreate(inertia->getPosition()), Screate("Position"),
                                               Fcreate(TIME - lastInertiaNotificationTime), Screate("DeltaTime"),
                                               Bcreate(true), Screate("Inertia"),
                                               target, Screate("Target"), NULL);
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("Inertia", arguments);
            if (fabs(inertia->getOffset().x) < MIN_SCROLL && fabs(inertia->getOffset().y) < MIN_SCROLL)
            {
                toRemove.pushBack(target);
            }
            inertia->release();
        }
        while(toRemove.size() > 0)
        {
            this->stopInertia(toRemove.at(0));
            toRemove.erase(0);
        }
        lastInertiaNotificationTime = TIME;
    }
}

//If a tap is recognized, no inertia is generated
void InertiaGenerator::tapRecognized(EventCustom* event)
{
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    Touch* touch = (Touch*)infos->objectForKey("Touch");
    this->ignoreTouch(touch);
}

void InertiaGenerator::ignoreTouch(Touch* touch)
{
    lastOffsets.erase(touch->getID());
    ignoredTouches.pushBack(touch);
}

void InertiaGenerator::scrolling(EventCustom* event)
{
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    CCArray* touches = (CCArray*)infos->objectForKey("Touches");
    for(int i = 0; i < touches->count(); i++)
    {
        Touch* touch = (Touch*)touches->objectAtIndex(i);
        if(!ignoredTouches.contains(touch))
        {
            Vec2 offset = Scene::touchOffset(touch);
            if(lastOffsets.find(touch->getID()) == lastOffsets.end())
            {
                lastOffsets.insert(std::make_pair(touch->getID(), std::vector<Vec2>()));
            }
            std::vector<Vec2>& offsets = lastOffsets.at(touch->getID());
            offsets.push_back(offset);
            if(offsets.size() > MAX_OFFSETS_MEMORY)
            {
                offsets.erase(offsets.begin());
            }
        }
    }
}

void InertiaGenerator::scrollingEnded(EventCustom* event)
{
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    int touches_count = TOINT(infos->objectForKey("TouchesCount"));
    if(touches_count == 1 && possibleTargets.size() > 0)
    {
        Vec2 inertiaOffset = Vec2(0, 0);
        Touch* touch = (Touch*)((CCArray*)infos->objectForKey("Touches"))->objectAtIndex(0);
        if(!ignoredTouches.contains(touch))
        {
            if(lastOffsets.find(touch->getID()) != lastOffsets.end())
            {
                std::vector<Vec2>& offsets = lastOffsets.at(touch->getID());
                for(Vec2 offset : offsets)
                {
                    inertiaOffset += offset;
                }
                inertiaOffset *= 1.0/offsets.size();
                lastOffsets.erase(lastOffsets.find(touch->getID()));
            }
            else
            {
                inertiaOffset = TOPOINT(infos->objectForKey("Offset"));
            }
            if(fabs(inertiaOffset.x) > MAX_SCROLL)
            {
                inertiaOffset.x = inertiaOffset.x > 0 ? MAX_SCROLL : -MAX_SCROLL;
            }
            if(fabs(inertiaOffset.y) > MAX_SCROLL)
            {
                inertiaOffset.y = inertiaOffset.y > 0 ? MAX_SCROLL : -MAX_SCROLL;
            }
            Vec2 position = TOPOINT(infos->objectForKey("Position"));
            CCArray* intersectingObjects = GraphicLayer::sharedLayer()->allObjectsAtPosition(position);
            RawObject* target = (RawObject*)infos->objectForKey("Target");
            bool originalTarget = true;
            if(target == NULL)
            {
                originalTarget = false;
                for(int i = 0; i < intersectingObjects->count() && target == NULL; i++)
                {
                    if(possibleTargets.contains(intersectingObjects->objectAtIndex(i)))
                    {
                        target = (RawObject*)intersectingObjects->objectAtIndex(i);
                    }
                }
            }
            if(target != NULL
               && fabs(inertiaOffset.x) > MIN_SCROLL
               && fabs(inertiaOffset.y) > MIN_SCROLL
               && !(originalTarget && !intersectingObjects->containsObject(target)))
            {
#if VERBOSE_TOUCH_RECOGNIZERS
                CCLOG("inertiaOffset : %f, %f", inertiaOffset.x, inertiaOffset.y);
#endif
                inertiaTargets.pushBack(target);
                inertiaParameters.pushBack(Inertia::create(inertiaOffset, position, target->getEventInfos()->objectForKey("isVertical") != NULL && TOBOOL(target->getEventInfos()->objectForKey("isVertical"))));
            }
            else
            {
                Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("NoInertia", DcreateP(target, Screate("Target"), NULL));
            }
        }
        else
        {
            ignoredTouches.eraseObject(touch);
        }
    }
}

void InertiaGenerator::stopInertia(Ref* obj)
{
    if(obj != NULL && inertiaTargets.contains(obj))
    {
        long index = inertiaTargets.getIndex(obj);
        inertiaParameters.erase(index);
        inertiaTargets.erase(index);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InertiaEnded", DcreateP(obj, Screate("Target"), NULL));
    }
}
NS_FENNEX_END
