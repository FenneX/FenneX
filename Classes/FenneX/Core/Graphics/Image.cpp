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

#include "Image.h"
#include "Shorteners.h"
#include "AppMacros.h"
#include "StringUtility.h"
#include <sstream>
#include <iomanip>

NS_FENNEX_BEGIN
Rect Image::getBoundingBox()
{
    return Rect(this->getNode()->getPositionX(), this->getNode()->getPositionY(), this->getNode()->getContentSize().width, this->getNode()->getContentSize().height);
}

Node* Image::getNode()
{
    if(spriteSheet != nullptr)
    {
        return spriteSheet;
    }
    CCAssert(delegate != nullptr, "Image getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}

Image::Image():
spriteSheet(nullptr),
delegate(nullptr),
runningAnimation(nullptr),
file(""),
loadingFile(""),
isLoadingTexture(false)
{
    
}
Image::Image(std::string filename, Vec2 location):
spriteSheet(nullptr),
runningAnimation(nullptr),
file(filename),
loadingFile(""),
isLoadingTexture(false)
{
    name = filename;
    if(stringEndsWith(file, ".png") || stringEndsWith(file, ".jpg") || stringEndsWith(file, ".jpeg"))
    {
        delegate = Sprite::create(file);
    }
    else
    { //Legacy compatibility: detect file extension
        delegate = Sprite::create(file.append(".png"));
        if(delegate == nullptr)
        {
            file.erase(file.length() - 4, 4);
            delegate = Sprite::create(file.append(".jpg"));
        }
        if(delegate == nullptr)
        {
            file.erase(file.length() - 4, 4);
            delegate = Sprite::create(file.append(".jpeg"));
        }
        if(delegate == nullptr)
        {
            file.erase(file.length() - 5, 5);
        }
    }
    if(delegate == nullptr)
    {
        log("Problem with asset : %s, the application will crash", filename.c_str());
    }
    delegate->retain();
    this->setPosition(location);
}
Image::Image(std::string filename, Vec2 location, int capacity):
file(filename),
loadingFile(""),
isLoadingTexture(false)
{
    name = filename;
    if(!stringEndsWith(file, ".png"))
    { //Legacy compatibility
        file.append(".png");
    }
    spriteSheet = SpriteBatchNode::create(file, capacity);
    std::string plistFile = file;
    plistFile.erase(plistFile.length() - 4, 4).append(".plist");
    spriteSheet->retain();
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
    
    spritesName.reserve(capacity);
    std::string fileNoExtension = file;
    fileNoExtension.erase(fileNoExtension.length() - 4, 4);
    for(int i = 1; i <= capacity; i++)
    {
        std::ostringstream spriteFileName;
        spriteFileName << fileNoExtension << '_' << std::setw(2) << std::setfill('0') << i << ".png";
        spritesName.push_back(spriteFileName.str());
    }
    delegate = Sprite::create();
    delegate->retain();
    SpriteFrame* firstFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spritesName.at(0));
    delegate->setSpriteFrame(firstFrame);
    this->setPosition(location);
    spriteSheet->addChild(delegate);
    runningAnimation = nullptr;
}

Image::Image(Sprite* node):
spritesName(0),
spriteSheet(nullptr),
runningAnimation(nullptr),
loadingFile(""),
isLoadingTexture(false)
{
    file = Director::getInstance()->getTextureCache()->getKeyForTexture(node->getTexture());
    long slashPos = file.rfind('/');
    if(slashPos != std::string::npos)
    {
        file = file.substr(slashPos+1);
    }
    this->setName(file.c_str());
    delegate = node;
    delegate->retain();
}

Image::~Image()
{
    if(spriteSheet != nullptr)
    {
        spriteSheet->release();
        spritesName.clear();
    }
    delegate->release();
#if VERBOSE_DEALLOC
    log("Dealloc image %s", name.c_str());
#endif
}

void Image::runFullAnimation(float delay, bool invert)
{
    CCAssert(spriteSheet != nullptr, "Image runFullAnimation called on an object without spritesheet");
    Vector<AnimationFrame*> animationFrames;
    for(int i = 0; i < spritesName.size(); i++)
    {
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spritesName.at(invert ? spritesName.size() - i - 1 : i));
        
        ValueMap infos;
        infos["Name"] = Value(getName());
        infos["Index"] = Value(i);
        AnimationFrame* frame = AnimationFrame::create(spriteFrame, 1, infos);
        animationFrames.pushBack(frame);
    }
    Action* action = RepeatForever::create(Animate::create(Animation::create(animationFrames, delay)));
    if(runningAnimation != action)
    {
        if(runningAnimation != nullptr)
        {
            delegate->stopAction(runningAnimation);
        }
        delegate->runAction(action);
        runningAnimation = action;
    }
}

