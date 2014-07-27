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
    lastPositions = new CCDictionary();
    lastScrollingNotificationTime = TIME;
    touchMoved = false;
}



bool ScrollingRecognizer::ccTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    lastPositions->setObject(Pcreate(Scene::touchPosition(touch)), touch->getID());
    return true;
}

void ScrollingRecognizer::ccTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    touchMoved = true;
}

void ScrollingRecognizer::ccTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
    if(lastPositions->objectForKey(touch->getID()) != NULL)
    {
        CCObject* object = mainLinker->linkedObjectOf(touch);
        CCArray* touches = this->mainLinker->touchesLinkedTo(object);
        CCPoint currentPosition = this->positionWithTouches(touches);
        CCPoint previousPosition = this->positionWithTouches(touches, true);
        CCPoint offset = ccpSub(currentPosition, previousPosition);
        CCDictionary* arguments = DcreateP(Pcreate(offset), Screate("Offset"),
                                           Icreate(touches->count()), Screate("TouchesCount"),
                                           Pcreate(currentPosition), Screate("Position"),
                                           Fcreate(TIME - lastScrollingNotificationTime), Screate("DeltaTime"),
                                           touches, Screate("Touches"),
                                           object, Screate("Target"), NULL);//Note : target have to be last because it can be NULL
        CCNotificationCenter::sharedNotificationCenter()->postNotification("ScrollingEnded", arguments);
        
        if(lastPositions->objectForKey(touch->getID()) != NULL)
        {
            lastPositions->removeObjectForKey(touch->getID());
        }
    }
}

void ScrollingRecognizer::update(float delta)
{
    if(TIME > lastScrollingNotificationTime + TIME_BETWEEN_NOTIFICATIONS && touchMoved)
    {
        CCArray* objects = mainLinker->allObjects();
        for(int i = -1; i < (int)objects->count(); i++)
        {
            CCObject* object = i == -1 ? NULL : objects->objectAtIndex(i);
            
            CCArray* touches = this->mainLinker->touchesLinkedTo(object);
            CCPoint currentPosition = this->positionWithTouches(touches);
            CCPoint previousPosition = this->positionWithTouches(touches, true);
            if(currentPosition.x != previousPosition.x || currentPosition.y != previousPosition.y)
            {
                CCPoint offset = ccpSub(currentPosition, previousPosition);
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
        CCObject* obj;
        CCARRAY_FOREACH(touches, obj)
        {
            CCTouch* touch = (CCTouch*) obj;
            //Only update touches that haven't been ignored
            if(lastPositions->objectForKey(touch->getID()) != NULL)
            {
                lastPositions->setObject(Pcreate(Scene::touchPosition(touch)), touch->getID());
            }
        }
        lastScrollingNotificationTime = TIME;
        touchMoved = false;
    }
}

void ScrollingRecognizer::cancelRecognitionForTouch(CCTouch* touch)
{
    lastPositions->removeObjectForKey(touch->getID());
}

CCPoint ScrollingRecognizer::positionWithTouches(CCArray* touches, bool last)
{
    //compute current position as an average of touches locations
    int x = 0;
    int y = 0;
    int count = 0;
    CCObject* obj;
    CCARRAY_FOREACH(touches, obj)
    {
        CCTouch* touch = (CCTouch*) obj;
        if(lastPositions->objectForKey(touch->getID()))
        {
            count++;
            x += last ? TOPOINT(lastPositions->objectForKey(touch->getID())).x : Scene::touchPosition(touch).x;
            y += last ? TOPOINT(lastPositions->objectForKey(touch->getID())).y : Scene::touchPosition(touch).y;
        }
    }
    //avoid a divide by 0
    if(count == 0)
    {
        return ccp(0, 0);
    }
    x /= count;
    y /= count;
    return ccp(x, y);
}

CCPoint ScrollingRecognizer::offsetFromLastPosition(CCObject* target)
{
    CCArray* touches = this->mainLinker->touchesLinkedTo(target);
    CCPoint currentPosition = this->positionWithTouches(touches);
    CCPoint previousPosition = this->positionWithTouches(touches, true);
    return ccpSub(currentPosition, previousPosition);
}
NS_FENNEX_END
