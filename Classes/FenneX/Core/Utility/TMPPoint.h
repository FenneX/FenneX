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

#ifndef __Puzzle__TMPPoint__
#define __Puzzle__TMPPoint__

#include "cocos2d.h"

USING_NS_CC;

/* This class is a stop-gap fix to the fact you can't pass Vec2 in Array and Dictionary (since modifying Vec2 to be passed causes DrawNode to stop working)
 */

class CC_DEPRECATED_ATTRIBUTE TMPPoint : public Ref, public Clonable
{
public:
    float x;
    float y;
    
    static TMPPoint* create();
    static TMPPoint* create(float x, float y);
    static TMPPoint* create(Vec2 pos);
    
    TMPPoint();
    TMPPoint(float x, float y);
    TMPPoint(Vec2 pos);
    
    virtual TMPPoint* clone() const
    {
        auto v = new TMPPoint(x, y);
        v->autorelease();
        return v;
    }
};

#endif /* defined(__Puzzle__TMPPoint__) */
