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

#include "RawObject.h"
#include "GraphicLayer.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN
const Vec2& RawObject::getPosition()
{
    return this->getNode()->getPosition();
}
void RawObject::setPosition(const Vec2& newPosition)
{
    this->getNode()->setPosition(newPosition);
}
void RawObject::setVisible(bool newVisible)
{
    this->getNode()->setVisible(newVisible);
}
bool RawObject::isVisible()
{
    return this->getNode()->isVisible();
}
const Size& RawObject::getSize()
{
    return this->getNode()->getContentSize();
}
const int RawObject::getZOrder()
{
    return this->getNode()->getLocalZOrder();
}
const float RawObject::getScale()
{
    return MAX(this->getNode()->getScaleX(), this->getNode()->getScaleY());
}
void RawObject::setScale(const float newScale)
{
    this->getNode()->setScale(newScale);
}
const float RawObject::getScaleX()
{
    return this->getNode()->getScaleX();
}
void RawObject::setScaleX(const float newScale)
{
    this->getNode()->setScaleX(newScale);
}
const float RawObject::getScaleY()
{
    return this->getNode()->getScaleY();
}
void RawObject::setScaleY(const float newScale)
{
    this->getNode()->setScaleY(newScale);
}

void RawObject::setOpacity(GLubyte opacity)
{
    this->getNode()->setOpacity(opacity);
}

GLubyte RawObject::getOpacity()
{
    return this->getNode()->getOpacity();
}

void RawObject::setOpacityRecursive(GLubyte opacity)
{
    if(isKindOfClass(this, Panel))
    {
        for(int i = 0; i < ((Panel*)this)->getChildren()->count(); i++)
        {
            RawObject* target = (RawObject*)((Panel*)this)->getChildren()->objectAtIndex(i);
            target->setOpacityRecursive(opacity);
        }
    }
    else
    {
        this->getNode()->setOpacity(opacity);
    }
}

CCDictionary* RawObject::getEventInfos() const
{
    CCDictionary* infos = CCDictionary::createWithDictionary(eventInfos);
    if(infos->objectForKey("Sender") == NULL)
    {
        infos->setObject(Icreate(identifier), "Sender");
    }
    return infos;
}

void RawObject::setEventInfo(Ref* obj, std::string key)
{
    eventInfos->setObject(obj, key);
}

void RawObject::addEventInfos(CCDictionary* infos)
{
    CCArray* keys = infos->allKeys();
    Ref* keyObj;
    CCARRAY_FOREACH(keys, keyObj)
    {
        std::string key = ((CCString*)keyObj)->getCString();
        if(key != "Sender")
        {
            this->setEventInfo(infos->objectForKey(key), key);
        }
    }
}

void RawObject::setEventInfo(std::string key, Value obj)
{
    setEventInfo(valueToRef(obj), key);
}

void RawObject::addEventInfos(ValueMap infos)
{
    for(auto iter = infos.begin(); iter != infos.end(); iter++)
    {
        if(iter->first != "Sender")
        {
            this->setEventInfo(iter->first, iter->second);
        }
    }
}

void RawObject::removeEventInfo(std::string key)
{
    eventInfos->removeObjectForKey(key);
}

RawObject::RawObject():
name(""),
eventName(""),
isEventActivated(true)
{
    eventInfos = CCDictionary::create();
    eventInfos->retain();
    identifier = GraphicLayer::sharedLayer()->getNextId();
}

RawObject::~RawObject()
{
    eventInfos->release();
}

bool RawObject::collision(Vec2 point)
{
    if(point.x >= this->getPosition().x - this->getSize().width * this->getNode()->getAnchorPoint().x  * this->getScaleX()
       && point.x <= this->getPosition().x + this->getSize().width * (1-this->getNode()->getAnchorPoint().x)  * this->getScaleX()
       && point.y >= this->getPosition().y - this->getSize().height * this->getNode()->getAnchorPoint().y * this->getScaleY()
       && point.y <= this->getPosition().y + this->getSize().height * (1-this->getNode()->getAnchorPoint().y) * this->getScaleY())
    {
        return true;
    }
    return false;
}

bool RawObject::collision(Rect rect)
{
    if(rect.origin.x + rect.size.width / 2 * this->getScale() >= this->getPosition().x - this->getSize().width / 2 * this->getScaleX()
       && rect.origin.x - rect.size.width / 2 * this->getScale() < this->getPosition().x + this->getSize().width * (1-this->getNode()->getAnchorPoint().x) * this->getScaleX()
       && rect.origin.y + rect.size.height / 2 * this->getScale() >= this->getPosition().y - this->getSize().height * this->getNode()->getAnchorPoint().y * this->getScaleY()
       && rect.origin.y - rect.size.height / 2 * this->getScale() < this->getPosition().y + this->getSize().height * (1-this->getNode()->getAnchorPoint().y) * this->getScaleY())
    {
        return true;
    }
    return false;
}

bool RawObject::containsRect(Rect rect)
{
    if(rect.origin.x - rect.size.width / 2 >= this->getPosition().x - this->getSize().width * this->getNode()->getAnchorPoint().x * this->getScaleX()
       && rect.origin.x + rect.size.width /2 < this->getPosition().x + this->getSize().width * (1-this->getNode()->getAnchorPoint().x) * this->getScaleX()
       && rect.origin.y - rect.size.height / 2 >= this->getPosition().y - this->getSize().height * this->getNode()->getAnchorPoint().y * this->getScaleY()
       && rect.origin.y + rect.size.height / 2 < this->getPosition().y + this->getSize().height * (1-this->getNode()->getAnchorPoint().y) * this->getScaleY())
    {
        return true;
    }
    return false;
}

bool operator<(const RawObject& obj1, const RawObject& obj2)
{
    if(isKindOfClass(obj1.getEventInfos()->objectForKey("Order"), CCInteger) && isKindOfClass(obj2.getEventInfos()->objectForKey("Order"), CCInteger))
    {
        int obj1Order = TOINT(obj1.getEventInfos()->objectForKey("Order"));
        int obj2Order = TOINT(obj2.getEventInfos()->objectForKey("Order"));
        return obj1Order < obj2Order;
    }
    if(isKindOfClass(obj1.getEventInfos()->objectForKey("Index"), CCInteger) && isKindOfClass(obj2.getEventInfos()->objectForKey("Index"), CCInteger))
    {
        int obj1Order = TOINT(obj1.getEventInfos()->objectForKey("Index"));
        int obj2Order = TOINT(obj2.getEventInfos()->objectForKey("Index"));
        return obj1Order < obj2Order;
    }
    //If all else fails, order them by screen zorder
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    return layer->isInFront(layer->getById(obj1.getID()), layer->getById(obj2.getID()));
}
NS_FENNEX_END
