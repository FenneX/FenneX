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

#include "SelectionRecognizer.h"
#include "GraphicLayer.h"
#include "Shorteners.h"
#include "AppMacros.h"

#define TIME GraphicLayer::sharedLayer()->getClock()

NS_FENNEX_BEGIN
// singleton stuff
static SelectionRecognizer *s_SharedRecognizer = NULL;

SelectionRecognizer* SelectionRecognizer::sharedRecognizer(void)
{
    if (!s_SharedRecognizer)
    {
        s_SharedRecognizer = new SelectionRecognizer();
        s_SharedRecognizer->init();
    }
    
    return s_SharedRecognizer;
}

void SelectionRecognizer::init()
{
    storedTouches.clear();
    maxMovement = 60 * RESOLUTION_MULTIPLIER;
    duration = 0.4;
}


bool SelectionRecognizer::onTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    storedTouches.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    performSelectorAfterDelay(this, callfuncO_selector(SelectionRecognizer::checkForSelection), duration, touch);
    CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectionStarted", DcreateP(touch, Screate("Touch"), NULL));
    return true;
}

void SelectionRecognizer::onTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    if(isTouchInSelection(touch))
    {
        CCPoint touchOrigin = storedTouches.at(touch->getID());
        if(ccpDistance(Scene::touchPosition(touch), touchOrigin) > maxMovement)
        {
            cancelSelectionForTouch(touch);
        }
        else
        {
            CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectionMoved", DcreateP(touch, Screate("Touch"), NULL));
        }
    }
}

void SelectionRecognizer::onTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
    cancelSelectionForTouch(touch);
}

bool SelectionRecognizer::isTouchInSelection(CCTouch *touch)
{
    return storedTouches.find(touch->getID()) != storedTouches.end();
}

void SelectionRecognizer::cancelSelectionForTouch(CCTouch* touch)
{
    if(isTouchInSelection(touch))
    {
        CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectionCanceled", DcreateP(touch, Screate("Touch"), Pcreate(storedTouches.at(touch->getID())), Screate("Origin"), NULL));
        storedTouches.erase(touch->getID());
    }
}

void SelectionRecognizer::checkForSelection(CCObject* obj)
{
    CCTouch* touch = (CCTouch*)obj;
    //the touch could have been discarded in the meantime
    if(isTouchInSelection(touch))
    {
        CCPoint currentLocation = Scene::touchPosition(touch);
        CCPoint touchOrigin = storedTouches.at(touch->getID());
        CCObject* target = mainLinker->linkedObjectOf(touch);
        if(ccpDistance(Scene::touchPosition(touch), touchOrigin) <= maxMovement)
        {
            CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectionRecognized", DcreateP(touch, Screate("Touch"), target, Screate("Target"), NULL));
        }
        else
        {
            CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectionCanceled", DcreateP(touch, Screate("Touch"), Pcreate(storedTouches.at(touch->getID())), Screate("Origin"), target, Screate("Target"), NULL));
        }
        
        storedTouches.erase(touch->getID());
    }
}
NS_FENNEX_END
