/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
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
 ****************************************************************************/

#ifndef __CCFLOAT_H__
#define __CCFLOAT_H__

#include "CCObject.h"
#include "ccMacros.h"

NS_CC_BEGIN

/**
 * @addtogroup data_structures
 * @{
 * @js NA
 * @lua NA
 */

class CC_DLL CCFloat : public CCObject
{
public:
    CCFloat(float v)
        : m_fValue(v) {}
    float getValue() const {return m_fValue;}

    static CCFloat* create(float v)
    {
        CCFloat* pRet = new CCFloat(v);
        if (pRet)
        {
            pRet->autorelease();
        }
        return pRet;
    }
    
    virtual CCObject* copyWithZone(CCZone* pZone)
    {
        CCAssert(pZone == NULL, "CCFloat should not be inherited.");
        CCFloat* pFloat = new CCFloat(m_fValue);
        return pFloat;
    }
    
    virtual bool isEqual(const CCObject* pObject)
    {
        bool bRet = false;
        const CCFloat* pFloat = dynamic_cast<const CCFloat*>(pObject);
        if (pFloat != NULL)
        {
            if (m_fValue == pFloat->getValue())
            {
                bRet = true;
            }
        }
        return bRet;
    }

    /* override functions */
    virtual void acceptVisitor(CCDataVisitor &visitor) { visitor.visit(this); }

private:
    float m_fValue;
};

// end of data_structure group
/// @}

NS_CC_END

#endif /* __CCFLOAT_H__ */
