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

#ifndef __FenneX__InertiaGenerator__
#define __FenneX__InertiaGenerator__

#include "Logs.h"
#include "cocos2d.h"
#include "Pausable.h"
#include "FenneXMacros.h"

USING_NS_CC;


NS_FENNEX_BEGIN
class Inertia;
class InertiaGenerator : public Ref, public Pausable
{
public:
    static InertiaGenerator* sharedInertia(void);
    
    void planSceneSwitch(EventCustom* event);
    void scrolling(EventCustom* eventj);
    void scrollingEnded(EventCustom* event);
    void stopInertia(Ref* obj);
    virtual void update(float delta);
    
    //If a tap is recognized, no inertia is generated
    void tapRecognized(EventCustom* event);
    void ignoreTouch(Touch* touch);
    
    void addPossibleTarget(Ref* target);
    CC_DEPRECATED_ATTRIBUTE void addPossibleTargets(CCArray* target);
    void addPossibleTargets(Vector<Ref*> target);
    
protected:
    void init();
    ~InertiaGenerator();
    
    Vector<Ref*> possibleTargets;
    Vector<Ref*> inertiaTargets;
    Vector<Inertia*> inertiaParameters;
    
    std::map<int, std::vector<Vec2>> lastOffsets;
    Vector<Touch*> ignoredTouches;
    
    float currentTime;
    float lastInertiaNotificationTime;
    Vector<EventListenerCustom*> listeners;
};
NS_FENNEX_END

#endif /* defined(__FenneX__InertiaGenerator__) */
