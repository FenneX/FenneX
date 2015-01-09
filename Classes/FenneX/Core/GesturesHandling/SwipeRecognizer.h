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

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "GenericRecognizer.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//use http://devblog.wm-innovations.com/2010/03/30/custom-swipe-uitableviewcell/ for the algorithm

//instead of evaluating a swipe at each movement, swipe are evaluated only at touchEnded
//throw event Swipe with Touch, Direction (CCString : Left or Right) and InitialPosition (CCPoint) as argument
//support multi-touch context, and will ignore any touch linked in the given mainLinker
//currently only recognize horizontal swipes
class SwipeRecognizer : public GenericRecognizer
{
    CC_SYNTHESIZE(float, minSpeed, MinSpeed);
    CC_SYNTHESIZE(float, minMovement, MinMovement);
public:
    static SwipeRecognizer* sharedRecognizer(void);
    
    virtual bool onTouchBegan(CCTouch *touch, CCEvent *pEvent);
    virtual void onTouchMoved(CCTouch *touch, CCEvent *pEvent);
    virtual void onTouchEnded(CCTouch *touch, CCEvent *pEvent);
    virtual void cleanTouches();
    void cancelRecognitionForTouch(CCTouch *touch);
protected:
    void init();
    
protected:
    CCDictionary* touchStart;
    std::map<int, Vec2> touchInitialPosition;
};
NS_FENNEX_END

#endif /* defined(__FenneX__SwipeRecognizer__) */
