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

NS_FENNEX_BEGIN
Rect Image::getBoundingBox()
{
    return Rect(this->getNode()->getPositionX(), this->getNode()->getPositionY(), this->getNode()->getContentSize().width, this->getNode()->getContentSize().height);
}

Node* Image::getNode()
{
    if(spriteSheet != NULL)
    {
        return spriteSheet;
    }
    CCAssert(delegate != NULL, "Image getNode is called upon a non-initialized object (or perhaps image/sheet load failed)");
    return delegate;
}

Image::Image():
spritesName(NULL),
spriteSheet(NULL),
delegate(NULL),
runningAnimation(NULL),
imageFile(""),
loadingImageFile(""),
isLoadingTexture(false)
{
    
}
Image::Image(std::string filename, Vec2 location):
spritesName(NULL),
spriteSheet(NULL),
runningAnimation(NULL),
imageFile(filename),
loadingImageFile(""),
isLoadingTexture(false)
{
    name = filename;
    delegate = Sprite::create(imageFile.append(".png").c_str());
    imageFile.erase(imageFile.length() - 4, 4);
    if(delegate == NULL)
    {
        delegate = Sprite::create(imageFile.append(".jpg").c_str());
        imageFile.erase(imageFile.length() - 4, 4);
    }
    if(delegate == NULL)
    {
        delegate = Sprite::create(imageFile.append(".jpeg").c_str());
        imageFile.erase(imageFile.length() - 5, 5);
    }
    if(delegate == NULL)
    {
        CCLOG("Problem with asset : %s, the application will crash", filename.c_str());
    }
    delegate->retain();
    this->setPosition(location);
}
Image::Image(std::string filename, Vec2 location, int capacity):
imageFile(filename),
loadingImageFile(""),
isLoadingTexture(false)
{
    name = filename;
    spriteSheet = SpriteBatchNode::create(imageFile.append(".png").c_str(), capacity);
    imageFile.erase(imageFile.length() - 4, 4);
    spriteSheet->retain();
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(imageFile.append(".plist").c_str());
    imageFile.erase(imageFile.length() - 6, 6);
    
    spritesName = CCArray::createWithCapacity(capacity);
    spritesName->retain();
    for(int i = 1; i <= capacity; i++)
    {
        /*char num[10];
         sprintf(num, "%04d", i);*/
        spritesName->addObject(ScreateF("%s_%02d.png", filename.c_str(), i));//imageFile.append(num).append(".png")));
    }
    delegate = Sprite::create();
    delegate->retain();
    SpriteFrame* firstFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(((CCString*)spritesName->objectAtIndex(0))->getCString());
    delegate->setDisplayFrame(firstFrame);
    this->setPosition(location);
    spriteSheet->addChild(delegate);
    runningAnimation = NULL;
}

Image::Image(Sprite* node):
spritesName(NULL),
spriteSheet(NULL),
runningAnimation(NULL),
loadingImageFile(""),
isLoadingTexture(false)
{
    imageFile = Director::getInstance()->getTextureCache()->getKeyForTexture(node->getTexture());
    size_t extensionPos = imageFile.rfind(".png");
    if(extensionPos != std::string::npos)
    {
        imageFile = imageFile.substr(0, extensionPos);
    }
    extensionPos = imageFile.rfind(".jpg");
    if(extensionPos != std::string::npos)
    {
        imageFile = imageFile.substr(0, extensionPos);
    }
    extensionPos = imageFile.rfind(".jpeg");
    if(extensionPos != std::string::npos)
    {
        imageFile = imageFile.substr(0, extensionPos);
    }
    size_t slashPos = imageFile.rfind('/');
    if(slashPos != std::string::npos)
    {
        imageFile = imageFile.substr(slashPos+1);
    }
    this->setName(imageFile.c_str());
    delegate = node;
    delegate->retain();
}

Image::~Image()
{
    if(spriteSheet != NULL)
    {
        spriteSheet->release();
        spritesName->release();
    }
    delegate->release();
#if VERBOSE_DEALLOC
    CCLOG("Dealloc image %s", name.c_str());
#endif
}

void Image::runFullAnimation(float delay, bool invert)
{
    CCAssert(spriteSheet != NULL, "Image runFullAnimation called on an object without spritesheet");
    Vector<AnimationFrame*> animationFrames;
    for(int i = 0; i < spritesName->count(); i++)
    {
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(((CCString*)spritesName->objectAtIndex(invert ? spritesName->count() - i - 1 : i))->getCString());
        
        ValueMap infos;
        infos["Name"] = Value(getName());
        infos["Index"] = Value(i);
        AnimationFrame* frame = AnimationFrame::create(spriteFrame, 1, infos);
        animationFrames.pushBack(frame);
    }
    Action* action = RepeatForever::create(Animate::create(Animation::create(animationFrames, delay)));
    if(runningAnimation != action)
    {
        if(runningAnimation != NULL)
        {
            delegate->stopAction(runningAnimation);
        }
        delegate->runAction(action);
        runningAnimation = action;
    }
}

