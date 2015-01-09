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

#include "TapRecognizer.h"
#include "GraphicLayer.h"
#include "Shorteners.h"
#include "AppMacros.h"

#define TIME GraphicLayer::sharedLayer()->getClock()

NS_FENNEX_BEGIN
// singleton stuff
static TapRecognizer *s_SharedRecognizer = NULL;

TapRecognizer* TapRecognizer::sharedRecognizer(void)
{
    if (!s_SharedRecognizer)
    {
        s_SharedRecognizer = new TapRecognizer();
        s_SharedRecognizer->init();
    }
    
    return s_SharedRecognizer;
}

void TapRecognizer::init()
{
    touchStart = CCDictionary::create();
    touchStart->retain();
    touchInitialPosition.clear();
}


bool TapRecognizer::onTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    touchStart->setObject(Fcreate(TIME), touch->getID());
    touchInitialPosition.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    return true;
}

void TapRecognizer::onTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    
}

void TapRecognizer::onTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("linked ? %s", mainLinker->linkedObjectOf(touch) == NULL ? "yes" : "no");
    CCLOG("is in start ? %s", touchStart->objectForKey(touch->getID()) != NULL ? "yes" : "no");
    CCLOG("time ? %s", (TIME - ((CCFloat*)touchStart->objectForKey(touch->getID()))->getValue()) < 2.0 ? "yes" : "no");
    CCLOG("distance ? %s : real : %f", ccpDistance(Scene::touchPosition(touch), touchInitialPosition->at(touch->getID())) < 20 ? "yes" : "no", ccpDistance(Scene::touchPosition(touch), touchInitialPosition->at(touch->getID())));
    CCLOG("initial position : %f, %f, current : %f, %f", touchInitialPosition->at(touch->getID()).x, touchInitialPosition->at(touch->getID()).y, Scene::touchPosition(touch).x, Scene::touchPosition(touch).y);
#endif
    if(mainLinker != NULL
       //&& mainLinker->linkedObjectOf(touch) == NULL //prevent drag + tap from working at the same time
       && touchStart->objectForKey(touch->getID()) != NULL
       && (TIME - ((CCFloat*)touchStart->objectForKey(touch->getID()))->getValue()) < 2.0
       && ccpDistance(Scene::touchPosition(touch), touchInitialPosition.at(touch->getID())) < 50 * RESOLUTION_MULTIPLIER)
    {
        CCDictionary* infos = CCDictionary::create();
        infos->setObject(touch, "Touch");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("TapRecognized", infos);
    }
    touchStart->removeObjectForKey(touch->getID());
    touchInitialPosition.erase(touch->getID());
}

void TapRecognizer::cleanTouches()
{
    touchStart->removeAllObjects();
    touchInitialPosition.clear();
}

void TapRecognizer::cancelRecognitionForTouch(CCTouch* touch)
{
    touchStart->removeObjectForKey(touch->getID());
    touchInitialPosition.erase(touch->getID());
}
NS_FENNEX_END