Animate* Image::getFullAnimation(float delay, bool invert)
{
    CCAssert(spriteSheet != nullptr, "Image getFullAnimation called on an object without spritesheet");
    Vector<AnimationFrame*> animationFrames;
    for(int i = 0; i < spritesName.size(); i++)
    {
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spritesName.at(invert ? spritesName.size() - i - 1 : i));
        
        ValueMap infos;
        infos["Name"] = Value(getName());
        infos["Index"] = Value(i);
        AnimationFrame* frame = AnimationFrame::create(spriteFrame, 1, infos);
        animationFrames.pushBack(frame);
    }
    return Animate::create(Animation::create(animationFrames, delay));
}

Node* Image::getAnimationTarget()
{
    CCAssert(spriteSheet != nullptr, "Image getAnimationTarget called on an object without spritesheet");
    return delegate;
}

void Image::update(float deltaTime)
{
    if(!isLoadingTexture && !loadingFile.empty())
    {
        isLoadingTexture = true;
        Director::getInstance()->getTextureCache()->addImageAsync(loadingFile, CC_CALLBACK_1(Image::textureLoaded, this));
    }
}

void Image::loadAnimation(const char* filename, int capacity, bool useLastFrame)
{
    file = filename;
    spriteSheet = SpriteBatchNode::create(file, capacity);
    spriteSheet->retain();
    std::string plistFile = file;
    plistFile.erase(plistFile.length() - 4, 4).append(".plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
    
    spritesName.reserve(capacity);
    std::string fileNoExtension = file;
    fileNoExtension.erase(fileNoExtension.length() - 4, 4);
    for(int i = 1; i <= capacity; i++)
    {
        std::ostringstream spriteFileName;
        spriteFileName << fileNoExtension << '_' << std::setw(2) << std::setfill('0') << i << ".png";
        spritesName.push_back(spriteFileName.str());
    }
    SpriteFrame* firstFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spritesName.at(!useLastFrame ? 0 : spritesName.size() - 1));
    Node* parent = delegate->getParent();
    //If this is a previous animation, stop it first
    if(isKindOfClass(parent, SpriteBatchNode))
    {
        Node* realParent = parent->getParent();
        parent->removeChild(delegate);
        realParent->removeChild(parent, true);
        realParent->addChild(delegate);
        parent = realParent;
    }
    parent->addChild(spriteSheet);
    parent->removeChild(delegate, false);
    delegate->setSpriteFrame(firstFrame);
    spriteSheet->addChild(delegate);
    spriteSheet->setContentSize(firstFrame->getOriginalSize());
    runningAnimation = nullptr;
}

void Image::replaceTexture(std::string filename, bool keepExactSize, bool async, bool keepRatio)
{
    if(async)
    {
        //Note : async will be done on next update to avoid the performance drop because of the overhead of addImageAsync,
        //and in case the image already exists (which loads the image synchronously)
        loadingFile = filename;
        loadingKeepExactSize = keepExactSize;
        loadingKeepRatio = keepRatio;
    }
    else
    {
        std::string originalImageFile = file;
        file = filename;
        Sprite* sprite = (Sprite*)delegate;
        Size initialSize = Size(sprite->getContentSize().width * sprite->getScaleX(), sprite->getContentSize().height * sprite->getScaleY());
        Texture2D* newTexture = Director::getInstance()->getTextureCache()->addImage(file);
        if(newTexture == nullptr)
        {
#if VERBOSE_WARNING
            log("Warning : Problem with asset : %s, texture not replaced", filename.c_str());
#endif
            file = originalImageFile;
            return;
        }
        
        //If there is a spriteSheet, switch back normal delegate
        if(spriteSheet != nullptr)
        {
            Node* parent = spriteSheet->getParent();
            spriteSheet->removeChild(delegate, true);
            parent->removeChild(spriteSheet, true);
            parent->addChild(delegate);
            spriteSheet->release();
            spriteSheet = nullptr;
        }
        sprite->setTexture(newTexture);
        Rect textureRect = Rect(0, 0, newTexture->getContentSize().width, newTexture->getContentSize().height);
        //Change the textureRect to crop it if necessary
        if(keepRatio && initialSize.width / initialSize.height != textureRect.size.width / textureRect.size.height)
        {
            //initial is wider, crop top and bottom
            if(initialSize.width / initialSize.height > textureRect.size.width / textureRect.size.height)
            {
                float excessHeight = textureRect.size.height - initialSize.height / initialSize.width * textureRect.size.width;
                textureRect.origin.y = excessHeight/2;
                textureRect.size.height -= excessHeight;
            }
            //initial is taller, crop the sides
            else
            {
                float excessWidth = textureRect.size.width - initialSize.width / initialSize.height * textureRect.size.height;
                textureRect.origin.x = excessWidth/2;
                textureRect.size.width -= excessWidth;
            }
        }
        sprite->setTextureRect(textureRect);
        if(keepExactSize)
        {
            sprite->setScale(MIN(initialSize.width / sprite->getContentSize().width,
                                 initialSize.height / sprite->getContentSize().height));
        }
    }
}

