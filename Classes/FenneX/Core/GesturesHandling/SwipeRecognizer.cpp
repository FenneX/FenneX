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
    touchStart.clear();
    touchInitialPosition.clear();
    minMovement = 40;
    minSpeed = 50;
}

bool SwipeRecognizer::onTouchBegan(Touch *touch, Event *pEvent)
{
    touchStart.insert(std::make_pair(touch->getID(), TIME));
    touchInitialPosition.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    return true;
}

void SwipeRecognizer::onTouchMoved(Touch *touch, Event *pEvent)
{
    
}

void SwipeRecognizer::onTouchEnded(Touch *touch, Event *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("linked ? %s", mainLinker->linkedObjectOf(touch) == NULL ? "yes" : "no");
    CCLOG("is in start ? %s", touchStart.find(touch->getID()) != touchStart.end() ? "yes" : "no");
    CCLOG("time ? %s", (TIME - touchStart.at(touch->getID())) < 2.0 ? "yes" : "no");
    CCLOG("initial position : %f, %f, current : %f, %f", touchInitialPosition.at(touch->getID()).x, touchInitialPosition.at(touch->getID()).y, Scene::touchPosition(touch).x, Scene::touchPosition(touch).y);
#endif
    if(mainLinker != NULL
       && mainLinker->linkedObjectOf(touch) == NULL
       && touchStart.find(touch->getID()) != touchStart.end())
    {
        Vec2 initialPosition = touchInitialPosition.at(touch->getID());
        Vec2 currentPosition = Scene::touchPosition(touch);
        if(fabsf(initialPosition.x - currentPosition.x) >= minMovement * RESOLUTION_MULTIPLIER &&
           fabsf(initialPosition.y - currentPosition.y) <= fabsf(initialPosition.x - currentPosition.x) &&
           fabsf(initialPosition.x - currentPosition.x) > fabsf(initialPosition.y - currentPosition.y) &&
           fabsf(initialPosition.x - currentPosition.x) / (TIME - touchStart.at(touch->getID())) > minSpeed * RESOLUTION_MULTIPLIER)
        {
            CCDictionary* infos = CCDictionary::create();
            infos->setObject(touch, "Touch");
            infos->setObject(initialPosition.x > currentPosition.x ? Screate("Left") : Screate("Right"), "Direction");
            infos->setObject(Fcreate(initialPosition.x), "InitialPositionX");
            infos->setObject(Fcreate(initialPosition.y), "InitialPositionY");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("SwipeRecognized", infos);
        }
        
    }
    touchStart.erase(touch->getID());
    touchInitialPosition.erase(touch->getID());
}

void SwipeRecognizer::cleanTouches()
{
    touchStart.clear();
    touchInitialPosition.clear();
}

void SwipeRecognizer::cancelRecognitionForTouch(Touch* touch)
{
    touchStart.erase(touch->getID());
    touchInitialPosition.erase(touch->getID());
}
NS_FENNEX_END
