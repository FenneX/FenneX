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

#include "TMPPoint.h"


TMPPoint* TMPPoint::create()
{
    TMPPoint* p = new TMPPoint();
    p->autorelease();
    return p;
}

TMPPoint* TMPPoint::create(float x, float y)
{
    TMPPoint* p = new TMPPoint(x, y);
    p->autorelease();
    return p;
}

TMPPoint* TMPPoint::create(Vec2 pos)
{
    TMPPoint* p = new TMPPoint(pos);
    p->autorelease();
    return p;
}

TMPPoint::TMPPoint()
{
    
}

TMPPoint::TMPPoint(float x, float y)
{
    this->x = x;
    this->y = y;
}

TMPPoint::TMPPoint(Vec2 pos)
{
    this->x = pos.x;
    this->y = pos.y;
}