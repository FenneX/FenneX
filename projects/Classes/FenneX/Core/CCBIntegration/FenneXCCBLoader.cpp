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

NS_FENNEX_BEGIN
static float _loadingScale = 1;
static bool isPhone = false;

static CCArray* animManagers = new CCArray();

void resizeChildren(CCNode* parentNode, CCNode* resizeNode, float usedScale, int depth)
{
    CCArray* nodeChildren = resizeNode->getChildren();
    for(int i = 0 ; i < nodeChildren->count(); i++)
    {
        CCNode* node = (CCNode*)nodeChildren->objectAtIndex(i);
        node->setPosition(ccpMult(node->getPosition(), usedScale));
        if(isKindOfClass(node, CCLabelTTF))
        {
            CCLabelTTF* label = (CCLabelTTF*)node;
            label->setFontSize(label->getFontSize()*usedScale);
            label->setDimensions(CCSizeMult(label->getDimensions(), usedScale));
#if VERBOSE_LOAD_CCB
            CCLOG("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth : %d", label->getFontName(), label->getFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height, depth);
#endif
        }
        else if(isKindOfClass(node, CustomInput))
        {
            CustomInput* input = (CustomInput*)node;
            input->setFontSize((float)input->getFontSize() * usedScale);
            input->setPreferredSize(CCSizeMake(input->getPreferredSize().width * usedScale, input->getPreferredSize().height * usedScale));
#if VERBOSE_LOAD_CCB
            CCLOG("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth : %d", input->getFontSize(), parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height, depth);
#endif
        }
        else if(isKindOfClass(node, CCSprite))
        {
            
        }
        else if(isKindOfClass(node, CCScale9Sprite))
        {
            CCScale9Sprite* sprite = (CCScale9Sprite*)node;
            sprite->setContentSize(CCSizeMult(node->getContentSize(), usedScale));
            sprite->setInsetBottom(sprite->getInsetBottom() * usedScale);
            sprite->setInsetLeft(sprite->getInsetRight() * usedScale);
            sprite->setInsetRight(sprite->getInsetRight() * usedScale);
            sprite->setInsetTop(sprite->getInsetTop() * usedScale);
        }
        else  //Panel
        {
            node->setContentSize(CCSizeMult(node->getContentSize(), usedScale));
            if(node->getChildren() != NULL)
                resizeChildren(parentNode, node, usedScale, depth+1);
        }
    }
}

