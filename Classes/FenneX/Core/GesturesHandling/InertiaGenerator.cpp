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
#include "ScrollingRecognizer.h"

#define TIME GraphicLayer::sharedLayer()->getClock()
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
    
    ScrollingRecognizer::sharedRecognizer()->addDelegate(this);
    TapRecognizer::sharedRecognizer()->addDelegate(this);
    
    ADD_OBSERVER(InertiaGenerator::planSceneSwitch, "PlanSceneSwitch");
}

InertiaGenerator::~InertiaGenerator()
{
    ScrollingRecognizer::sharedRecognizer()->removeDelegate(this);
    for(EventListenerCustom* listener : eventListeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }
    eventListeners.clear();
}

void InertiaGenerator::addPossibleTarget(RawObject* target)
{
    if(target != NULL)
    {
        possibleTargets.pushBack(target);
    }
}

void InertiaGenerator::addPossibleTargets(Vector<RawObject*> targets)
{
    for(RawObject* target : targets)
    {
        addPossibleTarget(target);
    }
}

void InertiaGenerator::addPossibleTargets(Vector<Panel*> targets)
{
    for(Panel* target : targets)
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
        Vector<RawObject*> toRemove;
        for(int i = 0; i < inertiaTargets.size(); i++)
        {
            RawObject* target = inertiaTargets.at(i);
            Inertia* inertia = inertiaParameters.at(i);
            inertia->retain(); //retain inertia in case stopInertia is called during the notification
            inertia->setOffset(inertia->getOffset() * (1-INERTIA_FRICTION));
            
            for(ScrollingDelegate* delegate : delegates)
            {
                delegate->scrolling(inertia->getOffset(), inertia->getPosition(), {}, TIME - lastInertiaNotificationTime, target, true);
            }
            
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
void InertiaGenerator::tapRecognized(Touch* touch)
{
    lastOffsets.erase(touch->getID());
    ignoredTouches.pushBack(touch);
}

void InertiaGenerator::scrolling(Vec2 offset, Vec2 position, Vector<Touch*> touches, float deltaTime, RawObject* target, bool inertia)
{
    for(Touch* touch : touches)
    {
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

void InertiaGenerator::scrollingEnded(Vec2 offset, Vec2 position, Vector<Touch*> touches, float deltaTime, RawObject* target, bool inertia)
{
    int touches_count = (int)touches.size();
    if(touches_count == 1 && possibleTargets.size() > 0)
    {
        Vec2 inertiaOffset = Vec2(0, 0);
        Touch* touch = touches.at(0);
        if(!ignoredTouches.contains(touch))
        {
            if(lastOffsets.find(touch->getID()) != lastOffsets.end())
            {
                std::vector<Vec2>& offsets = lastOffsets.at(touch->getID());
                for(Vec2 touchOffset : offsets)
                {
                    inertiaOffset += touchOffset;
                }
                inertiaOffset *= 1.0/offsets.size();
                lastOffsets.erase(lastOffsets.find(touch->getID()));
            }
            else
            {
                inertiaOffset = offset;
            }
            if(fabs(inertiaOffset.x) > MAX_SCROLL)
            {
                inertiaOffset.x = inertiaOffset.x > 0 ? MAX_SCROLL : -MAX_SCROLL;
            }
            if(fabs(inertiaOffset.y) > MAX_SCROLL)
            {
                inertiaOffset.y = inertiaOffset.y > 0 ? MAX_SCROLL : -MAX_SCROLL;
            }
            Vector<RawObject*> intersectingObjects = GraphicLayer::sharedLayer()->all(position);
            bool originalTarget = true;
            if(target == NULL)
            {
                originalTarget = false;
                for(RawObject* candidate : intersectingObjects)
                {
                    if(target == NULL && possibleTargets.contains(candidate))
                    {
                        target = candidate;
                    }
                }
            }
            if(target != NULL
               && fabs(inertiaOffset.x) > MIN_SCROLL
               && fabs(inertiaOffset.y) > MIN_SCROLL
               && !(originalTarget && !intersectingObjects.contains(target)))
            {
#if VERBOSE_TOUCH_RECOGNIZERS
                log("inertiaOffset : %f, %f", inertiaOffset.x, inertiaOffset.y);
#endif
                inertiaTargets.pushBack(target);
                inertiaParameters.pushBack(Inertia::create(inertiaOffset, position, !target->getEventInfos()["isVertical"].isNull()
                                                                                    && target->getEventInfos()["isVertical"].asBool()));
            }
            else
            {
                for(ScrollingDelegate* delegate : delegates)
                {
                    delegate->scrollingEnded(Vec2(0,0), Vec2(0,0), {}, TIME - lastInertiaNotificationTime);
                }
            }
        }
        else
        {
            ignoredTouches.eraseObject(touch);
        }
    }
}

void InertiaGenerator::stopInertia(RawObject* obj)
{
    if(obj != NULL && inertiaTargets.contains(obj))
    {
        long index = inertiaTargets.getIndex(obj);
        inertiaParameters.erase(index);
        inertiaTargets.erase(index);
        for(ScrollingDelegate* delegate : delegates)
        {
            delegate->scrollingEnded(Vec2(0,0), Vec2(0,0), {}, TIME - lastInertiaNotificationTime, obj, true);
        }
    }
}

void InertiaGenerator::addDelegate(ScrollingDelegate* delegate)
{
    if(std::find(delegates.begin(), delegates.end(), delegate) == delegates.end()) delegates.push_back(delegate);
}

void InertiaGenerator::removeDelegate(ScrollingDelegate* delegate)
{
    delegates.erase(std::remove(delegates.begin(), delegates.end(), delegate), delegates.end());
}

NS_FENNEX_END
