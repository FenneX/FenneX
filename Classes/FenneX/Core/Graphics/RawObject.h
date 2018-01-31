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

#ifndef __FenneX__RawObject__
#define __FenneX__RawObject__

#include "cocos2d.h"
USING_NS_CC;
#include "Pausable.h"
#include "SynthesizeString.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
class RawObject : public Ref
{
    CC_SYNTHESIZE_STRING(name, Name);
    CC_SYNTHESIZE_STRING(eventName, EventName);
    CC_SYNTHESIZE(bool, isEventActivated, EventActivated);
    CC_SYNTHESIZE_READONLY(int, identifier, ID);
public:
    
    //property-like methods that will actually call getNode
    virtual const Vec2& getPosition();
    virtual void setPosition(const Vec2& newPosition);
    virtual void setVisible(bool newVisible);
    virtual bool isVisible();
    const cocos2d::Size& getSize();
    const int getZOrder();
    const float getScale();
    void setScale(const float newScale);
    const float getScaleX();
    void setScaleX(const float newScale);
    const float getScaleY();
    void setScaleY(const float newScale);
    void setOpacity(GLubyte opacity);
    GLubyte getOpacity();
    void setOpacityRecursive(GLubyte opacity);
    
    ValueMap getEventInfos() const;//Warning : the returned ValueMap is copied, changes will not affect RawObject
    Value getEventInfo(std::string key) const;
    //Will not copy Sender automatically. Do it manually if required
    void addEventInfos(ValueMap infos);
    void setEventInfo(std::string key, Value obj);
    void removeEventInfo(std::string key);
    //TODO : add opacity, isMoving
    
    RawObject();
    virtual ~RawObject();
    
    //mandatory to do for sub-class
    virtual Node* getNode() = 0;
    //virtual void update(float deltaTime) = 0; //already in Ref
    
    //collisions methods
    virtual bool collision(Vec2 point);
    virtual bool collision(cocos2d::Rect rect);
    virtual bool containsRect(cocos2d::Rect rect);
    
    virtual void update(float deltatime) {};
    
    //TODO ? description, eventInfos handling, move methods, distance methods
    
    //Shitty code to go around the copy requirement
    virtual Ref* copyWithZone(void** pZone)
    {
        this->retain();
        return this;
    }
    
protected:
    ValueMap eventInfos;
};

bool operator<(const RawObject& obj1, const RawObject& obj2);
NS_FENNEX_END

#endif /* defined(__FenneX__RawObject__) */