void Image::textureLoaded(Texture2D* tex)
{
    if(!loadingFile.empty())
    {
        this->replaceTexture(loadingFile, loadingKeepExactSize, false, loadingKeepRatio);
        loadingFile = "";
        isLoadingTexture = false;
    }
}

bool Image::isAnimation()
{
    return spriteSheet != nullptr;
}

bool Image::collision(Vec2 point)
{
    if(spriteSheet != nullptr)
    {
        point.x = (point.x - delegate->getPosition().x + delegate->getAnchorPoint().x * delegate->getContentSize().width) / delegate->getScaleX();
        point.y = (point.y - delegate->getPosition().y + delegate->getAnchorPoint().y * delegate->getContentSize().height) / delegate->getScaleY();
    }
    return RawObject::collision(point);
}


cocos2d::Image::Format detectFormat(std::string& file)
{
    //Try to detect extension
    if(stringEndsWith(file, ".png")) return cocos2d::Image::Format::PNG;
    if(stringEndsWith(file, ".jpg")) return cocos2d::Image::Format::JPG;
    if(stringEndsWith(file, ".jpeg")) return cocos2d::Image::Format::JPG;
    //Legacy compatibility, detect file format
    Texture2D* newTexture = Director::getInstance()->getTextureCache()->addImage(file.append(".png").c_str());
    if(newTexture != nullptr) return cocos2d::Image::Format::PNG;
    file.erase(file.length() - 4, 4);
    newTexture = Director::getInstance()->getTextureCache()->addImage(file.append(".jpg").c_str());
    if(newTexture == nullptr)
    {
        newTexture = Director::getInstance()->getTextureCache()->addImage(file.append(".jpeg").c_str());
        file.erase(file.length() - 5, 5);
    }
    return newTexture == nullptr ? cocos2d::Image::Format::UNKNOWN : cocos2d::Image::Format::JPG;
}

bool Image::generateScaledImage(std::string fileToScale, std::string fileToSave, float scale)
{
    CCAssert(scale > 0, "Scale must be > 0 for generateScaledImage");
    cocos2d::Image::Format format = detectFormat(fileToScale);
    Texture2D* newTexture = Director::getInstance()->getTextureCache()->addImage(fileToScale);
    if(format == cocos2d::Image::Format::UNKNOWN || newTexture == nullptr)
    {
#if VERBOSE_WARNING
        log("Warning : Problem with asset : %s, texture not replaced", fileToScale.c_str());
#endif
        return false;
    }
    //Make it async to avoid crash when already rendering
    DelayedDispatcher::funcAfterDelay([fileToScale, fileToSave, scale, newTexture, format](EventCustom* event) {
        Sprite* image = Sprite::createWithTexture(newTexture);
        Size thumbnailSize = image->getContentSize() * scale;
        RenderTexture* thumbnail = RenderTexture::create(thumbnailSize.width, thumbnailSize.height);
        image->setScale(scale);
        image->setPosition(Vec2(0, 0));
        image->setAnchorPoint(Vec2(0, 0));
        thumbnail->beginWithClear(0, 0, 0, 0);
        image->visit();
        thumbnail->end();
        std::string fileSaveName = fileToSave;
        if(!stringEndsWith(fileToSave, ".png") && !stringEndsWith(fileToSave, ".jpg"))
        {
            fileSaveName += (format == cocos2d::Image::Format::PNG ? ".png" : ".jpg");
        }
        thumbnail->saveToFile(fileSaveName,
                              format,
                              true,
                              [fileToScale, fileSaveName] (RenderTexture* texture, const std::string& filename){
                                  //DO NOT USE FILENAME. It is corrupted on iOS. Use lambda capture instead.
                                  Director::getInstance()->getTextureCache()->removeTextureForKey(fileSaveName);
                                  Value toSend = Value(ValueMap({{"Original", Value(fileToScale)}, {"Name", Value(fileSaveName)}}));
                                  Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ImageScaled", &toSend);
                              });
    }, Value(), 0.01);
    return true;
}
NS_FENNEX_END
