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

#ifndef __FenneX__Panel__
#define __FenneX__Panel__

#include "cocos2d.h"
USING_NS_CC;
#include "RawObject.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN

//TODO : finish implementation when needed : scrapped open/close for now, which is not needed yet
class Panel : public RawObject
{
public:
    virtual Node* getNode();
    
    //WARNING : experimental method, used to replace the standard node by a ClippingNode
    void setNode(Node* node);
    void setClippingNode(); //Will replace itself by a ClippingNode with a DrawNode stencil using ContentSize
    
    Panel(std::string panelName, Vec2 location);
    Panel(Node* node, std::string panelName = "");
    ~Panel();
    
    void addChild(RawObject* child);
    bool containsObject(RawObject* child);
    void removeChild(RawObject* child);
    void reorderChild(RawObject* child, int zOrder);
    void clear();
    Vector<RawObject*> getChildren();
    
    //TODO : reorder methods when needed by GraphicLayer (protected and friend GraphicLayer ?)
    
    virtual void update(float deltaTime);
    
    virtual bool collision(Vec2 point);
    virtual bool collision(cocos2d::Rect rect);
    virtual bool containsRect(cocos2d::Rect rect);
    
protected:
    //the actual rendering layer for graphic objects stored
    Node* delegate;
    
    //Array containing graphic objects
    Vector<RawObject*> children;
};
NS_FENNEX_END

#endif /* defined(__FenneX__Panel__) */
