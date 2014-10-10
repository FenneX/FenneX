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

#ifndef __FenneX__Image__
#define __FenneX__Image__

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "RawObject.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
class Image : public RawObject
{
    CC_SYNTHESIZE_STRING_READONLY(imageFile, ImageFile);
public:
    
    CCRect getBoundingBox();
    virtual CCNode* getNode();
    
    Image();
    Image(const char* filename, CCPoint location);
    Image(const char* filename, CCPoint location, int capacity);
    Image(CCSprite* node);
    //TODO : add init from pixels data
    ~Image();
    
    //TODO : add more detailed animation methods
    //The first method is there to easily run a full animation. The 2 other to do custom things like delay/repeat/show/hide etc ...
    void runFullAnimation(float delay, bool invert = false); //delay is the delay between frames
    CCAnimate* getFullAnimation(float delay, bool invert = false);
    CCNode* getAnimationTarget();
    
    virtual void update(float deltaTime);
    
    void loadAnimation(const char* filename, int capacity);
    
    /*replace the Image texture using a new file
     filename : the new image to be loaded (without extension)
     keepExactSize : will fit the new image inside the old one, by changing the Image scale
     async : will defer the actual replace to next frame, and the texture will be loaded in async using CCTextureCache
     keepRatio : will crop the new image to have the same ratio as the previous one. Use keepExactSize to also have the same exact size
     */
    void replaceTexture(const char* filename, bool keepExactSize = false, bool async = false, bool keepRatio = false);
    void textureLoaded(Texture2D* tex);
    bool isAnimation();
    bool collision(CCPoint point); //Overload for spritesheet, which behaves differently
protected:
    //the actual CCSprite which will perform cocos2d actions
    CCSprite* delegate;
    
    //the animation action
    CCAction* runningAnimation;
    
    //NULL if the object is not an animation
    CCSpriteBatchNode* spriteSheet;
    
    //NULL if the object is not an animation, it is equal to animated sprites filenames in the plist
    CCArray* spritesName;
    
    std::string loadingImageFile;
    bool loadingKeepExactSize;
    bool loadingKeepRatio;
    bool isLoadingTexture;
};
NS_FENNEX_END

#endif /* defined(__FenneX__Image__) */