Animate* Image::getFullAnimation(float delay, bool invert)
{
    CCAssert(spriteSheet != NULL, "Image getFullAnimation called on an object without spritesheet");
    Vector<AnimationFrame*> animationFrames;
    for(int i = 0; i < spritesName->count(); i++)
    {
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(((CCString*)spritesName->objectAtIndex(invert ? spritesName->count() - i - 1 : i))->getCString());
        
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
    CCAssert(spriteSheet != NULL, "Image getAnimationTarget called on an object without spritesheet");
    return delegate;
}

void Image::update(float deltaTime)
{
    if(!isLoadingTexture && !loadingImageFile.empty())
    {
        isLoadingTexture = true;
        std::string withEtension = loadingImageFile.c_str();//ensure a deep copy
        Director::getInstance()->getTextureCache()->addImageAsync(withEtension.append(".png").c_str(), CC_CALLBACK_1(Image::textureLoaded, this));
                                                                  //const std::function<void(Texture2D*)>& callback
                                                                  //this, callfuncO_selector(Image::textureLoaded));
        std::string withEtensionJpg = loadingImageFile.c_str();//ensure a deep copy
        Director::getInstance()->getTextureCache()->addImageAsync(withEtensionJpg.append(".jpg").c_str(), CC_CALLBACK_1(Image::textureLoaded, this));
        std::string withEtensionJpeg = loadingImageFile.c_str();//ensure a deep copy
        Director::getInstance()->getTextureCache()->addImageAsync(withEtensionJpeg.append(".jpeg").c_str(), CC_CALLBACK_1(Image::textureLoaded, this));
    }
}

void Image::loadAnimation(const char* filename, int capacity, bool useLastFrame)
{
    imageFile = filename;
    spriteSheet = SpriteBatchNode::create(imageFile.append(".png").c_str(), capacity);
    imageFile.erase(imageFile.length() - 4, 4);
    spriteSheet->retain();
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(imageFile.append(".plist").c_str());
    imageFile.erase(imageFile.length() - 6, 6);
    
    spritesName = CCArray::createWithCapacity(capacity);
    spritesName->retain();
    for(int i = 1; i <= capacity; i++)
    {
        /*char num[10];
         sprintf(num, "%04d", i);*/
        spritesName->addObject(ScreateF("%s_%02d.png", filename, i));//imageFile.append(num).append(".png")));
    }
    SpriteFrame* firstFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(((CCString*)spritesName->objectAtIndex(!useLastFrame ? 0 : spritesName->count() - 1))->getCString());
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
    delegate->setDisplayFrame(firstFrame);
    spriteSheet->addChild(delegate);
    spriteSheet->setContentSize(firstFrame->getOriginalSize());
    runningAnimation = NULL;
}

void Image::replaceTexture(std::string filename, bool keepExactSize, bool async, bool keepRatio)
{
    if(async)
    {
        //Note : async will be done on next update to avoid the performance drop because of the overhead of addImageAsync,
        //and in case the image already exists (which loads the image synchronously)
        loadingImageFile = filename;
        loadingKeepExactSize = keepExactSize;
        loadingKeepRatio = keepRatio;
    }
    else
    {
        std::string originalImageFile = imageFile;
        imageFile = filename;
        Sprite* sprite = (Sprite*)delegate;
        Size initialSize = Size(sprite->getContentSize().width * sprite->getScaleX(), sprite->getContentSize().height * sprite->getScaleY());
        Texture2D* newTexture = Director::getInstance()->getTextureCache()->addImage(imageFile.append(".png").c_str());
        imageFile.erase(imageFile.length() - 4, 4);
        if(newTexture == NULL)
        {
            newTexture = Director::getInstance()->getTextureCache()->addImage(imageFile.append(".jpg").c_str());
            imageFile.erase(imageFile.length() - 4, 4);
        }
        if(newTexture == NULL)
        {
            newTexture = Director::getInstance()->getTextureCache()->addImage(imageFile.append(".jpeg").c_str());
            imageFile.erase(imageFile.length() - 5, 5);
        }
        if(newTexture == NULL)
        {
#if VERBOSE_WARNING
            CCLOG("Warning : Problem with asset : %s, texture not replaced", filename.c_str());
#endif
            imageFile = originalImageFile;
            return;
        }
        
        //If there is a spriteSheet, switch back normal delegate
        if(spriteSheet != NULL)
        {
            Node* parent = spriteSheet->getParent();
            spriteSheet->removeChild(delegate, true);
            parent->removeChild(spriteSheet, true);
            parent->addChild(delegate);
            spriteSheet->release();
            spriteSheet = NULL;
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
    if(!loadingImageFile.empty())
    {
        this->replaceTexture(loadingImageFile, loadingKeepExactSize, false, loadingKeepRatio);
        loadingImageFile = "";
        isLoadingTexture = false;
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : Problem with asset : %s loaded asynchronously, texture not replaced", loadingImageFile.c_str());
    }
#endif
}

bool Image::isAnimation()
{
    return spriteSheet != NULL;
}

bool Image::collision(Vec2 point)
{
    if(spriteSheet != NULL)
    {
        point.x = (point.x - delegate->getPosition().x + delegate->getAnchorPoint().x * delegate->getContentSize().width) / delegate->getScale();
        point.y = (point.y - delegate->getPosition().y + delegate->getAnchorPoint().y * delegate->getContentSize().height) / delegate->getScale();
    }
    return RawObject::collision(point);
}
NS_FENNEX_END
