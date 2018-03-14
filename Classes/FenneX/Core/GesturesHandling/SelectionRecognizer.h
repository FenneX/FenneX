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

#ifndef __FenneX__SelectionRecognizer__
#define __FenneX__SelectionRecognizer__

#include "cocos2d.h"
USING_NS_CC;
#include "DelegatingRecognizer.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN

class SelectionDelegate
{
public:
    virtual void selectionRecognized(Touch* touch, RawObject* target) = 0; // When a selection is detected
    virtual void selectionStarted(Touch* touch) = 0;  // When a touch event start, which might be a selection
    virtual void selectionMoved(Touch* touch) = 0; // When a touch event moved, which might be a selection
    virtual void selectionCanceled(Touch* touch, Vec2 origin, RawObject* target = NULL) = 0; // When the touch event can no longer be a selection, either because it moved too far or because the touch ended
};

class SelectionRecognizer : public DelegatingRecognizer<SelectionDelegate>
{
    CC_SYNTHESIZE(float, maxMovement, MaxMovement);
    CC_SYNTHESIZE(float, duration, Duration);
public:
    static SelectionRecognizer* sharedRecognizer(void);
    
    virtual bool onTouchBegan(Touch *touch, Event *pEvent);
    virtual void onTouchMoved(Touch *touch, Event *pEvent);
    virtual void onTouchEnded(Touch *touch, Event *pEvent);
    virtual void cleanTouches();
    bool isTouchInSelection(Touch *touch);
    void cancelSelectionForTouch(Touch *touch);

protected:
    void init();
    
protected:
    std::map<int, Vec2> storedTouches;//key : touch ID, value : origin
    void checkForSelection(EventCustom* event);
};
NS_FENNEX_END

#endif /* defined(__FenneX__SelectionRecognizer__) */
