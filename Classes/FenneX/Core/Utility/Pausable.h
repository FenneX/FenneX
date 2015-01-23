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

#ifndef __FenneX__Pausable__
#define __FenneX__Pausable__

#include "cocos2d.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
class Pausable
{
public:
    //The update method is the only one you HAVE to implement and which is guaranteed to be called.
    //virtual void update(float deltaTime) = 0; //Well, seems like it's already implemented by Ref, and obviously this cause a diamond of death during method resolution if left here ....
    virtual void update(float deltaTime) = 0;
    virtual void pause() {}
    virtual void resume() {}
    virtual void stop() {}
};
NS_FENNEX_END

#endif /* defined(__FenneX__Pausable__) */
