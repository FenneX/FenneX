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
#include "DelayedDispatcher.h"
#include <string>
#include <sstream>
USING_NS_CC;

NS_FENNEX_BEGIN

//Warning: it doesn't work well in Fast mode when trying to check for non-cocos class. You should inline the dynamic cast in this case.
#define isKindOfClass(obj,class) (dynamic_cast<class*>((cocos2d::Ref*)obj) != nullptr)
#define isKindOf(obj, class) (dynamic_cast<class*>(obj) != nullptr)
#define isValueOfType(obj,type) (!obj.isNull() && obj.getType() == cocos2d::Value::Type::type)
#define valueMapContains(obj, name, type) (obj.find(name) != obj.end() && obj.at(name).getType() == cocos2d::Value::Type::type)

#define IFEXIST(obj) if(obj != nullptr) (obj)

/* creation shorteners : since those are widly used, shortening the name makes sense (much like ccp), as well as uniformizing the format
 * format *create where * is the type
 I = Integer
 F = Float
 B = Bool
 S = String, append F for format
 D = Dictionary, append P for parameters
 A = Array, append P for parameters
 */

cocos2d::Size* sizeCreate(float width = 0, float height = 0);
//note : keys have to be passed as CCString, unfortunately. Must be nullptr terminated

static inline cocos2d::Size
SizeMult(const cocos2d::Size& v, const float s)
{
    return cocos2d::Size(v.width*s, v.height*s);
}

static inline bool isColorEqual(const Color3B left, const Color3B right)
{
    return left.r == right.r && left.g == right.g && left.b == right.b;
}

static inline bool hasEnding (const std::string &fullString, const std::string &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

static inline float getTimeDifferenceMS(const timeval& start, const timeval& end)
{
    return (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
}

//std::to_string isn't always defined on Android, use this method as a replacement.
template < typename T > std::string to_string( const T& n )
{
    std::ostringstream stm;
    stm << n ;
    return stm.str();
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
