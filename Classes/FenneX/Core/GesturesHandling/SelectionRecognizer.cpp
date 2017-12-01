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


bool SelectionRecognizer::onTouchBegan(Touch *touch, Event *pEvent)
{
    storedTouches.insert(std::make_pair(touch->getID(), Scene::touchPosition(touch)));
    DelayedDispatcher::funcAfterDelay(std::bind(&SelectionRecognizer::checkForSelection, this, std::placeholders::_1), Value(touch->getID()), duration);
    for(SelectionDelegate* delegate : delegates)
    {
        delegate->selectionStarted(touch);
    }
    return true;
}

void SelectionRecognizer::onTouchMoved(Touch *touch, Event *pEvent)
{
    if(isTouchInSelection(touch))
    {
        Vec2 touchOrigin = storedTouches.at(touch->getID());
        if(Scene::touchPosition(touch).getDistance(touchOrigin) > maxMovement)
        {
            cancelSelectionForTouch(touch);
        }
        else
        {
            for(SelectionDelegate* delegate : delegates)
            {
                delegate->selectionMoved(touch);
            }
        }
    }
}

void SelectionRecognizer::onTouchEnded(Touch *touch, Event *pEvent)
{
    cancelSelectionForTouch(touch);
}

void SelectionRecognizer::cleanTouches()
{
    storedTouches.clear();
}

bool SelectionRecognizer::isTouchInSelection(Touch *touch)
{
    return storedTouches.find(touch->getID()) != storedTouches.end();
}

void SelectionRecognizer::cancelSelectionForTouch(Touch* touch)
{
    if(isTouchInSelection(touch))
    {
        for(SelectionDelegate* delegate : delegates)
        {
            delegate->selectionCanceled(touch, storedTouches.at(touch->getID()));
        }
        storedTouches.erase(touch->getID());
    }
}

void SelectionRecognizer::checkForSelection(EventCustom* event)
{
    Touch* touch = mainLinker->getTouch(((Value*)event->getUserData())->asInt());
    //the touch could have been discarded in the meantime
    if(touch != NULL && isTouchInSelection(touch))
    {
        Vec2 currentLocation = Scene::touchPosition(touch);
        Vec2 touchOrigin = storedTouches.at(touch->getID());
        RawObject* target = mainLinker->linkedObjectOf(touch);
        if(Scene::touchPosition(touch).getDistance(touchOrigin) <= maxMovement)
        {
            for(SelectionDelegate* delegate : delegates)
            {
                delegate->selectionRecognized(touch, target);
            }
        }
        else
        {
            for(SelectionDelegate* delegate : delegates)
            {
                delegate->selectionCanceled(touch, storedTouches.at(touch->getID()), target);
            }
        }
        
        storedTouches.erase(touch->getID());
    }
}

NS_FENNEX_END
