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

#include "LazyLoader.h"
#include "GraphicLayer.h"

NS_FENNEX_BEGIN
// singleton stuff
static LazyLoader *s_SharedLoader = NULL;

LazyLoader* LazyLoader::sharedLoader(void)
{
    if (!s_SharedLoader)
    {
        s_SharedLoader = new LazyLoader();
        s_SharedLoader->init();
    }
    
    return s_SharedLoader;
}

LazyLoader::~LazyLoader()
{
    this->clear();
    s_SharedLoader = NULL;
}

void LazyLoader::checkAll()
{
    for(FenneX::Image* image : images)
    {
        this->checkState(image);
    }
    for(FenneX::LabelTTF* label : labels)
    {
        this->checkState(label);
    }
}

void LazyLoader::loadAll()
{
    for(FenneX::Image* image : images)
    {
        this->loadImage(image);
    }
    for(FenneX::LabelTTF* label : labels)
    {
        this->loadLabel(label);
    }
}

void LazyLoader::moveHappened(Vector<RawObject*> children)
{
    for(RawObject* child : children)
    {
        if(isKindOfClass(child, FenneX::Image))
        {
            this->checkState((FenneX::Image*)child);
        }
        else if(isKindOfClass(child, FenneX::LabelTTF))
        {
            this->checkState((FenneX::LabelTTF*)child);
        }
        else if(isKindOfClass(child, Panel))
        {
            this->moveHappened(((Panel*)child)->getChildren());
        }
    }
}

void LazyLoader::addDynamicLoad(FenneX::Image* image, std::string textureName, bool checkState)
{
    auto imagePos = std::find(images.begin(), images.end(), image);
    if(imagePos != images.end())
    {
        //If an image is re-loaded, its texture and size might have changed. Remove it and reload it as new
        auto index = std::distance(images.begin(), imagePos);
        images.eraseObject(image);
        textures.erase(textures.begin() + index);
        sizes.erase(index);
        loadedImages.eraseObject(image);
    }
    images.pushBack(image);
    textures.push_back(textureName);
    Size* size = new Size(SizeMult(image->getSize(), GraphicLayer::sharedLayer()->getRealScale(image)));
    size->autorelease();
    sizes.pushBack(size);
    if(checkState)
    {
        this->checkState(image);
    }
}

void LazyLoader::addDynamicLoad(FenneX::LabelTTF* label, std::string string, bool checkState)
{
    auto labelPos = std::find(labels.begin(), labels.end(), label);
    if(labelPos != labels.end())
    {
        //If an image is re-loaded, its texture and size might have changed. Remove it and reload it as new
        auto index = std::distance(labels.begin(), labelPos);
        labels.eraseObject(label);
        strings.erase(strings.begin() + index);
        labelSizes.erase(index);
        loadedLabels.eraseObject(label);
    }
    labels.pushBack(label);
    strings.push_back(string);
    Size* size = new Size(SizeMult(label->getSize(), GraphicLayer::sharedLayer()->getRealScale(label)));
    size->autorelease();
    labelSizes.pushBack(size);
    if(checkState)
    {
        this->checkState(label);
    }
}

void LazyLoader::addDynamicLoadFunc(FenneX::Image* image, std::string key, std::function<std::string(std::string)> getTextureName, bool checkState)
{
    auto imagePos = std::find(images.begin(), images.end(), image);
    if(imagePos != images.end())
    {
        //If an image is re-loaded, its texture and size might have changed. Remove it and reload it as new
        auto index = std::distance(images.begin(), imagePos);
        images.eraseObject(image);
        textures.erase(textures.begin() + index);
        sizes.erase(index);
        loadedImages.eraseObject(image);
    }
    images.pushBack(image);
    textures.push_back(key);
    texturesFuncs[key] = getTextureName;
    Size* size = new Size(SizeMult(image->getSize(), GraphicLayer::sharedLayer()->getRealScale(image)));
    size->autorelease();
    sizes.pushBack(size);
    if(checkState)
    {
        this->checkState(image);
    }
}

void LazyLoader::init()
{
    this->clear();
}

void LazyLoader::clear()
{
    images.clear();
    textures.clear();
    sizes.clear();
    loadedImages.clear();
    labels.clear();
    strings.clear();
    labelSizes.clear();
    loadedLabels.clear();
}

void LazyLoader::checkState(FenneX::Image* img)
{
    if(images.find(img) != images.end()
       && loadedImages.find(img) == loadedImages.end()
       && GraphicLayer::sharedLayer()->isOnScreen(img, *sizes.at(images.getIndex(img))))
    {
        this->loadImage(img);
    }
}

void LazyLoader::loadImage(FenneX::Image* img)
{
    if(images.find(img) != images.end()
       && loadedImages.find(img) == loadedImages.end())
    {
        std::string texture = textures.at(images.getIndex(img));
        if(texturesFuncs.find(texture) != texturesFuncs.end())
        {
            std::string key = texture;
            texture = texturesFuncs[key](key);
            textures.at(images.getIndex(img)) = texture;
            texturesFuncs.erase(texturesFuncs.find(key));
        }
        if(texture != "")
        {
            img->replaceTexture(texture, true, true, true);
        }
        loadedImages.pushBack(img);
    }
}

void LazyLoader::checkState(FenneX::LabelTTF* label)
{
    if(labels.find(label) != labels.end()
       && loadedLabels.find(label) == loadedLabels.end()
       && GraphicLayer::sharedLayer()->isOnScreen(label, *labelSizes.at(labels.getIndex(label))))
    {
        this->loadLabel(label);
    }
}

void LazyLoader::loadLabel(FenneX::LabelTTF* label)
{
    if(labels.find(label) != labels.end()
       && loadedLabels.find(label) == loadedLabels.end())
    {
        label->setLabelValue(strings.at(labels.getIndex(label)), false);
        loadedLabels.pushBack(label);
    }
}
NS_FENNEX_END
