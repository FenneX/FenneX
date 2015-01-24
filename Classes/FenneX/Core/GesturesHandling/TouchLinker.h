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

#ifndef __FenneX__TouchLinker__
#define __FenneX__TouchLinker__

#include "Logs.h"
#include "cocos2d.h"
#include "FenneXMacros.h"
USING_NS_CC;

NS_FENNEX_BEGIN
class TouchLinker : public Ref
{
public:    
    //call record on touchBegin if it should be used by recognizers. Any linkTouch will also record the touch if it's not already done.
    void recordTouch(Touch* touch);
    
    //call remove on touchEnded/Cancelled if it was recorded (any touch can safely be passsed)
    void removeTouch(Touch* touch);
    
    void linkTouch(Touch* touch, Ref* object);
    bool isTouchLinkedTo(Touch* touch, Ref* object);
    Ref* linkedObjectOf(Touch* touch);
    Vector<Touch*> touchesLinkedTo(Ref* object);
    void unlinkTouch(Touch* touch);
    
    //return the touches which were unlinked
    Vector<Touch*> unlinkObject(Ref* object);
    
    //return all touches, even unlinked ones
    Vector<Touch*> allTouches();
    //return all objects, without duplicate
    Vector<Ref*> allObjects();
    int count();
protected:
    Map<int, Ref*> touchLinker;
    Vector<Touch*> touches;
};
NS_FENNEX_END

#endif /* defined(__FenneX__TouchLinker__) */