Panel* loadCCBFromFileToFenneX(const char* file, const char* inPanel, int zIndex)
{
    if(inPanel == NULL)
    {
        animManagers->removeAllObjects();
    }
    CCNodeLoaderLibrary * ccNodeLoaderLibrary = cocos2d::extension::CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    ccNodeLoaderLibrary->registerCCNodeLoader("CustomSprite", CustomSpriteLoader::loader());
    ccNodeLoaderLibrary->registerCCNodeLoader("CustomScaleSprite", CustomScaleSpriteLoader::loader());
    ccNodeLoaderLibrary->registerCCNodeLoader("CustomNode", CustomNodeLoader::loader());
    ccNodeLoaderLibrary->registerCCNodeLoader("CustomInput", CustomInputLoader::loader());
    ccNodeLoaderLibrary->registerCCNodeLoader("CustomLabel", CustomLabelLoader::loader());
    CCBReader *ccbReader = new cocos2d::extension::CCBReader(ccNodeLoaderLibrary);
    
    //IF this one fail, it needs to be silent
    bool shouldNotify = CCFileUtils::sharedFileUtils()->isPopupNotify();
    CCFileUtils::sharedFileUtils()->setPopupNotify(false);
    
#if VERBOSE_PERFORMANCE_TIME
    cc_timeval startTime;
    CCTime::gettimeofdayCocos2d(&startTime, NULL);
#endif
    CCNode* myNode = ccbReader->readNodeGraphFromFile(ScreateF("%s%s.ccbi", file, isPhone ? "-phone" : "")->getCString());
    CCFileUtils::sharedFileUtils()->setPopupNotify(shouldNotify);
    
    if(myNode == NULL && isPhone)
    {
        myNode = ccbReader->readNodeGraphFromFile(ScreateF("%s.ccbi", file)->getCString());
    }
    
#if VERBOSE_PERFORMANCE_TIME
    cc_timeval middleTime;
    CCTime::gettimeofdayCocos2d(&middleTime, NULL);
    CCLog("CCBReader loaded file %s in %f ms", file, CCTime::timersubCocos2d(&startTime, &middleTime));
#endif
    
    //Despite cocosbuilder saying so, CCLabelTTF and CCNode (for Panel) aren't resized properly, so there it is
    /*CCSize frameSize = CCDirector::sharedDirector()->getWinSize();
     float scaleX = (float)frameSize.width / designResolutionSize.width;
     float scaleY = (float)frameSize.height / designResolutionSize.height;
     float scale = MAX(scaleX, scaleY);
     float minScale = MIN(scaleX, scaleY);
     CCLOG("min : %f, max : %f, loading : %f", minScale, scale, _loadingScale);*/
    float usedScale = _loadingScale;
    
    CCArray* children = myNode->getChildren();
    //this code is dubious at best .... refactor it later
#if VERBOSE_LOAD_CCB
    CCLOG("ccb file %s loaded, doing rescaling ...", file);
#endif
    for(int i = 0 ; children != NULL && i < children->count(); i++)
    {
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        //Note : Panels are never tested against isKindOfClass because :
        //against CCNode, all nodes return true
        //against CustomNode, a tagging is required on cocosBuilder
        
        //Depth 0 of nesting
        if(isKindOfClass(node, CCLabelTTF))
        {
            CCLabelTTF* label = (CCLabelTTF*)node;
#if VERBOSE_LOAD_CCB
            CCLOG("label font : %s", label->getFontName());
#endif
            label->setFontSize(label->getFontSize()*usedScale);
        }
        else if(isKindOfClass(node, CustomInput))
        {
            
        }
        else if(isKindOfClass(node, CCSprite) || isKindOfClass(node, CCScale9Sprite))
        {
            
        }
        else if(node->getChildren() != NULL)//Panel
        {
            node->setScaleX(node->getScaleX()*usedScale);
            node->setScaleY(node->getScaleY()*usedScale);
            CCNode* parentNode = node;
            
            CCArray* nodeChildren = node->getChildren();
            for(int i = 0 ; i < nodeChildren->count(); i++)
            {
                //Depth 1 is a special case too, because nodes don't have to be moved and the scale is not calculated the same way
                CCNode* node = (CCNode*)nodeChildren->objectAtIndex(i);
                if(isKindOfClass(node, CCLabelTTF))
                {
                    CCLabelTTF* label = (CCLabelTTF*)node;
                    label->setScale(label->getScale() / parentNode->getScale());
                    label->setFontSize(label->getFontSize() * parentNode->getScale());
#if VERBOSE_LOAD_CCB
                    CCLOG("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth 1", label->getFontName(), label->getFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height);
#endif
                }
                else if(isKindOfClass(node, CustomInput))
                {
                    //input->setFontSize((float)input->getFontSize() / usedScale);
                    //input->setPreferredSize(CCSizeMake(input->getPreferredSize().width / usedScale, input->getPreferredSize().height / usedScale));
                    //input->setFontSize((float)input->getFontSize() * parentNode->getScale());
#if VERBOSE_LOAD_CCB
                    CustomInput* input = (CustomInput*)node;
                    CCLOG("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth 1", input->getFontSize(), parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height);
#endif
                }
                else if(isKindOfClass(node, CCSprite))
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                }
                else if(isKindOfClass(node, CCScale9Sprite))
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                    node->setContentSize(CCSizeMult(node->getContentSize(), usedScale));
                }
                else if(node->getChildren() != NULL)//Panel
                {
                    node->setScaleX(node->getScaleX() / usedScale);
                    node->setScaleY(node->getScaleY() / usedScale);
                    node->setContentSize(CCSizeMult(node->getContentSize(), usedScale));
                    //For depth 2 and more, the algorithm is the same
                    resizeChildren(parentNode, node, usedScale, 2);
                }
            }
        }
    }
    
    Panel* parent = NULL;
    if(inPanel != NULL)
    {
        myNode->setContentSize(CCSizeMake(0, 0));
        parent = GraphicLayer::sharedLayer()->createPanelWithNode(inPanel, myNode, zIndex);
    }
    
    loadNodeToFenneX(myNode, parent);
    reorderZindex();
    linkInputLabels();
    
    animManagers->addObject(ccbReader->getAnimationManager());
    
    ccbReader->release();
    
