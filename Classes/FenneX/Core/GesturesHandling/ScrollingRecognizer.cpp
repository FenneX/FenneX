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

#include "ScrollingRecognizer.h"
#include "GraphicLayer.h"
#include "Scene.h"
#include "Shorteners.h"

#define TIME GraphicLayer::sharedLayer()->getClock()
#define TIME_BETWEEN_NOTIFICATIONS 0.015

NS_FENNEX_BEGIN
// singleton stuff
static ScrollingRecognizer *s_SharedRecognizer = NULL;

ScrollingRecognizer* ScrollingRecognizer::sharedRecognizer(void)
{
    if (!s_SharedRecognizer)
    {
        s_SharedRecognizer = new ScrollingRecognizer();
        s_SharedRecognizer->init();
    }
    
    return s_SharedRecognizer;
}

void ScrollingRecognizer::init()
{
    lastPositions.clear();
    lastScrollingNotificationTime = TIME;
    touchMoved = false;
}



bool ScrollingRecognizer::onTouchBegan(Touch *touch, Event *pEvent)
{
    lastPositions.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    return true;
}

void ScrollingRecognizer::onTouchMoved(Touch *touch, Event *pEvent)
{
    touchMoved = true;
}

void ScrollingRecognizer::onTouchEnded(Touch *touch, Event *pEvent)
{
    if(lastPositions.find(touch->getID()) != lastPositions.end())
    {
        Ref* object = mainLinker->linkedObjectOf(touch);
        CCArray* touches = this->mainLinker->touchesLinkedTo(object);
        Vec2 currentPosition = this->positionWithTouches(touches);
        Vec2 previousPosition = this->positionWithTouches(touches, true);
        Vec2 offset = currentPosition - previousPosition;
        CCDictionary* arguments = DcreateP(Pcreate(offset), Screate("Offset"),
                                           Icreate(touches->count()), Screate("TouchesCount"),
                                           Pcreate(currentPosition), Screate("Position"),
                                           Fcreate(TIME - lastScrollingNotificationTime), Screate("DeltaTime"),
                                           touches, Screate("Touches"),
                                           object, Screate("Target"), NULL);//Note : target have to be last because it can be NULL
        CCNotificationCenter::sharedNotificationCenter()->postNotification("ScrollingEnded", arguments);
        
        if(lastPositions.find(touch->getID()) != lastPositions.end())
        {
            lastPositions.erase(touch->getID());
        }
    }
}

void ScrollingRecognizer::cleanTouches()
{
    lastPositions.clear();
}

void ScrollingRecognizer::update(float delta)
{
    if(TIME > lastScrollingNotificationTime + TIME_BETWEEN_NOTIFICATIONS && touchMoved)
    {
        CCArray* objects = mainLinker->allObjects();
        for(int i = -1; i < (int)objects->count(); i++)
        {
            Ref* object = i == -1 ? NULL : objects->objectAtIndex(i);
            
            CCArray* touches = this->mainLinker->touchesLinkedTo(object);
            Vec2 currentPosition = this->positionWithTouches(touches);
            Vec2 previousPosition = this->positionWithTouches(touches, true);
            if(currentPosition.x != previousPosition.x || currentPosition.y != previousPosition.y)
            {
                Vec2 offset = currentPosition - previousPosition;
                CCDictionary* arguments = DcreateP(Pcreate(offset), Screate("Offset"),
                                                   Icreate(touches->count()), Screate("TouchesCount"),
                                                   Pcreate(currentPosition), Screate("Position"),
                                                   Fcreate(TIME - lastScrollingNotificationTime), Screate("DeltaTime"),
                                                   touches, Screate("Touches"),
                                                   object, Screate("Target"), NULL);//Note : target have to be last because it can be NULL
                CCNotificationCenter::sharedNotificationCenter()->postNotification("Scrolling", arguments);
            }
        }
        CCArray* touches = mainLinker->allTouches();
        Ref* obj;
        CCARRAY_FOREACH(touches, obj)
        {
            Touch* touch = (Touch*) obj;
            //Only update touches that haven't been ignored
            if(lastPositions.find(touch->getID()) != lastPositions.end())
            {
                lastPositions[touch->getID()] = Scene::touchPosition(touch);
            }
        }
        lastScrollingNotificationTime = TIME;
        touchMoved = false;
    }
}

void ScrollingRecognizer::cancelRecognitionForTouch(Touch* touch)
{
    lastPositions.erase(touch->getID());
}

Vec2 ScrollingRecognizer::positionWithTouches(CCArray* touches, bool last)
{
    //compute current position as an average of touches locations
    int x = 0;
    int y = 0;
    int count = 0;
    Ref* obj;
    CCARRAY_FOREACH(touches, obj)
    {
        Touch* touch = (Touch*) obj;
        if(lastPositions.find(touch->getID()) != lastPositions.end())
        {
            count++;
            x += last ? lastPositions.at(touch->getID()).x : Scene::touchPosition(touch).x;
            y += last ? lastPositions.at(touch->getID()).y : Scene::touchPosition(touch).y;
        }
    }
    //avoid a divide by 0
    if(count == 0)
    {
        return Vec2(0, 0);
    }
    x /= count;
    y /= count;
    return Vec2(x, y);
}

Vec2 ScrollingRecognizer::offsetFromLastPosition(Ref* target)
{
    CCArray* touches = this->mainLinker->touchesLinkedTo(target);
    Vec2 currentPosition = this->positionWithTouches(touches);
    Vec2 previousPosition = this->positionWithTouches(touches, true);
    return currentPosition - previousPosition;
}
NS_FENNEX_END
