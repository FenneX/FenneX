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

#include "TouchLinker.h"

NS_FENNEX_BEGIN
TouchLinker::TouchLinker()
{
    touchLinker = CCDictionary::create();
    touchLinker->retain();
    touches = CCArray::create();
    touches->retain();
}

TouchLinker::~TouchLinker()
{
    touchLinker->release();
    touches->release();
}

void TouchLinker::recordTouch(CCTouch* touch)
{
    if(!touches->containsObject(touch))
    {
        CCObject* obj;
        bool shouldAdd = true;
        CCARRAY_FOREACH(touches, obj)
        {
            CCTouch* otherTouch = (CCTouch*)obj;
            if(otherTouch->getID() == touch->getID())
            {
                shouldAdd = false;
            }
        }
        if(shouldAdd)
        {
            touches->addObject(touch);
        }
    }
}

void TouchLinker::removeTouch(CCTouch* touch)
{
    if(touches->containsObject(touch))
    {
        touches->removeObject(touch);
    }
    if(touchLinker->objectForKey(touch->getID()) != NULL)
    {
        touchLinker->removeObjectForKey(touch->getID());
    }
}

void TouchLinker::linkTouch(CCTouch* touch, CCObject* object)
{
    if(touch != NULL && object != NULL)
    {
        touchLinker->setObject(object, touch->getID());
        this->recordTouch(touch);
    }
}

bool TouchLinker::isTouchLinkedTo(CCTouch* touch, CCObject* object)
{
    if(touchLinker->objectForKey(touch->getID()) == object)
    {
        return true;
    }
    return false;
}

CCObject* TouchLinker::linkedObjectOf(CCTouch* touch)
{
    return touchLinker->objectForKey(touch->getID());
}

CCArray* TouchLinker::touchesLinkedTo(CCObject* object)
{
    CCObject* obj;
    CCArray* linked = CCArray::create();
    CCARRAY_FOREACH(touches, obj)
    {
        CCTouch* touch = (CCTouch*)obj;
        if(touchLinker->objectForKey(touch->getID()) == object)
        {
            linked->addObject(touch);
        }
    }
    return linked;
}

void TouchLinker::unlinkTouch(CCTouch* touch)
{
    touchLinker->removeObjectForKey(touch->getID());
}

CCArray* TouchLinker::unlinkObject(CCObject* object)
{
    CCObject* obj;
    CCArray* linked = CCArray::create();
    CCARRAY_FOREACH(touches, obj)
    {
        CCTouch* touch = (CCTouch*)obj;
        if(touchLinker->objectForKey(touch->getID()) == object)
        {
            touchLinker->removeObjectForKey(touch->getID());
            linked->addObject(touch);
        }
    }
    return linked;
}

CCArray* TouchLinker::allTouches()
{
    return touches;
}

CCArray* TouchLinker::allObjects()
{
    CCObject* obj;
    CCArray* objects = CCArray::create();
    CCARRAY_FOREACH(touches, obj)
    {
        CCTouch* touch = (CCTouch*)obj;
        CCObject* linked = touchLinker->objectForKey(touch->getID());
        if(linked != NULL && !objects->containsObject(linked))
        {
            objects->addObject(linked);
        }
    }
    return objects;
}

int TouchLinker::count()
{
    return touches->count();
}
NS_FENNEX_END
