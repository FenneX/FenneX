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

void TouchLinker::recordTouch(Touch* touch)
{
    if(!touches.contains(touch))
    {
        Ref* obj;
        bool shouldAdd = true;
        for(Touch* otherTouch : touches)
        {
            if(otherTouch->getID() == touch->getID())
            {
                shouldAdd = false;
            }
        }
        if(shouldAdd)
        {
            touches.pushBack(touch);
        }
    }
}

void TouchLinker::removeTouch(Touch* touch)
{
    if(touches.contains(touch))
    {
        touches.eraseObject(touch);
    }
    if(touchLinker.at(touch->getID()) != NULL)
    {
        touchLinker.erase(touch->getID());
    }
}

void TouchLinker::linkTouch(Touch* touch, Ref* object)
{
    if(touch != NULL && object != NULL)
    {
        touchLinker.insert(touch->getID(), object);
        this->recordTouch(touch);
    }
}

bool TouchLinker::isTouchLinkedTo(Touch* touch, Ref* object)
{
    if(touchLinker.at(touch->getID()) == object)
    {
        return true;
    }
    return false;
}

Ref* TouchLinker::linkedObjectOf(Touch* touch)
{
    return touchLinker.at(touch->getID());
}

Vector<Touch*> TouchLinker::touchesLinkedTo(Ref* object)
{
    Vector<Touch*> linked;
    for(Touch* touch : touches)
    {
        if(touchLinker.at(touch->getID()) == object)
        {
            linked.pushBack(touch);
        }
    }
    return linked;
}

void TouchLinker::unlinkTouch(Touch* touch)
{
    touchLinker.erase(touch->getID());
}

Vector<Touch*> TouchLinker::unlinkObject(Ref* object)
{
    Vector<Touch*> linked;
    for(Touch* touch : touches)
    {
        if(touchLinker.at(touch->getID()) == object)
        {
            touchLinker.erase(touch->getID());
            linked.pushBack(touch);
        }
    }
    return linked;
}

Vector<Touch*> TouchLinker::allTouches()
{
    return touches;
}

Vector<Ref*> TouchLinker::allObjects()
{
    Vector<Ref*> objects;
    for(Touch* touch : touches)
    {
        Ref* linked = touchLinker.at(touch->getID());
        if(linked != NULL && !objects.contains(linked))
        {
            objects.pushBack(linked);
        }
    }
    return objects;
}

int TouchLinker::count()
{
    return touches.size();
}
NS_FENNEX_END
