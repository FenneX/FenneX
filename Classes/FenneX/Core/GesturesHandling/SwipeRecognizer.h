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

#ifndef __FenneX__SwipeRecognizer__
#define __FenneX__SwipeRecognizer__

#include "cocos2d.h"
USING_NS_CC;
#include "DelegatingRecognizer.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//use http://devblog.wm-innovations.com/2010/03/30/custom-swipe-uitableviewcell/ for the algorithm

class SwipeDelegate{
public:
    virtual void swipeRecognized(Touch* touch, std::string direction, Vec2 initialPosition) = 0;
};

//instead of evaluating a swipe at each movement, swipe are evaluated only at touchEnded
//support multi-touch context, and will ignore any touch linked in the given mainLinker
//currently only recognize horizontal swipes
// You need to subscribe to the recognizer by adding a delegate (it'll launch the swipeRecognized method)
class SwipeRecognizer : public DelegatingRecognizer<SwipeDelegate>
{
    CC_SYNTHESIZE(float, minSpeed, MinSpeed);
    CC_SYNTHESIZE(float, minMovement, MinMovement);
public:
    static SwipeRecognizer* sharedRecognizer(void);
    
    virtual bool onTouchBegan(Touch *touch, Event *pEvent);
    virtual void onTouchMoved(Touch *touch, Event *pEvent);
    virtual void onTouchEnded(Touch *touch, Event *pEvent);
    virtual void cleanTouches();
    void cancelRecognitionForTouch(Touch *touch);
protected:
    void init();
    
protected:
    std::map<int, float> touchStart;
    std::map<int, Vec2> touchInitialPosition;
    
};
NS_FENNEX_END

#endif /* defined(__FenneX__SwipeRecognizer__) */
