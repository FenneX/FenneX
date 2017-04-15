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

#include "CustomObject.h"
#include "Shorteners.h"
#include "AppMacros.h"

NS_FENNEX_BEGIN
Rect CustomObject::getBoundingBox()
{
    return Rect(this->getNode()->getPositionX(), this->getNode()->getPositionY(), this->getNode()->getContentSize().width, this->getNode()->getContentSize().height);
}

Node* CustomObject::getNode()
{
    CCAssert(delegate != NULL, "CustomObject getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}

void CustomObject::setNode(Node* node)
{
    node->setPosition(delegate->getPosition());
    node->setScaleX(delegate->getScaleX());
    node->setScaleY(delegate->getScaleY());
    node->setAnchorPoint(delegate->getAnchorPoint());
    node->setLocalZOrder(delegate->getLocalZOrder());
    node->setVisible(delegate->isVisible());
    delegate->getParent()->addChild(node);
    delegate->removeFromParentAndCleanup(true);
    delegate->release();
    
    delegate = node;
    delegate->retain();
}

CustomObject::CustomObject():
delegate(NULL)
{
    
}

CustomObject::CustomObject(Node* child)
{
    delegate = child;
    if(delegate == NULL)
    {
        log("Problem with CustomObject, delegate is NULL, the application will crash");
    }
    delegate->retain();
}

CustomObject::CustomObject(Node* child, Vec2 location)
{
    delegate = child;
    if(delegate == NULL)
    {
        log("Problem with CustomObject, delegate is NULL, the application will crash");
    }
    delegate->retain();
    this->setPosition(location);
}

CustomObject::~CustomObject()
{
    delegate->release();
#if VERBOSE_DEALLOC
    if(this->getName() != NULL)
    {
        log("Dealloc CustomObject %s", this->getName());
    }
    else
    {
        log("Dealloc unnamed CustomObject");
    }
#endif
}
NS_FENNEX_END
