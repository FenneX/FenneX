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
#include "Inertia.h"
#include "AppMacros.h"

#define TIME GraphicLayer::sharedLayer()->getClock()
#define ADD_OBSERVER(func, notifName) (center->addObserver(this, callfuncO_selector(InertiaGenerator::func), notifName, NULL))
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
    inertiaTargets = new CCArray();
    inertiaParameters = new CCArray();
    possibleTargets = new CCArray();
    lastOffsets = new CCDictionary();
    ignoredTouches = new CCArray();
    
    currentTime = 0;
    lastInertiaNotificationTime = 0;
    
    
    CCNotificationCenter* center = CCNotificationCenter::sharedNotificationCenter();
    ADD_OBSERVER(planSceneSwitch, "PlanSceneSwitch");
    ADD_OBSERVER(scrollingEnded, "ScrollingEnded");
    ADD_OBSERVER(scrolling, "Scrolling");
    ADD_OBSERVER(tapRecognized, "TapRecognized");
}

InertiaGenerator::~InertiaGenerator()
{
    inertiaTargets->release();
    inertiaParameters->release();
    possibleTargets->release();
    lastOffsets->release();
    ignoredTouches->release();
}

void InertiaGenerator::addPossibleTarget(CCObject* object)
{
    if(object != NULL)
    {
        possibleTargets->addObject(object);
    }
}

void InertiaGenerator::addPossibleTargets(CCArray* array)
{
    CCObject* obj;
    CCARRAY_FOREACH(array, obj)
    {
        this->addPossibleTarget(obj);
    }
}

void InertiaGenerator::planSceneSwitch(Ref* obj)
{
    inertiaTargets->removeAllObjects();
    inertiaParameters->removeAllObjects();
    possibleTargets->removeAllObjects();
    ignoredTouches->removeAllObjects();
}

void InertiaGenerator::update(float delta)
{
    if(TIME > lastInertiaNotificationTime + TIME_BETWEEN_NOTIFICATIONS)
    {
        CCArray* toRemove = new CCArray();
        for(int i = 0; i < inertiaTargets->count(); i++)
        {
            CCObject* target = inertiaTargets->objectAtIndex(i);
            Inertia* inertia = (Inertia*)inertiaParameters->objectAtIndex(i);
            inertia->retain(); //retain inertia in case stopInertia is called during the notification
            inertia->setOffset(ccpMult(inertia->getOffset(), 1-INERTIA_FRICTION));
            CCDictionary* arguments = DcreateP(Pcreate(inertia->getOffset()), Screate("Offset"),
                                               Icreate(0), Screate("TouchesCount"),
                                               Pcreate(inertia->getPosition()), Screate("Position"),
                                               Fcreate(TIME - lastInertiaNotificationTime), Screate("DeltaTime"),
                                               Bcreate(true), Screate("Inertia"),
                                               target, Screate("Target"), NULL);
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Inertia", arguments);
            if (fabs(inertia->getOffset().x) < MIN_SCROLL && fabs(inertia->getOffset().y) < MIN_SCROLL)
            {
                toRemove->addObject(target);
            }
            inertia->release();
        }
        while(toRemove->count() > 0)
        {
            this->stopInertia(toRemove->objectAtIndex(0));
            toRemove->removeObjectAtIndex(0);
        }
        toRemove->release();
        lastInertiaNotificationTime = TIME;
    }
}

//If a tap is recognized, no inertia is generated
void InertiaGenerator::tapRecognized(Ref* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    CCTouch* touch = (CCTouch*)infos->objectForKey("Touch");
    this->ignoreTouch(touch);
}

void InertiaGenerator::ignoreTouch(CCTouch* touch)
{
    lastOffsets->removeObjectForKey(touch->getID());
    ignoredTouches->addObject(touch);
}

void InertiaGenerator::scrolling(CCObject* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    CCArray* touches = (CCArray*)infos->objectForKey("Touches");
    for(int i = 0; i < touches->count(); i++)
    {
        CCTouch* touch = (CCTouch*)touches->objectAtIndex(i);
        if(!ignoredTouches->containsObject(touch))
        {
            CCPoint offset = Scene::touchOffset(touch);
            if(lastOffsets->objectForKey(touch->getID()) == NULL)
            {
                lastOffsets->setObject(Acreate(), touch->getID());
            }
            CCArray* offsets = (CCArray*)lastOffsets->objectForKey(touch->getID());
            offsets->addObject(Pcreate(offset));
            if(offsets->count() > MAX_OFFSETS_MEMORY)
            {
                offsets->removeObjectAtIndex(0);
            }
        }
    }
}

void InertiaGenerator::scrollingEnded(CCObject* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    int touches_count = TOINT(infos->objectForKey("TouchesCount"));
    if(touches_count == 1 && possibleTargets->count() > 0)
    {
        CCPoint inertiaOffset = ccp(0, 0);
        CCTouch* touch = (CCTouch*)((CCArray*)infos->objectForKey("Touches"))->objectAtIndex(0);
        if(!ignoredTouches->containsObject(touch))
        {
            CCArray* offsets = (CCArray*)lastOffsets->objectForKey(touch->getID());
            if(offsets != NULL)
            {
                for(int i = 0; i < offsets->count(); i++)
                {
                    inertiaOffset = ccpAdd(inertiaOffset, TOPOINT(offsets->objectAtIndex(i)));
                }
                inertiaOffset = ccpMult(inertiaOffset, 1.0/offsets->count());
                offsets = NULL;
                lastOffsets->removeObjectForKey(touch->getID());
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
            CCPoint position = TOPOINT(infos->objectForKey("Position"));
            CCArray* intersectingObjects = GraphicLayer::sharedLayer()->allObjectsAtPosition(position);
            RawObject* target = (RawObject*)infos->objectForKey("Target");
            bool originalTarget = true;
            if(target == NULL)
            {
                originalTarget = false;
                for(int i = 0; i < intersectingObjects->count() && target == NULL; i++)
                {
                    if(possibleTargets->containsObject(intersectingObjects->objectAtIndex(i)))
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
                inertiaTargets->addObject(target);
                inertiaParameters->addObject(Inertia::create(inertiaOffset, position, target->getEventInfos()->objectForKey("isVertical") != NULL && TOBOOL(target->getEventInfos()->objectForKey("isVertical"))));
            }
            else
            {
                CCNotificationCenter::sharedNotificationCenter()->postNotification("NoInertia", DcreateP(target, Screate("Target"), NULL));
            }
        }
        else
        {
            ignoredTouches->removeObject(touch);
        }
    }
}

void InertiaGenerator::stopInertia(CCObject* obj)
{
    if(obj != NULL && inertiaTargets->containsObject(obj))
    {
        int index = inertiaTargets->indexOfObject(obj);
        inertiaParameters->removeObjectAtIndex(index);
        inertiaTargets->removeObjectAtIndex(index);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("InertiaEnded", DcreateP(obj, Screate("Target"), NULL));
    }
}
NS_FENNEX_END