#if VERBOSE_PERFORMANCE_TIME
    cc_timeval endTime;
    CCTime::gettimeofdayCocos2d(&endTime, NULL);
    CCLog("Node %s loaded to FenneX in %f ms, total with load file : %f ms", file, CCTime::timersubCocos2d(&middleTime, &endTime), CCTime::timersubCocos2d(&startTime, &endTime));
#endif
    return parent;
}

void loadNodeToFenneX(CCNode* baseNode, Panel* parent)
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    if(parent == NULL)
    {
        layer->useBaseLayer((CCLayer*)baseNode);
#if VERBOSE_LOAD_CCB
        CCLOG("replaced base layer by CCB node : position : %f, %f, scale : %f", baseNode->getPosition().x, baseNode->getPosition().y, baseNode->getScale());
#endif
    }
    CCArray* children = baseNode->getChildren();
    if(children != NULL)
    {
        for(int i = 0 ; i < children->count(); i++)
        {
#if VERBOSE_LOAD_CCB
            CCLOG("doing child %d from parent %s ...", i, parent != NULL ? parent->getName() != NULL ? parent->getName() : "Panel" : "base layer");
#endif
            CCNode* node = (CCNode*)children->objectAtIndex(i);
            RawObject* result = NULL;
            if(isKindOfClass(node, CCLabelTTF))
            {
                CCLabelTTF* label = (CCLabelTTF*)node;
#if VERBOSE_LOAD_CCB
                CCLOG("label, font : %s", label->getFontName());
#endif
                CCString* translationKey = isKindOfClass(label, CustomBaseNode) ? (CCString*)dynamic_cast<CustomBaseNode*>(label)->getParameters()->objectForKey("translationKey") : NULL;
                const char* translated = Localization::getLocalizedString(translationKey != NULL ? translationKey->getCString() : label->getString());
                if(translationKey == NULL || translationKey->compare(translated) != 0)
                { //Don't replace the string if it's the same, as it may only be a key, not a real label
                    label->setString(translated);
                }
                result = layer->createLabelTTFromCCLabelTTF(label, parent);
            }
            else if(isKindOfClass(node, CCSprite))
            {
#if VERBOSE_LOAD_CCB
                CCLOG("image");
#endif
                CCSprite* sprite = (CCSprite*)node;
                result = layer->createImageFromCCSprite(sprite, parent);
            }
            else if(isKindOfClass(node, CustomInput))
            {
#if VERBOSE_LOAD_CCB
                CCLOG("input label");
#endif
                CCScale9Sprite* sprite = (CCScale9Sprite*)node;
                result = layer->createInputLabelFromCCScale9Sprite(sprite, parent);
                i--;
            }
            else if(isKindOfClass(node, CCScale9Sprite))
            {
#if VERBOSE_LOAD_CCB
                CCLOG("scale sprite");
#endif
                CCScale9Sprite* sprite = (CCScale9Sprite*)node;
                result = layer->createCustomObjectFromCCNode(sprite, parent);
            }
            else if(!isKindOfClass(node, CCEditBox))
            {
#if VERBOSE_LOAD_CCB
                CCLOG("panel");
#endif
                result = layer->createPanelFromCCNode(node, parent);
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
                    ((CCEditBox*)child->getNode())->setFontSize(input->getOriginalInfos()->getFontSize());
                }
            }
            if(isKindOfClass(child, InputLabel) && child->getEventInfos()->objectForKey("LinkTo") != NULL && isKindOfClass(child->getEventInfos()->objectForKey("LinkTo"), CCString))
            {
                InputLabel* input = (InputLabel*)child;
                if(input->getLinkTo() == NULL)
                {
                    CCString* linkTo = (CCString*)child->getEventInfos()->objectForKey("LinkTo");
                    CCArray* matchs = layer->allObjectsWithName(linkTo);
                    for(int j = 0; j < matchs->count(); j++)
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

CCArray* getAnimatioManagers()
{
    return animManagers;
}

NS_FENNEX_END