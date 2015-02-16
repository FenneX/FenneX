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

#ifndef FenneX_Shorteners_h
#define FenneX_Shorteners_h

#include "cocos2d.h"
#include "SceneSwitcher.h"
#include "FenneXMacros.h"
#include "TMPPoint.h"
#include "DelayedDispatcher.h"
USING_NS_CC;

NS_FENNEX_BEGIN
#define isKindOfClass(obj,class) (dynamic_cast<class*>(obj) != NULL)

//Compensate for scale factor for image picked by user
#define AUTO_SCALE (Fcreate(Director::getInstance()->getContentScaleFactor() ))

#define IFEXIST(obj) if(obj != NULL) (obj)

#define REAL_SCALE(obj) (Fcreate(obj \
* Director::getInstance()->getContentScaleFactor() ))

#define TOINT(obj) (((CCInteger*)obj)->getValue())
#define TOFLOAT(obj) (((CCFloat*)obj)->getValue())
#define TOBOOL(obj) (((CCBool*)obj)->getValue())
#define TOCSTRING(obj) (((CCString*)obj)->getCString())
#define TOPOINT(obj) Vec2(((TMPPoint*)obj)->x, ((TMPPoint*)obj)->y)

/* creation shorteners : since those are widly used, shortening the name makes sense (much like ccp), as well as uniformizing the format
 * format *create where * is the type
 P = Point
 I = Integer
 F = Float
 B = Bool
 S = String, append F for format
 D = Dictionary, append P for parameters
 A = Array, append P for parameters
 */

#define Pcreate TMPPoint::create
#define Icreate CCInteger::create
#define Fcreate CCFloat::create
#define Bcreate CCBool::create
#define Screate CCString::create
#define ScreateF CCString::createWithFormat
#define Dcreate CCDictionary::create
#define DcreateP createDictionaryWithParameters
#define Acreate CCArray::create
#define AcreateP createArrayWithParameters

CCString* getResourcesPath(const char* file);

cocos2d::Size* sizeCreate(float width = 0, float height = 0);
//note : keys have to be passed as CCString, unfortunately. Must be NULL terminated
CCDictionary* createDictionaryWithParameters(Ref* firstObject, ... );
CCArray* createArrayWithParameters(Ref* firstObject, ... );

//perform a selector after a delay. All pending selector will be cancelled during a scene switch (it's assumed most of them are tied to the current Scene)
/* Obsolete functions. Please use DelayedDispatcher
 
 Replaced by DelayedDispatcher::funcAfterDelay (requires to merge first 2 argument into a std::function, and change delay/object order)
void performSelectorAfterDelay(Ref* target, SEL_CallFuncO selector, float delay, Ref* object = NULL);
 
 Replaced by DelayedDispatcher::cancelFunc (requires to use std::string as identifiers)
bool cancelSelector(Ref* target, SEL_CallFuncO selector);
 
 Replaced by DelayedDispatcher::eventAfterDelay (simple replace)
void performNotificationAfterDelay(const char* name, Ref* obj, float delay);*/

#define performNotificationAfterDelay DelayedDispatcher::eventAfterDelay

static inline cocos2d::Size
SizeMult(const cocos2d::Size& v, const float s)
{
    return cocos2d::Size(v.width*s, v.height*s);
}

static inline bool isColorEqual(const Color3B left, const Color3B right)
{
    return left.r == right.r && left.g == right.g && left.b == right.b;
}

static inline bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

static inline float getTimeDifferenceMS(timeval& start, timeval& end)
{
    return ((((end.tv_sec - start.tv_sec)*1000.0f
             +end.tv_usec) - start.tv_usec) / 1000.0f);
}
/*
class Shorteners : public Ref
{
public:
    static Shorteners* sharedClass(void);
    //Reserved for internal usage, need "_NotificationName" as a CCString and "_Infos" as a Ref
    void delayedPostNotification(Ref* obj);
private:
};*/
NS_FENNEX_END

#endif
