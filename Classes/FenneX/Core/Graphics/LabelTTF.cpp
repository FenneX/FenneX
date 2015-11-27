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

#include "LabelTTF.h"
#include <stdio.h>
#include "Shorteners.h"
#include "CustomLabel.h"
#include "AppMacros.h"
#include "StringUtility.h"

using namespace std;

NS_FENNEX_BEGIN
Rect LabelTTF::getBoundingBox()
{
    return Rect(delegate->getPositionX(), delegate->getPositionY(), delegate->getContentSize().width, delegate->getContentSize().height);
}

Node* LabelTTF::getNode()
{
    CCAssert(delegate != NULL, "Label getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}


void LabelTTF::setDimensions(Size dimensions)
{
    realDimensions = dimensions;
    adjustLabel();
}

Size LabelTTF::getDimensions()
{
    return realDimensions;
}

void LabelTTF::setLabelValue(const char* value, bool async)
{
    if(async && !std::string(value).empty())
    {
        loadingValue = value;
    }
    else
    {
        /*if(delegate->isUpdateEnabled())
        { //group those 2 calls in a single updateTexture
            delegate->enableUpdate(false);
            delegate->setDimensions(realDimensions.width / this->getScale(), 0);
            delegate->setString(value);
            delegate->enableUpdate(true);
        }
        else
        {*/
        delegate->setDimensions(realDimensions.width / this->getScaleX(), 0);
        delegate->setString(value);
        //}
        this->adjustLabel();
    }
}

const char* LabelTTF::getLabelValue()
{
    return delegate->getString().c_str();
}

const char* LabelTTF::getFontFile()
{
    return fontFile->getCString();
}

float LabelTTF::getFontSize()
{
    return delegate->getSystemFontSize();
    //return delegate->getTTFConfig().fontSize;
}

void LabelTTF::setFontSize(float size)
{
    delegate->setSystemFontSize(size);
    /*TTFConfig newConfig = delegate->getTTFConfig();
    newConfig.fontSize = size;
    delegate->setTTFConfig(newConfig);*/
}

void LabelTTF::setFont(std::string filename)
{
    
    fontFile = new CCString(filename);
    fullFontFile = new CCString(filename);
    CCString* fontFile = CCString::create(filename);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //ios doesn't like full path and ttf extension
    std::string fontFileWithoutTTF = fontFile->getCString();
    long extensionPos = fontFileWithoutTTF.find_last_of(".ttf");
    if(extensionPos != std::string::npos)
    {
        fontFileWithoutTTF = fontFileWithoutTTF.substr(0, extensionPos - 3);
    }
    long slashPos = fontFileWithoutTTF.find_last_of('/');
    if(slashPos != std::string::npos)
    {
        fontFileWithoutTTF = fontFileWithoutTTF.substr(slashPos+1);
    }
#endif
    delegate->setSystemFontName(fontFile->getCString());
}

LabelTTF::LabelTTF() :
delegate(NULL),
loadingValue("")
{
}

LabelTTF::LabelTTF(const char* labelString, const char* filename, Vec2 location, Size dimensions, TextHAlignment format) :
loadingValue("")
{
    name = labelString;
    fitType = ResizeFont;
    fontFile = new CCString(filename);
    fullFontFile = new CCString(filename);
    alignment = format;
    int sizeBegin = -1, sizeEnd = -1;
    for(int i = 0; i < strlen(filename); i++)
    {
        bool isNumber = filename[i] >= '0' && filename[i] <= '9';
        if(isNumber && sizeBegin == -1)
        {
            sizeBegin = i;
        }
        if(!isNumber && sizeBegin != -1 && sizeEnd == -1)
        {
            sizeEnd = i;
        }
    }
    if(sizeEnd == -1 || sizeBegin == -1)
    {
        CCLOG("LabelTTF : incorrect font formating, use : FontnameSizeColor");
        return;
    }
    CCString* fontFile = CCString::createWithData((const unsigned char*)filename, sizeBegin);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //ios doesn't like full path and ttf extension
    std::string fontFileWithoutTTF = fontFile->getCString();
    long extensionPos = fontFileWithoutTTF.find_last_of(".ttf");
    if(extensionPos != std::string::npos)
    {
        fontFileWithoutTTF = fontFileWithoutTTF.substr(0, extensionPos - 3);
    }
    long slashPos = fontFileWithoutTTF.find_last_of('/');
    if(slashPos != std::string::npos)
    {
        fontFileWithoutTTF = fontFileWithoutTTF.substr(slashPos+1);
    }
#endif
    CCString* fontSize = CCString::createWithData((const unsigned char*)(filename + sizeBegin), sizeEnd - sizeBegin);
    CCString* color = CCString::createWithData((const unsigned char*)(filename + sizeEnd), strlen(filename) - sizeEnd);
    delegate = Label::create(labelString, fontFile->getCString(), fontSize->intValue());
    delegate->retain();
    delegate->setPosition(location);
    delegate->setHorizontalAlignment(format);
    Color3B color3B = color->isEqual(Screate("Gray")) ? Color3B::GRAY : color->isEqual(Screate("White")) ? Color3B::WHITE : Color3B::BLACK;
    delegate->setColor(color3B);
    realDimensions = dimensions;
    delegate->setDimensions(realDimensions.width / this->getScale(), 0);
    //delegate->enableUpdate(true);
    this->adjustLabel();
}

LabelTTF::LabelTTF(Label* label) :
loadingValue("")
{
    name = label->getString();
    fitType = ResizeFont;
    fontFile = new CCString(label->getSystemFontName().c_str());
    alignment = TextHAlignment::CENTER;
    realDimensions = label->getDimensions();
    delegate = label;
    label->retain();
    fullFontFile = ScreateF("%s%d%s", label->getSystemFontName().c_str(), (int)label->getSystemFontSize(), isColorEqual(label->getColor() , Color3B::BLACK) ? "Black" : isColorEqual(label->getColor() , Color3B::WHITE) ? "White" : "Gray");
    fullFontFile->retain();
    CustomLabel* customLabel = dynamic_cast<CustomLabel*>(label);
    if(customLabel != NULL)
    {
        fitType = customLabel->getFitType();
    }
    if(fitType != NoResize)
    {
        label->setDimensions(0, 0);
    }
    delegate->setDimensions(realDimensions.width / this->getScale(), 0);
    //delegate->enableUpdate(true);
    this->adjustLabel();
}

LabelTTF::~LabelTTF()
{
#if VERBOSE_DEALLOC
    CCLOG("Dealloc label %s, font : %s", name.c_str(), fullFontFile->getCString());
#endif
    delegate->release();
    fontFile->release();
    fullFontFile->release();
}

void LabelTTF::adjustLabel()
{
    if(realDimensions.width != 0 && realDimensions.height != 0 && fitType != NoResize)
    {
        bool wasChanged = false;
        std::string original = delegate->getString();
        delegate->setString("l");
        float lineHeight = delegate->getContentSize().height;
        delegate->setString(original);

        float scaleX = this->getScaleX();
        float scaleY = this->getScaleY();
        Size size = delegate->getContentSize();
        
        //Add a 5% margin for fitInside comparison since the algorithm underneath is not exact ....
        bool fitInside = (size.height * scaleY <= realDimensions.height * 1.05 || (fitType == CutEnd && lineHeight >= size.height)) && size.width * scaleX <= realDimensions.width * 1.05;
        
        //Used by CutEnd to perform a binary search (optimization because Label::updateTexture is slow on Android)
        //If you run into performance issues, you should also cache the CutEnd results
        size_t end = utf8_len(original);
        long start = fitInside || fitType != CutEnd ? end : 0; //If it already fit, bypass the while
        
        while((fitType != CutEnd && !fitInside)
              || end - start > 1) //There is one character precision (it may cut one more character than necessary)
        {
            wasChanged = true;
            if(fitType == ResizeFont)
            {
                scaleX *= 0.9;
                this->setScaleX(scaleX);
                scaleY *= 0.9;
                this->setScaleY(scaleY);
                delegate->setDimensions(realDimensions.width / scaleX, 0);
                size = delegate->getContentSize();
                fitInside = size.height * scaleY <= realDimensions.height * 1.05 && size.width * scaleX <= realDimensions.width * 1.05;
            }
            else if(fitType == CutEnd)
            {
                std::string value = delegate->getString();
                long middle = start + ((end - start) / 2);
                value = utf8_substr(original, 0, middle);
                CCAssert(value.length() != 0, "Invalid UTF8 string");
                delegate->setString(value.c_str());
                size = delegate->getContentSize();
                //the 1.05 multiplier is there to avoid rounding issues
                fitInside = (size.height * scaleY <= realDimensions.height * 1.05 || lineHeight >= size.height) && size.width * scaleX <= realDimensions.width * 1.05;
                if(fitInside)
                    start = middle;
                else
                    end = middle;
            }
            else
            {
                CCLOG("Warning, unsupported fit type, won't cut");
                return;
            }
        }
        if(wasChanged && fitType == CutEnd)
        {
            //TODO : refactor to compute the size with 3 '.' instead of just replacing the 3 last characters
            std::string value = delegate->getString();
            value = utf8_substr(value, 0, utf8_len(value) - 3).append("...");
            delegate->setString(value.c_str());
        }
    }
}

void LabelTTF::update(float deltaTime)
{
    if(!loadingValue.empty())
    {
        this->setLabelValue(loadingValue.c_str());
        loadingValue = "";
    }
}

CCString* LabelTTF::getFullFontFile()
{
    return fullFontFile;
}

TextHAlignment LabelTTF::getAlignment()
{
    return delegate->getHorizontalAlignment();
}
NS_FENNEX_END
