//
//  CustomDropDownList.cpp
//  FenneX
//
//  Created by Pierre Bertinet on 22/09/15.
//
//

#include "CustomDropDownList.h"
#include "Shorteners.h"

NS_FENNEX_BEGIN
void CustomDropDownList::updateTweenAction(float value, const std::string& key)
{
    if(key.compare("textureWidth") == 0)
    {
        setTextureRect(Rect(getTextureRect().origin.x, getTextureRect().origin.y, value, getTextureRect().size.height));
    }
    else if(key.compare("textureHeigth") == 0)
    {
        setTextureRect(Rect(getTextureRect().origin.x, getTextureRect().origin.y, getTextureRect().size.width, value));
    }
}
NS_FENNEX_END