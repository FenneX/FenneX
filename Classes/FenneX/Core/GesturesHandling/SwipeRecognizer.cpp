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

#include "SwipeRecognizer.h"
#include "GraphicLayer.h"
#include "Shorteners.h"
#include "AppMacros.h"

#define TIME GraphicLayer::sharedLayer()->getClock()

NS_FENNEX_BEGIN
// singleton stuff
static SwipeRecognizer *s_SharedRecognizer = NULL;

SwipeRecognizer* SwipeRecognizer::sharedRecognizer(void)
{
    if (!s_SharedRecognizer)
    {
        s_SharedRecognizer = new SwipeRecognizer();
        s_SharedRecognizer->init();
    }
    
    return s_SharedRecognizer;
}

void SwipeRecognizer::init()
{
    touchStart = new CCDictionary();
    touchInitialPosition.clear();
    minMovement = 40;
    minSpeed = 50;
}



bool SwipeRecognizer::onTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    touchStart->setObject(Fcreate(TIME), touch->getID());
    touchInitialPosition.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    return true;
}

void SwipeRecognizer::onTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    
}

void SwipeRecognizer::onTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("linked ? %s", mainLinker->linkedObjectOf(touch) == NULL ? "yes" : "no");
    CCLOG("is in start ? %s", touchStart->objectForKey(touch->getID()) != NULL ? "yes" : "no");
    CCLOG("time ? %s", (TIME - ((CCFloat*)touchStart->objectForKey(touch->getID()))->getValue()) < 2.0 ? "yes" : "no");
    CCLOG("initial position : %f, %f, current : %f, %f", touchInitialPosition->at(touch->getID()).x, touchInitialPosition->at(touch->getID()).y, Scene::touchPosition(touch).x, Scene::touchPosition(touch).y);
#endif
    if(mainLinker != NULL
       && mainLinker->linkedObjectOf(touch) == NULL
       && touchStart->objectForKey(touch->getID()) != NULL)
    {
        CCPoint initialPosition = touchInitialPosition.at(touch->getID());
        CCPoint currentPosition = Scene::touchPosition(touch);
        if(fabsf(initialPosition.x - currentPosition.x) >= minMovement * RESOLUTION_MULTIPLIER &&
           fabsf(initialPosition.y - currentPosition.y) <= fabsf(initialPosition.x - currentPosition.x) &&
           fabsf(initialPosition.x - currentPosition.x) > fabsf(initialPosition.y - currentPosition.y) &&
           fabsf(initialPosition.x - currentPosition.x) / (TIME - TOFLOAT(touchStart->objectForKey(touch->getID()))) > minSpeed * RESOLUTION_MULTIPLIER)
        {
            CCDictionary* infos = CCDictionary::create();
            infos->setObject(touch, "Touch");
            infos->setObject(initialPosition.x > currentPosition.x ? Screate("Left") : Screate("Right"), "Direction");
            infos->setObject(Fcreate(initialPosition.x), "InitialPositionX");
            infos->setObject(Fcreate(initialPosition.y), "InitialPositionY");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("SwipeRecognized", infos);
        }
        
    }
    touchStart->removeObjectForKey(touch->getID());
    touchInitialPosition.erase(touch->getID());
}

void SwipeRecognizer::cleanTouches()
{
    touchStart->removeAllObjects();
    touchInitialPosition.clear();
}

void SwipeRecognizer::cancelRecognitionForTouch(CCTouch* touch)
{
    touchStart->removeObjectForKey(touch->getID());
    touchInitialPosition.erase(touch->getID());
}
NS_FENNEX_END
