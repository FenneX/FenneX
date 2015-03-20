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

#define COCOS2D_DEBUG 1

#include "FenneXCCBLoader.h"
#include "GraphicLayer.h"
#include "Shorteners.h"
#include "AppMacros.h"
#include "Localization.h"

#include "CustomScaleSprite.h"
#include "CustomSprite.h"
#include "CustomNode.h"
#include "CustomInput.h"
#include "CustomLabel.h"

using namespace cocosbuilder;

NS_FENNEX_BEGIN
static float _loadingScale = 1;
static bool isPhone = false;

//Don't retain the CCBAnimationManager, because it's troublesome to release them at the right time. Soft references is enough.
static std::vector<CCBAnimationManager*> animManagers;

void resizeChildren(Node* parentNode, Node* resizeNode, float usedScale, int depth)
{
    Vector<Node*> nodeChildren = resizeNode->getChildren();
    for(auto node : nodeChildren)
    {
        node->setPosition(node->getPosition() * usedScale);
        if(isKindOfClass(node, Label))
        {
            Label* label = (Label*)node;
            label->setSystemFontSize(label->getSystemFontSize() * usedScale);
            /*TTFConfig newConfig = label->getTTFConfig();
            newConfig.fontSize *= usedScale;
            label->setTTFConfig(newConfig);*/
            label->setDimensions(label->getDimensions().width * usedScale, label->getDimensions().height * usedScale);
#if VERBOSE_LOAD_CCB
            CCLOG("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth : %d", label->getSystemFontName().c_str(), label->getSystemFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height, depth);
#endif
        }
        else if(isKindOfClass(node, CustomInput))
        {
            CustomInput* input = (CustomInput*)node;
            input->setFontSize((float)input->getFontSize() * usedScale);
            input->setPreferredSize(Size(input->getPreferredSize().width * usedScale, input->getPreferredSize().height * usedScale));
#if VERBOSE_LOAD_CCB
            CCLOG("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth : %d", input->getFontSize(), parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height, depth);
#endif
        }
        else if(isKindOfClass(node, Sprite))
        {
            
        }
        else if(isKindOfClass(node, ui::Scale9Sprite))
        {
            ui::Scale9Sprite* sprite = (ui::Scale9Sprite*)node;
            sprite->setContentSize(SizeMult(node->getContentSize(), usedScale));
            sprite->setInsetBottom(sprite->getInsetBottom() * usedScale);
            sprite->setInsetLeft(sprite->getInsetRight() * usedScale);
            sprite->setInsetRight(sprite->getInsetRight() * usedScale);
            sprite->setInsetTop(sprite->getInsetTop() * usedScale);
        }
        else  //Panel
        {
            node->setContentSize(SizeMult(node->getContentSize(), usedScale));
            if(!node->getChildren().empty())
            {
                resizeChildren(parentNode, node, usedScale, depth+1);
            }
        }
    }
}

