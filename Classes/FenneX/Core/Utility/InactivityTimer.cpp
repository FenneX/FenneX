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

#include "InactivityTimer.h"
#include "FenneXCore.h"

static int timerDuration = -1;
static InactivityTimer* instance = NULL;

InactivityTimer* InactivityTimer::getInstance()
{
    if(instance == NULL)
    {
        instance = new InactivityTimer();
    }
    return instance;
}

InactivityTimer::InactivityTimer()
{
    Director::getInstance()->getEventDispatcher()->addCustomEventListener("AppEnterBackground", std::bind(&InactivityTimer::resetTimer));
}

void InactivityTimer::resetTimer()
{
    instance->timePassedSinceLastActivity = 0;
    instance->eventLaunched = false;
}

void InactivityTimer::setTimerDuration(int timer)
{
    timerDuration = timer;
}

void InactivityTimer::update(float delta)
{
    timePassedSinceLastActivity += delta;
    if(timePassedSinceLastActivity > timerDuration && timerDuration != -1 && !eventLaunched)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("UserInactivityDetected");
        eventLaunched = true;
    }
}
