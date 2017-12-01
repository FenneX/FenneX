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

#ifndef __FenneX__TapRecognizer__
#define __FenneX__TapRecognizer__

#include "cocos2d.h"
USING_NS_CC;
#include "DelegatingRecognizer.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN

class TapDelegate
{
public:
    virtual void tapRecognized(Touch* touch) = 0;
};

//Doesn't need to be updated
// You need to subscribe to the recognizer by adding a delegate (it'll launch the tapRecognized method)
class TapRecognizer : public DelegatingRecognizer<TapDelegate>
{
public:
    static TapRecognizer* sharedRecognizer(void);
    
    virtual bool onTouchBegan(Touch *touch, Event *pEvent);
    virtual void onTouchMoved(Touch *touch, Event *pEvent);
    virtual void onTouchEnded(Touch *touch, Event *pEvent);
    virtual void cleanTouches();
    void cancelRecognitionForTouch(Touch* touch);
protected:
    void init();
    
protected:
    std::map<int, float> touchStart;
    std::map<int, Vec2> touchInitialPosition;
};
NS_FENNEX_END

#endif /* defined(__FenneX__TapRecognizer__) */