Panel* loadCCBFromFileToFenneX(const char* file, const char* inPanel, int zIndex)
{
    if(inPanel == NULL)
    {
        animManagers.clear();
    }
    NodeLoaderLibrary * nodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    nodeLoaderLibrary->registerNodeLoader("CustomSprite", CustomSpriteLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomScaleSprite", CustomScaleSpriteLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomNode", CustomNodeLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomInput", CustomInputLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomLabel", CustomLabelLoader::loader());
    CCBReader *ccbReader = new CCBReader(nodeLoaderLibrary);
    
    //IF this one fail, it needs to be silent
    bool shouldNotify = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    
#if VERBOSE_PERFORMANCE_TIME
    timeval startTime;
    gettimeofday(&startTime, NULL);
#endif
    std::string filePath = ScreateF("%s%s.ccbi", file, isPhone ? "-phone" : "")->getCString();
    CCLOG("Filepath : %s", filePath.c_str());
    FileUtils::getInstance()->setPopupNotify(shouldNotify);
    Node* myNode = NULL;
    if(FileUtils::getInstance()->isFileExist(filePath))
    {
        CCLOG("File exist");
        myNode = ccbReader->readNodeGraphFromFile(filePath.c_str());
    }
    else if(isPhone)
    {
        myNode = ccbReader->readNodeGraphFromFile(ScreateF("%s.ccbi", file)->getCString());
    }
    
#if VERBOSE_PERFORMANCE_TIME
    timeval middleTime;
    gettimeofday(&middleTime, NULL);
    CCLOG("CCBReader loaded file %s in %f ms", file, getTimeDifferenceMS(startTime, middleTime));
#endif
    
    //Despite cocosbuilder saying so, Label and Node (for Panel) aren't resized properly, so there it is
    /*Size frameSize = Director::getInstance()->getWinSize();
     float scaleX = (float)frameSize.width / designResolutionSize.width;
     float scaleY = (float)frameSize.height / designResolutionSize.height;
     float scale = MAX(scaleX, scaleY);
     float minScale = MIN(scaleX, scaleY);
     CCLOG("min : %f, max : %f, loading : %f", minScale, scale, _loadingScale);*/
    float usedScale = _loadingScale;
    
    //this code is dubious at best .... refactor it later
#if VERBOSE_LOAD_CCB
    CCLOG("ccb file %s loaded, doing rescaling ...", file);
#endif
    for(auto node : myNode->getChildren())
    {
        //Note : Panels are never tested against isKindOfClass because :
        //against Node, all nodes return true
        //against CustomNode, a tagging is required on cocosBuilder
        
        //Depth 0 of nesting
        if(isKindOfClass(node, Label))
        {
            Label* label = (Label*)node;
#if VERBOSE_LOAD_CCB
            CCLOG("label font : %s", label->getSystemFontName().c_str());
#endif
            label->setSystemFontSize(label->getSystemFontSize() * usedScale);
            /*TTFConfig newConfig = label->getTTFConfig();
            newConfig.fontSize *= usedScale;
            label->setTTFConfig(newConfig);*/
        }
        else if(isKindOfClass(node, CustomInput))
        {
            
        }
        else if(isKindOfClass(node, Sprite) || isKindOfClass(node, ui::Scale9Sprite))
        {
            
        }
        else if(!node->getChildren().empty())//Panel
        {
            node->setScaleX(node->getScaleX()*usedScale);
            node->setScaleY(node->getScaleY()*usedScale);
            Node* parentNode = node;
            
            for(auto node : node->getChildren())
            {
                //Depth 1 is a special case too, because nodes don't have to be moved and the scale is not calculated the same way
                if(isKindOfClass(node, Label))
                {
                    Label* label = (Label*)node;
                    label->setScale(label->getScale() / parentNode->getScale());
                    label->setSystemFontSize(label->getSystemFontSize() * parentNode->getScale());
                    /*TTFConfig newConfig = label->getTTFConfig();
                    newConfig.fontSize *= parentNode->getScale();
                    label->setTTFConfig(newConfig);*/
#if VERBOSE_LOAD_CCB
                    CCLOG("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth 1", label->getSystemFontName().c_str(), label->getSystemFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height);
#endif
                }
                else if(isKindOfClass(node, CustomInput))
                {
                    //input->setFontSize((float)input->getFontSize() / usedScale);
                    //input->setPreferredSize(Size(input->getPreferredSize().width / usedScale, input->getPreferredSize().height / usedScale));
                    //input->setFontSize((float)input->getFontSize() * parentNode->getScale());
#if VERBOSE_LOAD_CCB
                    CustomInput* input = (CustomInput*)node;
                    CCLOG("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth 1", input->getFontSize() , parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height);
#endif
                }
                else if(isKindOfClass(node, Sprite))
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                }
                else if(isKindOfClass(node, ui::Scale9Sprite))
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                    node->setContentSize(SizeMult(node->getContentSize(), usedScale));
                }
                else if(!node->getChildren().empty())//Panel
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                    node->setContentSize(SizeMult(node->getContentSize(), usedScale));
                    //For depth 2 and more, the algorithm is the same
                    resizeChildren(parentNode, node, usedScale, 2);
                }
            }
        }
    }
    
    Panel* parent = NULL;
    if(inPanel != NULL)
    {
        myNode->setContentSize(Size(0, 0));
        parent = GraphicLayer::sharedLayer()->createPanelWithNode(inPanel, myNode, zIndex);
    }
    
    loadNodeToFenneX(myNode, parent);
    reorderZindex();
    linkInputLabels();
    
    animManagers.push_back(ccbReader->getAnimationManager());
    
    ccbReader->release();
    
#if VERBOSE_PERFORMANCE_TIME
    timeval endTime;
    gettimeofday(&endTime, NULL);
    CCLOG("Node %s loaded to FenneX in %f ms, total with load file : %f ms", file, getTimeDifferenceMS(middleTime, endTime), getTimeDifferenceMS(startTime, endTime));
#endif
    return parent;
}

