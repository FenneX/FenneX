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
#include "CustomDropDownList.h"

using namespace cocosbuilder;

NS_FENNEX_BEGIN
static float _loadingScale = 1;
static Size _loadSize = Size(-1, -1);
static bool isPhoneLayout = false;

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
            log("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth : %d", label->getSystemFontName().c_str(), label->getSystemFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height, depth);
#endif
        }
        else if(isKindOfClass(node, CustomInput))
        {
            CustomInput* input = (CustomInput*)node;
            input->setFontSize((float)input->getFontSize() * usedScale);
            input->setPreferredSize(Size(input->getPreferredSize().width * usedScale, input->getPreferredSize().height * usedScale));
#if VERBOSE_LOAD_CCB
            log("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth : %d", input->getFontSize(), parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height, depth);
#endif
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
        else if(isKindOfClass(node, Sprite))
        {
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

Panel* loadCCBFromFileToFenneX(std::string file, std::string inPanel, int zIndex)
{
    if(inPanel.empty())
    {
        animManagers.clear();
    }
    NodeLoaderLibrary * nodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    nodeLoaderLibrary->registerNodeLoader("CustomSprite", CustomSpriteLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomScaleSprite", CustomScaleSpriteLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomNode", CustomNodeLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomInput", CustomInputLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomLabel", CustomLabelLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CustomDropDownList", CustomDropDownListLoader::loader());
    CCBReader *ccbReader = new CCBReader(nodeLoaderLibrary);
    
    //IF this one fail, it needs to be silent
    bool shouldNotify = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    
#if VERBOSE_PERFORMANCE_TIME
    timeval startTime;
    gettimeofday(&startTime, NULL);
#endif
    std::string filePath = file +  (isPhoneLayout ? "-phone" : "") + ".ccbi";
    log("Filepath : %s", filePath.c_str());
    FileUtils::getInstance()->setPopupNotify(shouldNotify);
    Node* myNode = NULL;
    if(_loadSize.width == -1)
    {
        _loadSize = Director::getInstance()->getWinSize();
    }
    if(FileUtils::getInstance()->isFileExist(filePath))
    {
        log("File exist");
        myNode = ccbReader->readNodeGraphFromFile(filePath.c_str(), nullptr, _loadSize);
    }
    else if(isPhoneLayout)
    {
        filePath = file + ".ccbi";
        myNode = ccbReader->readNodeGraphFromFile(filePath.c_str(), nullptr, _loadSize);
    }
    
#if VERBOSE_PERFORMANCE_TIME
    timeval middleTime;
    gettimeofday(&middleTime, NULL);
    log("CCBReader loaded file %s in %f ms", file.c_str(), getTimeDifferenceMS(startTime, middleTime));
#endif
    
    //Despite cocosbuilder saying so, Label and Node (for Panel) aren't resized properly, so there it is
    /*Size frameSize = CCBLoaderGetLoadSize();
     float scaleX = (float)frameSize.width / designResolutionSize.width;
     float scaleY = (float)frameSize.height / designResolutionSize.height;
     float scale = MAX(scaleX, scaleY);
     float minScale = MIN(scaleX, scaleY);
     log("min : %f, max : %f, loading : %f", minScale, scale, _loadingScale);*/
    float usedScale = _loadingScale;
    
    //this code is dubious at best .... refactor it later
#if VERBOSE_LOAD_CCB
    log("ccb file %s loaded, doing rescaling ...", file.c_str());
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
            log("label font : %s", label->getSystemFontName().c_str());
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
            node->setContentSize(node->getContentSize() / node->getScaleX());
            Node* parentNode = node;
            
            for(auto nodeChild : node->getChildren())
            {
                //Depth 1 is a special case too, because nodes don't have to be moved and the scale is not calculated the same way
                if(isKindOfClass(nodeChild, Label))
                {
                    Label* label = (Label*)nodeChild;
                    label->setScale(label->getScale() / parentNode->getScale());
                    label->setSystemFontSize(label->getSystemFontSize() * parentNode->getScale());
                    /*TTFConfig newConfig = label->getTTFConfig();
                    newConfig.fontSize *= parentNode->getScale();
                    label->setTTFConfig(newConfig);*/
#if VERBOSE_LOAD_CCB
                    log("label font : %s, size : %f, scale : %f, parent node scale : %f, dimensions : %f, %f, depth 1", label->getSystemFontName().c_str(), label->getSystemFontSize(), label->getScale(), parentNode->getScale(), label->getDimensions().width, label->getDimensions().height);
#endif
                }
                else if(isKindOfClass(nodeChild, CustomInput))
                {
                    //input->setFontSize((float)input->getFontSize() / usedScale);
                    //input->setPreferredSize(Size(input->getPreferredSize().width / usedScale, input->getPreferredSize().height / usedScale));
                    //input->setFontSize((float)input->getFontSize() * parentNode->getScale());
#if VERBOSE_LOAD_CCB
                    CustomInput* input = (CustomInput*)nodeChild;
                    log("input font size : %d, parent node scale : %f, dimensions : %f, %f, depth 1", input->getFontSize() , parentNode->getScale(), input->getPreferredSize().width, input->getPreferredSize().height);
#endif
                }
                else if(isKindOfClass(nodeChild, ui::Scale9Sprite))
                {
                    nodeChild->setScaleX(nodeChild->getScaleX() / usedScale);
                    nodeChild->setScaleY(nodeChild->getScaleY() / usedScale);
                    nodeChild->setContentSize(SizeMult(nodeChild->getContentSize(), usedScale));
                }
                else if(isKindOfClass(nodeChild, Sprite))
                {
                    nodeChild->setScaleX(nodeChild->getScaleX() / usedScale);
                    nodeChild->setScaleY(nodeChild->getScaleY() / usedScale);
                }
                else if(!nodeChild->getChildren().empty())//Panel
                {
                    nodeChild->setScaleX(nodeChild->getScaleX() / usedScale);
                    nodeChild->setScaleY(nodeChild->getScaleY() / usedScale);
                    nodeChild->setContentSize(SizeMult(nodeChild->getContentSize(), usedScale));
                    //For depth 2 and more, the algorithm is the same
                    resizeChildren(parentNode, nodeChild, usedScale, 2);
                }
            }
        }
    }
    
    Panel* parent = NULL;
    if(!inPanel.empty())
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
    log("Node %s loaded to FenneX in %f ms, total with load file : %f ms", file.c_str(), getTimeDifferenceMS(middleTime, endTime), getTimeDifferenceMS(startTime, endTime));
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
        log("replaced base layer by CCB node : position : %f, %f, scale : %f", baseNode->getPosition().x, baseNode->getPosition().y, baseNode->getScale());
#endif
    }
    
    //Use an index because InputLabel modify the array, so you need to rewind a bit at some point
    for(int i = 0; i < baseNode->getChildren().size(); i++)
    {
        Node* node = baseNode->getChildren().at(i);
#if VERBOSE_LOAD_CCB
        log("doing child %d from parent %s ...", i, parent != NULL ? parent->getName() != "" ? parent->getName().c_str() : "Panel" : "base layer");
#endif
        RawObject* result = NULL;
        if(isKindOfClass(node, Label))
        {
            Label* label = (Label*)node;
#if VERBOSE_LOAD_CCB
            log("label, font : %s", label->getSystemFontName().c_str());
#endif
            std::string translationKey;
            if(isKindOfClass(label, CustomBaseNode))
            {
                ValueMap& parameters = dynamic_cast<CustomBaseNode*>(label)->getParameters();
                if(parameters.find("translationKey") != parameters.end())
                {
                    translationKey = parameters["translationKey"].asString();
                }
            }
            
            const std::string translated = Localization::getLocalizedString(!translationKey.empty() ? translationKey : label->getString());
            if(translationKey.size() == 0 || translationKey != translated)
            { //Don't replace the string if it's the same, as it may only be a key, not a real label
                label->setString(translated);
            }
            result = layer->createLabelTTFromLabel(label, parent);
        }
        else if(isKindOfClass(node, CustomDropDownList))
        {
#if VERBOSE_LOAD_CCB
            log("DropDownList");
#endif
            Sprite* sprite = (Sprite*)node;
            result = layer->createDropDownListFromSprite(sprite, parent);
        }
        else if(isKindOfClass(node, CustomInput))
        {
#if VERBOSE_LOAD_CCB
            log("input label");
#endif
            ui::Scale9Sprite* sprite = (ui::Scale9Sprite*)node;
            
            std::string translationKey;
            if(isKindOfClass(sprite, CustomBaseNode))
            {
                ValueMap& parameters = dynamic_cast<CustomBaseNode*>(sprite)->getParameters();
                if(parameters.find("translationKey") != parameters.end())
                {
                    translationKey = parameters["translationKey"].asString();
                }
            }
            std::string placeHolder = isKindOfClass(sprite, CustomInput) ? ((CustomInput*) sprite)->getPlaceHolder() : "";
            
            result = layer->createInputLabelFromScale9Sprite(sprite, parent);
            
            const std::string text = Localization::getLocalizedString(!translationKey.empty() ? translationKey : placeHolder);
            if(translationKey.size() == 0 || translationKey != text)
            { //Don't replace the string if it's the same, as it may only be a key, not a real label
                ((InputLabel*) result)->setInitialText(text);
            }
            i--;
        }
        else if(isKindOfClass(node, ui::Scale9Sprite))
        {
#if VERBOSE_LOAD_CCB
            log("scale sprite");
#endif
            ui::Scale9Sprite* sprite = (ui::Scale9Sprite*)node;
            result = layer->createCustomObjectFromNode(sprite, parent);
        }
        else if(isKindOfClass(node, Sprite))
        {
#if VERBOSE_LOAD_CCB
            log("image");
#endif
            Sprite* sprite = (Sprite*)node;
            result = layer->createImageFromSprite(sprite, parent);
        }
        else if(!isKindOfClass(node, ui::EditBox))
        {
#if VERBOSE_LOAD_CCB
            log("Edit Box");
#endif
            result = layer->createPanelFromNode(node, parent);
        }
#if VERBOSE_LOAD_CCB
        if(result != NULL)
        {
            log("Child %d loaded : position : %f, %f, scale : %f", i, result->getPosition().x, result->getPosition().y, result->getScale());
        }
        else
        {
            log("Problem loading child %d !", i);
        }
#endif
    }
}

