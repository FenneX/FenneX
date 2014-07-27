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

#include "ClippingNode.h"

NS_FENNEX_BEGIN
void ClippingNode::visit()
{
    if (!this->isVisible()) {
        return;
    }
    glEnable(GL_SCISSOR_TEST);
    
    CCPoint bottomLeft = this->convertToWorldSpaceAR(ccp(this->getContentSize().width * this->getAnchorPoint().x * -1, this->getContentSize().height  * this->getAnchorPoint().y * -1));
    CCPoint topRight = this->convertToWorldSpaceAR(ccp(this->getContentSize().width * (1 - this->getAnchorPoint().x), this->getContentSize().height * (1 - this->getAnchorPoint().y)));
    topRight = ccpSub(topRight, bottomLeft);
    glScissor(bottomLeft.x,
              bottomLeft.y,
              topRight.x,
              topRight.y);
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
}


ClippingNode* ClippingNode::create()
{
    ClippingNode* newNode = new ClippingNode();
    newNode->autorelease();
    return newNode;
}

void ClippingNode::updateTweenAction(float value, const char* key)
{
    if(strcmp(key, "width") == 0)
    {
        setContentSize(CCSize(value, getContentSize().height));
    }
    else if(strcmp(key, "height") == 0)
    {
        setContentSize(CCSize(getContentSize().width, value));
    }
}
NS_FENNEX_END