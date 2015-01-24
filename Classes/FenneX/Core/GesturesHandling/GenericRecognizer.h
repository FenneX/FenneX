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

#ifndef __FenneX__GenericRecognizer__
#define __FenneX__GenericRecognizer__

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "TouchLinker.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//Provides some basic features for superclass
class GenericRecognizer : public Ref
{
    //the mainLinker used to ignore some touch (Selection) or group gestures by target (Pinch and Scrolling)
    CC_SYNTHESIZE(TouchLinker*, mainLinker, Linker);
    
public:    
    virtual bool onTouchBegan(Touch *pTouch, Event *pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent);return false;};
    virtual void onTouchMoved(Touch *pTouch, Event *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent);}
    virtual void onTouchEnded(Touch *pTouch, Event *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent);}
    virtual void onTouchCancelled(Touch *pTouch, Event *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent);}
    
    //Clean all ongoing touches from the Recognizer. You should call it when stopping using it, so that next session doesn't have ghost touches.
    virtual void cleanTouches() {};
protected:
    bool isInLinker(Touch* touch);
    Vector<Touch*> unlinkedTouches();
};
NS_FENNEX_END

#endif /* defined(__FenneX__GenericRecognizer__) */
