#ifndef __CCINTEGER_H__
#define __CCINTEGER_H__

#include "CCObject.h"
#include "ccMacros.h"

NS_CC_BEGIN

/**
 * @addtogroup data_structures
 * @{
 * @js NA
 */

class CC_DLL CCInteger : public CCObject
{
public:
    CCInteger(int v)
        : m_nValue(v) {}
    int getValue() const {return m_nValue;}

    static CCInteger* create(int v)
    {
        CCInteger* pRet = new CCInteger(v);
        pRet->autorelease();
        return pRet;
    }
    
    virtual CCObject* copyWithZone(CCZone* pZone)
    {
        CCAssert(pZone == NULL, "CCInteger should not be inherited.");
        CCInteger* pInt = new CCInteger(m_nValue);
        return pInt;
    }
    
    virtual bool isEqual(const CCObject* pObject)
    {
        bool bRet = false;
        const CCInteger* pInt = dynamic_cast<const CCInteger*>(pObject);
        if (pInt != NULL)
        {
            if (m_nValue == pInt->getValue())
            {
                bRet = true;
            }
        }
        return bRet;
    }

    /* override functions 
     * @lua NA
     */
    virtual void acceptVisitor(CCDataVisitor &visitor) { visitor.visit(this); }

private:
    int m_nValue;
};

// end of data_structure group
/// @}

NS_CC_END

#endif /* __CCINTEGER_H__ */
