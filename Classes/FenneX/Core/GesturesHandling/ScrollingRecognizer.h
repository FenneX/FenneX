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

#ifndef __FenneX__ScrollingRecognizer__
#define __FenneX__ScrollingRecognizer__

#include "cocos2d.h"
USING_NS_CC;
#include "DelegatingRecognizer.h"
#include "Pausable.h"
#include "FenneXMacros.h"

//WARNING : do not copy this ScrollingRecognizer version, because it ignore if the touch is already linked to another object

NS_FENNEX_BEGIN

class ScrollingDelegate
{
public:
    virtual void scrolling(Vec2 offset, Vec2 position, Vector<Touch*> touches, float deltaTime, RawObject* target = NULL, bool inertia = false) = 0;
    virtual void scrollingEnded(Vec2 offset, Vec2 position, Vector<Touch*> touches, float deltaTime, RawObject* target = NULL, bool inertia = false) = 0;
};
//throw events Scrolling with argument Offset, Position (as Vec2), DeltaTime (as CCFloat) and TouchesCount (as CCInteger
// and ScrollingEnded
//if there is a Target, only the associated target should respond
//must be updated, as Scrolling events are generated on update (for performance issues, the events are throttled)
//ScrollingRecognizer responds better in a real multi-touch context if every touch is linked to its target receiver, using mainLinker from super
//it will not change any link on this linker
//You need to subscribe to the recognizer by adding a delegate
class ScrollingRecognizer : public DelegatingRecognizer<ScrollingDelegate>, public Pausable
{
public:
    static ScrollingRecognizer* sharedRecognizer(void);
    void init();
    
    virtual bool onTouchBegan(Touch *touch, Event *pEvent);
    virtual void onTouchMoved(Touch *touch, Event *pEvent);
    virtual void onTouchEnded(Touch *touch, Event *pEvent);
    virtual void cleanTouches();
    virtual void update(float delta);
    void cancelRecognitionForTouch(Touch* touch);
    void adjustTargetPosition(Ref* obj);
private:
    std::map<int, Vec2> lastPositions;//key : touch ID, value : last position
    float lastScrollingNotificationTime;
    bool touchMoved;
    
    //last indicate to use the lastPosition instead of Scene::touchPosition
    Vec2 positionWithTouches(Vector<Touch*> touches, bool last = false);
    Vec2 offsetFromLastPosition(RawObject* target);
};

NS_FENNEX_END

#endif /* defined(__FenneX__ScrollingRecognizer__) */