void loadNodeToFenneX(Node* baseNode, Panel* parent)
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    if(parent == NULL)
    {
        layer->useBaseLayer((Layer*)baseNode);
#if VERBOSE_LOAD_CCB
        CCLOG("replaced base layer by CCB node : position : %f, %f, scale : %f", baseNode->getPosition().x, baseNode->getPosition().y, baseNode->getScale());
#endif
    }
    
    //Use an index because InputLabel modify the array, so you need to rewind a bit at some point
    for(int i = 0; i < baseNode->getChildren().size(); i++)
    {
        Node* node = baseNode->getChildren().at(i);
#if VERBOSE_LOAD_CCB
        CCLOG("doing child %d from parent %s ...", i, parent != NULL ? parent->getName() != NULL ? parent->getName() : "Panel" : "base layer");
#endif
        RawObject* result = NULL;
        if(isKindOfClass(node, Label))
        {
            Label* label = (Label*)node;
#if VERBOSE_LOAD_CCB
            CCLOG("label, font : %s", label->getSystemFontName().c_str());
#endif
            CCString* translationKey = isKindOfClass(label, CustomBaseNode) ? (CCString*)dynamic_cast<CustomBaseNode*>(label)->getParameters()->objectForKey("translationKey") : NULL;
            const std::string translated = Localization::getLocalizedString(translationKey != NULL ? translationKey->getCString() : label->getString());
            if(translationKey == NULL || translationKey->compare(translated.c_str()) != 0)
            { //Don't replace the string if it's the same, as it may only be a key, not a real label
                label->setString(translated);
            }
            result = layer->createLabelTTFromLabel(label, parent);
        }
        else if(isKindOfClass(node, Sprite))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("image");
#endif
            Sprite* sprite = (Sprite*)node;
            result = layer->createImageFromSprite(sprite, parent);
        }
        else if(isKindOfClass(node, CustomInput))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("input label");
#endif
            ui::Scale9Sprite* sprite = (ui::Scale9Sprite*)node;
            result = layer->createInputLabelFromScale9Sprite(sprite, parent);
            i--;
        }
        else if(isKindOfClass(node, ui::Scale9Sprite))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("scale sprite");
#endif
            ui::Scale9Sprite* sprite = (ui::Scale9Sprite*)node;
            result = layer->createCustomObjectFromNode(sprite, parent);
        }
        else if(!isKindOfClass(node, ui::EditBox))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("panel");
#endif
            result = layer->createPanelFromNode(node, parent);
        }
#if VERBOSE_LOAD_CCB
        if(result != NULL)
        {
            CCLOG("Child %d loaded : position : %f, %f, scale : %f", i, result->getPosition().x, result->getPosition().y, result->getScale());
        }
        else
        {
            CCLOG("Problem loading child %d !", i);
        }
#endif
    }
}

void reorderZindex()
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    CCArray* children = layer->getChildren();
    if(children != NULL)
    {
        for(int i = 0 ; i < children->count(); i++)
        {
            RawObject* child = (RawObject*)children->objectAtIndex(i);
            if(child->getZOrder() != 0)
            {
                layer->reorderChild(child, child->getZOrder());
            }
        }
    }
}

void linkInputLabels()
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    CCArray* children = layer->getChildren();
    if(children != NULL)
    {
        for(int i = 0 ; i < children->count(); i++)
        {
            RawObject* child = (RawObject*)children->objectAtIndex(i);
            //Force actualization of content size and fontSize after everything is loaded because the nodeToWorldTransform is only right after
            if(isKindOfClass(child, InputLabel))
            {
                InputLabel* input = (InputLabel*)child;
                child->getNode()->setContentSize(child->getNode()->getContentSize());
                if(input->getOriginalInfos() != NULL)
                {
                    ((ui::EditBox*)child->getNode())->setFontSize(input->getOriginalInfos()->getFontSize());
                }
            }
            if(isKindOfClass(child, InputLabel) && child->getEventInfos()->objectForKey("LinkTo") != NULL && isKindOfClass(child->getEventInfos()->objectForKey("LinkTo"), CCString))
            {
                InputLabel* input = (InputLabel*)child;
                if(input->getLinkTo() == NULL)
                {
                    CCString* linkTo = (CCString*)child->getEventInfos()->objectForKey("LinkTo");
                    CCArray* matchs = layer->allObjectsWithName(linkTo);
                    for(long j = 0; j < matchs->count(); j++)
                    {
                        RawObject* match = (RawObject*)matchs->objectAtIndex(j);
                        if(isKindOfClass(match, LabelTTF))
                        {
                            input->setLinkTo((LabelTTF*)match);
                            j = matchs->count();
                        }
                    }
                }
            }
        }
    }
}

void CCBLoaderSetScale(float scale)
{
    _loadingScale = scale;
}

float CCBLoaderGetScale()
{
    return _loadingScale;
}

void CCBLoaderSetPhoneLayout(bool usePhone)
{
    isPhone = usePhone;
}

bool CCBLoaderIsPhoneLayout()
{
    return isPhone;
}

std::vector<CCBAnimationManager*>& getAnimationManagers()
{
    return animManagers;
}

NS_FENNEX_END