void reorderZindex()
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    Vector<RawObject*> children = layer->all();
    for(RawObject* child : children)
    {
        if(child->getZOrder() != 0)
        {
            layer->reorderChild(child, child->getZOrder());
        }
    }
}

void linkInputLabels()
{
    GraphicLayer* layer = GraphicLayer::sharedLayer();
    Vector<RawObject*> children = layer->all();
    for(RawObject* child : children)
    {
        //Force actualization of content size and fontSize after everything is loaded because the nodeToWorldTransform is only right after
        if(isKindOfClass(child, InputLabel))
        {
            InputLabel* input = (InputLabel*)child;
            child->getNode()->setContentSize(child->getNode()->getContentSize());
            if(input->getOriginalInfos() != NULL)
            {
                input->setFontSize(input->getOriginalInfos()->getFontSize());
            }
        }
        if((isKindOfClass(child, DropDownList)) && isValueOfType(child->getEventInfos()["LinkTo"], STRING))
        {
            DropDownList* dropDownList = (DropDownList*)child;
            if(dropDownList->getLinkTo() == NULL)
            {
                std::string linkTo = child->getEventInfos()["LinkTo"].asString();
                Panel* parent = layer->getContainingPanel(dropDownList);
                for(RawObject* obj : parent != NULL ? parent->getChildren() : layer->all())
                {
                    if(obj->getName() == linkTo && isKindOfClass(obj, LabelTTF)) dropDownList->setLinkTo((LabelTTF*)obj);
                }
            }
        }
    }
}

void CCBLoaderSetLoadSize(Size loadSize)
{
    _loadSize = loadSize;
}

Size CCBLoaderGetLoadSize()
{
    return _loadSize;
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
    isPhoneLayout = usePhone;
}

bool CCBLoaderIsPhoneLayout()
{
    return isPhoneLayout;
}

std::vector<CCBAnimationManager*>& getAnimationManagers()
{
    return animManagers;
}

NS_FENNEX_END
