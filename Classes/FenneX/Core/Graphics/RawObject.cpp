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
        for(RawObject* target : ((Panel*)this)->getChildren())
        {
            target->setOpacityRecursive(opacity);
        }
    }
    else
    {
        this->getNode()->setOpacity(opacity);
    }
}

ValueMap RawObject::getEventInfos() const
{
    ValueMap infos = eventInfos;
    if(infos["Sender"].isNull())
    {
        infos["Sender"] = Value(identifier);
    }
    return infos;
}

Value RawObject::getEventInfo(std::string key) const
{
    return getEventInfos()[key];
}

void RawObject::setEventInfo(std::string key, Value obj)
{
    eventInfos[key] = obj;
}

void RawObject::addEventInfos(ValueMap infos)
{
    for(ValueMap::iterator it = infos.begin(); it != infos.end(); ++it) {
        if(it->first != "Sender")
        {
            eventInfos[it->first] = it->second;
        }
    }
}

void RawObject::removeEventInfo(std::string key)
{
    eventInfos.erase(key);
}

RawObject::RawObject():
name(""),
eventName(""),
isEventActivated(true)
{
    eventInfos = ValueMap();
    identifier = GraphicLayer::sharedLayer()->getNextId();
}

RawObject::~RawObject()
{
    eventInfos.clear();
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
    if(rect.origin.x + rect.size.width / 2 * this->getScaleX() >= this->getPosition().x - this->getSize().width / 2 * this->getScaleX()
       && rect.origin.x - rect.size.width / 2 * this->getScaleX() < this->getPosition().x + this->getSize().width * (1-this->getNode()->getAnchorPoint().x) * this->getScaleX()
       && rect.origin.y + rect.size.height / 2 * this->getScaleY() >= this->getPosition().y - this->getSize().height * this->getNode()->getAnchorPoint().y * this->getScaleY()
       && rect.origin.y - rect.size.height / 2 * this->getScaleY() < this->getPosition().y + this->getSize().height * (1-this->getNode()->getAnchorPoint().y) * this->getScaleY())
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
    if(isValueOfType(obj1.getEventInfos()["Order"], INTEGER) &&
       isValueOfType(obj2.getEventInfos()["Order"], INTEGER))
    {
        int obj1Order = obj1.getEventInfos()["Order"].asInt();
        int obj2Order = obj2.getEventInfos()["Order"].asInt();
        return obj1Order < obj2Order;
    }
    if(isValueOfType(obj1.getEventInfos()["Index"], INTEGER) &&
       isValueOfType(obj2.getEventInfos()["Index"], INTEGER))
    {
        int obj1Order = obj1.getEventInfos()["Index"].asInt();
        int obj2Order = obj2.getEventInfos()["Index"].asInt();
        return obj1Order < obj2Order;
    }
    //If all else fails, order them by screen zorder
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    return layer->isInFront(layer->first(obj1.getID()), layer->first(obj2.getID()));
}
NS_FENNEX_END
