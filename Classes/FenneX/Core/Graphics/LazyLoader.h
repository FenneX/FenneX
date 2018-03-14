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

#ifndef __FenneX__LazyLoader__
#define __FenneX__LazyLoader__

#include "Image.h"
#include "LabelTTF.h"

USING_NS_CC;


NS_FENNEX_BEGIN
class LazyLoader : public Ref
{
public:
    static LazyLoader* sharedLoader(void);
    ~LazyLoader();
    void moveHappened(Vector<RawObject*> children);
    void checkAll();
    //Force load of all lazy-loaded objects
    void loadAll();
    void addDynamicLoad(Image* image, std::string textureName, bool checkState = true);
    void addDynamicLoad(LabelTTF* label, std::string string, bool checkState = true);
    //Allow to defer textureName resolving to when it is really needed (in case getting textureName is time-consuming)
    void addDynamicLoadFunc(Image* image, std::string key, std::function<std::string(std::string)> getTextureName, bool checkState = true);
    void clear();
protected:
    void init();
    void checkState(Image* img);
    void loadImage(Image* img);
    void checkState(LabelTTF* label);
    void loadLabel(LabelTTF* label);
    
    //Images
    Vector<Image*> images; //The images which should load dynamically
    std::vector<std::string> textures; //The textures associated with those images
    Vector<cocos2d::Size*> sizes; //The sizes of images, kept for better performance
    Vector<Image*> loadedImages; //The loaded image to avoid reloading images
    std::map<std::string, std::function<std::string(std::string)>> texturesFuncs;
    
    //Labels
    Vector<LabelTTF*> labels; //The labels which should load dynamically
    std::vector<std::string> strings; //The string associated with those labels
    Vector<cocos2d::Size*> labelSizes; //The sizes of labels, kept for better performance
    Vector<LabelTTF*> loadedLabels; //The loaded labels to avoid reloading labels
};

NS_FENNEX_END

#endif /* defined(__FenneX__LazyLoader__) */
