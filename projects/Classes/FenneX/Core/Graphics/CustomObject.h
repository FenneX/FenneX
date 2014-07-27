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

#ifndef __FenneX__CustomObject__
#define __FenneX__CustomObject__

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "RawObject.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//use that object to provide a specific CCNode (for example CCScale9Sprite)
class CustomObject : public RawObject
{
public:
    CCRect getBoundingBox();
    virtual CCNode* getNode();
    virtual void setNode(CCNode* node);
    
    CustomObject();
    CustomObject(CCNode* child);
    CustomObject(CCNode* child, CCPoint location);
    ~CustomObject();
    
    
protected:
    //the actual CCNode which will perform cocos2d actions
    CCNode* delegate;
};
NS_FENNEX_END

#endif /* defined(__FenneX__CustomObject__) */
