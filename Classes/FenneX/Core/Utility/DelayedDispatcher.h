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

#ifndef __FenneX__DelayedDispatcher__
#define __FenneX__DelayedDispatcher__

#include "cocos2d.h"
#include "Pausable.h"

NS_FENNEX_BEGIN

typedef std::tuple<float, std::string, cocos2d::Ref*> EventTuple;
typedef std::tuple<float, std::function<void(cocos2d::EventCustom*)>, cocos2d::Ref*, std::string> FuncTuple;

/* DelayedDispatcher works by attaching itself to current scene and monitoring updates.
 For retro-compatibility, it only works for current scene
 */
class DelayedDispatcher : public cocos2d::Ref, public Pausable
{
public:
    ~DelayedDispatcher();
    static void eventAfterDelay(std::string eventName, Ref* userData, float delay);
    static void funcAfterDelay(std::function<void(cocos2d::EventCustom*)> func, Ref* userData, float delay, std::string eventName="");
    //Return true if at least one was cancelled
    static bool cancelEvents(std::string eventName);
    static bool cancelFuncs(std::string eventName);
    void update(float deltaTime);
private:
    static DelayedDispatcher* getInstance();
    std::vector<EventTuple> events;
    std::vector<FuncTuple> funcs;
};

NS_FENNEX_END

#endif /* defined(__FenneX__DelayedDispatcher__) */
