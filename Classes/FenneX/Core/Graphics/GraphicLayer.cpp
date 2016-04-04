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

#include "GraphicLayer.h"
#include "Shorteners.h"
#include "SynchronousReleaser.h"
#include "FenneXCCBLoader.h"
#include "AppMacros.h"
#include "AnalyticsWrapper.h"

NS_FENNEX_BEGIN
// singleton stuff
static GraphicLayer *s_SharedLayer = NULL;

GraphicLayer* GraphicLayer::sharedLayer(void)
{
    if (!s_SharedLayer)
    {
        s_SharedLayer = new GraphicLayer();
        s_SharedLayer->init();
    }
    
    return s_SharedLayer;
}

void GraphicLayer::init()
{
    nextAvailableId = 0;
    relatedScene = NULL;
    isPaused = false;
    mainPanels = new CCArray();
    storedObjects = new CCArray();
    storedPanels = new CCArray();
    layer = Layer::create();
    layer->retain();
    depthInScene = 0;
    clock = 0;
    isUpdating = false;
    objectsToAdd = new CCArray();
    objectsToRemove = new CCArray();
    childParent = new CCDictionary();
    tapObserver = NULL;
}

GraphicLayer::~GraphicLayer()
{
#if VERBOSE_DEALLOC
    CCLOG("Layer dealloc");
#endif
    this->clear();
    s_SharedLayer = NULL;
    mainPanels->release();
    storedObjects->release();
    storedPanels->release();
    layer->release();
    objectsToAdd->release();
    objectsToRemove->release();
    childParent->release();
}

void GraphicLayer::setTapObserver(ButtonTapObserver* observer)
{
    tapObserver = observer;
}

ButtonTapObserver* GraphicLayer::getTapObserver()
{
    return tapObserver;
}

void GraphicLayer::useBaseLayer(Layer* otherLayer)
{
    Node* parent = layer->getParent();
    parent->addChild(otherLayer, depthInScene);
    parent->removeChild(layer, true);
    //Ensure the layer is properly released: actually, only the first one (at launch) will have a retain count of 2 here
    if(layer->getReferenceCount() == 2)
    {
        layer->release();
    }
    layer = otherLayer;
    layer->retain();
}

void GraphicLayer::renderOnLayer(Scene* destination, int depth)
{
    relatedScene = destination;
    depthInScene = depth;
    //Ensure that the layer is not on another Scene
    if(layer->getParent() != NULL)
    {
        layer->removeFromParentAndCleanup(true);
    }
    storedObjects->removeAllObjects();
    destination->addChild(layer, depth);
    layer->setScale(SceneSwitcher::sharedSwitcher()->getScale());
}

void GraphicLayer::stopRenderOnLayer(Scene* destination, bool cleanup)
{
    relatedScene = NULL;
    destination->removeChild(layer, cleanup);
    this->clear();
}
void GraphicLayer::stop()
{
    this->stopRenderOnLayer(relatedScene, false);
}

/* TODO : create it if needed
 RawObject* GraphicLayer::createObject(Ref* firstObject, ... )
 {
 return NULL;
 }
 
 RawObject* GraphicLayer::createObject(CCDictionary* values)
 {
 return NULL;
 }*/

Image* GraphicLayer::createImage(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createImage(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createImage called with no firstObject");
    }
#endif
    return NULL;
}

Image* GraphicLayer::createImage(CCDictionary* values)
{
    Image* img = NULL;
    
    //try to create an image : each value is checked : if it exists and if it is of the right type
    //an image should have at least an ImageFile (String) or ImageData and cocosName (String) for texture
    if((values->objectForKey("ImageFile") != NULL
        && isKindOfClass(values->objectForKey("ImageFile"), CCString)) ||
       /*(values->objectForKey("ImageData") != NULL
        && values->objectForKey("cocosName") != NULL
        &&  isKindOfClass(values->objectForKey("cocosName"), CCString) ) ||*/
       (values->objectForKey("SpriteSheetFile") != NULL
        &&  isKindOfClass(values->objectForKey("SpriteSheetFile"), CCString)
        && values->objectForKey("Capacity") != NULL
        &&  isKindOfClass(values->objectForKey("Capacity"), CCInteger)))
    {
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        if(values->objectForKey("ImageFile") != NULL
           && isKindOfClass(values->objectForKey("ImageFile"), CCString))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("creating image %s", ((CCString*)values->objectForKey("ImageFile"))->getCString());
#endif
            img = new Image(((CCString*)values->objectForKey("ImageFile"))->getCString(), pos);
        }
        /*else if(values->objectForKey("cocosName") != NULL
         && [values->objectForKey("cocosName"] isKindOfClass:[NSString class]])
         {
         CGImageRef imageRef = (CGImageRef) values->objectForKey("ImageData"];
         img = [[Image alloc] initWithImageData:imageRef location:pos name:values->objectForKey("cocosName"]];
         }*/
        else
        {
            img = new Image(((CCString*)values->objectForKey("SpriteSheetFile"))->getCString(), pos, ((CCInteger*)values->objectForKey("Capacity"))->getValue());
        }
        
        
        if(img != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(img, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(img);
            }
            img->release();
            
            //try to process optional parameters if they exist : Name (String), EventName (String)  Panel (Panel)
            if(values->objectForKey("Name") != NULL && isKindOfClass(values->objectForKey("Name"), CCString))
            {
                img->setName(((CCString*)values->objectForKey("Name"))->getCString());
            }
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                img->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                img->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    img->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(img, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                img->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
            {
                ((Sprite*)img->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
            }
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                img->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
        }
        else
        {
#if VERBOSE_WARNING
            CCLOG("Warning : error creating image with file %s, perhaps the file name is incorrect or the file is too large", ((CCString*)values->objectForKey("ImageFile"))->getCString());
#endif
        }
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createImage aborted, incorrect parameter types");
    }
#endif
#if VERBOSE_LOAD_CCB
    CCLOG("Ended creating image");
#endif
    return img;
}

Image* GraphicLayer::createImageFromSprite(Sprite* sprite, Panel* parent)
{
    Image* img = NULL;
    img = new Image(sprite);
    if(img != NULL)
    {
        storedObjects->addObject(img);
        if(parent != NULL)
        {
            parent->addChild(img);
            childParent->setObject(parent, img->getID());
        }
        img->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(sprite), img);
    return img;
}

CustomObject* GraphicLayer::createCustomObjectFromNode(Node* node, Panel* parent)
{
    CustomObject* obj = NULL;
    obj = new CustomObject(node);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childParent->setObject(parent, obj->getID());
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(node), obj);
    return obj;
}

CustomObject* GraphicLayer::createCustomObject(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createCustomObject(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createCustomObject called with no firstObject");
    }
#endif
    return NULL;
}

CustomObject* GraphicLayer::createCustomObject(CCDictionary* values)
{
    CustomObject* obj = NULL;
    
    //try to create an image : each value is checked : if it exists and if it is of the right type
    //an image should have at least an ImageFile (String) or ImageData and cocosName (String) for texture
    if(values->objectForKey("Delegate") != NULL
       && isKindOfClass(values->objectForKey("Delegate"), Node))
    {
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        if(values->objectForKey("Delegate") != NULL
           && isKindOfClass(values->objectForKey("Delegate"), Node))
            
        {
            obj = new CustomObject((Node*)values->objectForKey("Delegate"), pos);
        }
        
        
        if(obj != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(obj, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(obj);
            }
            obj->release();
            
            //try to process optional parameters if they exist : Name (String), EventName (String)  Panel (Panel)
            if(values->objectForKey("Name") != NULL && isKindOfClass(values->objectForKey("Name"), CCString))
            {
                obj->setName(((CCString*)values->objectForKey("Name"))->getCString());
            }
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                obj->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                obj->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    obj->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(obj, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                obj->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
            {
                ((Sprite*)obj->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
            }
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                obj->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : error creating CustomObject");
        }
#endif
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createCustomObject aborted, incorrect parameter types");
    }
#endif
#if VERBOSE_LOAD_CCB
    CCLOG("Ended creating Custom Object");
#endif
    return obj;
}



LabelTTF* GraphicLayer::createLabelTTF(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createLabelTTF(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createLabelTTF called with no firstObject");
    }
#endif
    return NULL;
}

LabelTTF* GraphicLayer::createLabelTTF(CCDictionary* values)
{
    LabelTTF* label = NULL;
    
    //try to create an image : each value is checked : if it exists and if it is of the right type
    //an image should have at least an ImageFile (String) or ImageData and cocosName (String) for texture
    if(values->objectForKey("Label") != NULL
       && isKindOfClass(values->objectForKey("Label"), CCString)
       && values->objectForKey("FontFile") != NULL
       && isKindOfClass(values->objectForKey("FontFile"), CCString))
    {
#if VERBOSE_LOAD_CCB
        CCLOG("creating labelTTF %s", ((CCString*)values->objectForKey("Label"))->getCString());
#endif
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        if(values->objectForKey("Dimensions") != NULL
           && isKindOfClass(values->objectForKey("Dimensions"), Size)
           &&values->objectForKey("TextFormat") != NULL
           && isKindOfClass(values->objectForKey("TextFormat"), CCInteger))
        {
            label = new LabelTTF(((CCString*)values->objectForKey("Label"))->getCString(),
                                 ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                 pos,
                                 *(Size*)(values->objectForKey("Dimensions")),
                                 (TextHAlignment)((CCInteger*)values->objectForKey("TextFormat"))->getValue());
        }
        else if(values->objectForKey("Dimensions") != NULL
                && isKindOfClass(values->objectForKey("Dimensions"), Size))
        {
            label = new LabelTTF(((CCString*)values->objectForKey("Label"))->getCString(),
                                 ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                 pos,
                                 *(Size*)(values->objectForKey("Dimensions")));
        }
        else
        {
            label = new LabelTTF(((CCString*)values->objectForKey("Label"))->getCString(),
                                 ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                 pos);
        }
        
        
        if(label != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(label, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(label);
            }
            label->release();
            
            //try to process optional parameters if they exist : Name (String), EventName (String)  Panel (Panel)
            if(values->objectForKey("Name") != NULL && isKindOfClass(values->objectForKey("Name"), CCString))
            {
                label->setName(((CCString*)values->objectForKey("Name"))->getCString());
            }
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                label->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                label->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    label->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(label, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                label->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
            {
                ((CCLabelBMFont*)label->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
            }
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                label->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : error creating labelTTF with font %s, perhaps the file name is incorrect or the file is too large", ((CCString*)values->objectForKey("FontFile"))->getCString());
        }
#endif
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createLabelTTF aborted, incorrect parameter types");
    }
#endif
#if VERBOSE_LOAD_CCB
    CCLOG("Ended creating labelTTF");
#endif
    return label;
}

LabelTTF* GraphicLayer::createLabelTTFromLabel(Label* cocosLabel, Panel* parent)
{
    LabelTTF* label = NULL;
    label = new LabelTTF(cocosLabel);
    if(label != NULL)
    {
        storedObjects->addObject(label);
        if(parent != NULL)
        {
            parent->addChild(label);
            childParent->setObject(parent, label->getID());
        }
        label->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosLabel), label);
    return label;
}


InputLabel* GraphicLayer::createInputLabel(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createInputLabel(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createInputLabel called with no firstObject");
    }
#endif
    return NULL;
}

InputLabel* GraphicLayer::createInputLabel(CCDictionary* values)
{
    InputLabel* label = NULL;
    
    //try to create an input label : each value is checked : if it exists and if it is of the right type
    if(values->objectForKey("PlaceHolder") != NULL
       && isKindOfClass(values->objectForKey("PlaceHolder"), CCString)
       && values->objectForKey("FontFile") != NULL
       && isKindOfClass(values->objectForKey("FontFile"), CCString)
       && values->objectForKey("FontSize") != NULL
       && isKindOfClass(values->objectForKey("FontSize"), CCInteger))
    {
#if VERBOSE_LOAD_CCB
        CCLOG("creating input label %s", ((CCString*)values->objectForKey("PlaceHolder"))->getCString());
#endif
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        
        ui::EditBox::InputMode inputMode = values->objectForKey("KeyboardType") != NULL && isKindOfClass(values->objectForKey("KeyboardType"), CCInteger) ? (ui::EditBox::InputMode)TOINT(values->objectForKey("KeyboardType")) : ui::EditBox::InputMode::ANY;
        int maxDigits = values->objectForKey("MaxDigits") != NULL && isKindOfClass(values->objectForKey("MaxDigits"), CCInteger) ? TOINT(values->objectForKey("MaxDigits")) : -1;
        if(values->objectForKey("Dimensions") != NULL
           && isKindOfClass(values->objectForKey("Dimensions"), Size)
           &&values->objectForKey("TextFormat") != NULL
           && isKindOfClass(values->objectForKey("TextFormat"), CCInteger))
        {
            label = new InputLabel(((CCString*)values->objectForKey("PlaceHolder"))->getCString(),
                                   ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                   TOINT(values->objectForKey("FontSize")),
                                   pos,
                                   inputMode,
                                   maxDigits,
                                   *(Size*)(values->objectForKey("Dimensions")),
                                   (TextHAlignment)TOINT(values->objectForKey("TextFormat")));
        }
        else if(values->objectForKey("Dimensions") != NULL
                && isKindOfClass(values->objectForKey("Dimensions"), Size))
        {
            label = new InputLabel(((CCString*)values->objectForKey("PlaceHolder"))->getCString(),
                                   ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                   TOINT(values->objectForKey("FontSize")),
                                   pos,
                                   inputMode,
                                   maxDigits,
                                   *(Size*)(values->objectForKey("Dimensions")));
        }
        else
        {
            label = new InputLabel(((CCString*)values->objectForKey("PlaceHolder"))->getCString(),
                                   ((CCString*)values->objectForKey("FontFile"))->getCString(),
                                   TOINT(values->objectForKey("FontSize")),
                                   pos);
        }
        
        
        if(label != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(label, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(label);
            }
            label->release();
            
            //try to process optional parameters if they exist : Label (String), Name (String), EventName (String)  Panel (Panel)
            if(values->objectForKey("Label") != NULL && isKindOfClass(values->objectForKey("Label"), CCString))
            {
                label->setLabelValue(((CCString*)values->objectForKey("Label"))->getCString());
            }
            if(values->objectForKey("Name") != NULL && isKindOfClass(values->objectForKey("Name"), CCString))
            {
                label->setName(((CCString*)values->objectForKey("Name"))->getCString());
            }
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                label->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                label->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    label->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(label, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                label->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
            {
                ((CCLabelBMFont*)label->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
            }
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                label->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
            label->update(0);//used to apply text change right now so that the label is resized correctly right now
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : error creating label with fontfile %s, perhaps the file name is incorrect or the file is too large", ((CCString*)values->objectForKey("FontFile"))->getCString());
        }
#endif
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createInputLabel aborted, incorrect parameter types");
    }
#endif
#if VERBOSE_LOAD_CCB
    CCLOG("ended creating InputLabel");
#endif
    return label;
}


InputLabel* GraphicLayer::createInputLabelFromScale9Sprite(ui::Scale9Sprite* cocosSprite, Panel* parent)
{
    InputLabel* label = NULL;
    
    //ui::EditBoxInputMode inputMode = /*(ui::EditBoxInputMode)TOINT(values->objectForKey("KeyboardType"))*/ kui::EditBoxInputModeAny;
    //int maxDigits = values->objectForKey("MaxDigits") != NULL && isKindOfClass(values->objectForKey("MaxDigits"), CCInteger) ? TOINT(values->objectForKey("MaxDigits")) : -1;
    label = new InputLabel(cocosSprite);
    
    if(label != NULL)
    {
        //TODO : find a way to add the label at the right place in cocos hierarchy
        if(parent != NULL)
        {
            storedObjects->addObject(label);
            parent->addChild(label);
            childParent->setObject(parent, label->getID());
        }
        else
        {
            this->addObject(label);
        }
        label->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosSprite), label);
    
    return label;
}

Panel* GraphicLayer::createPanel(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createPanel(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createPanel called with no firstObject");
    }
#endif
    return NULL;
}

Panel* GraphicLayer::createPanel(CCDictionary* values)
{
    
    Panel* panel = NULL;
    
    if(values->objectForKey("Name") != NULL
       && isKindOfClass(values->objectForKey("Name"), CCString))
    {
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        panel = new Panel(((CCString*)values->objectForKey("Name"))->getCString(), pos);
        
        
        if(panel != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(panel, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(panel);
            }
            panel->release();
            
            //try to process optional parameters if they exist : EventName (String)  Panel (Panel)
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                panel->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                panel->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    panel->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(panel, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                panel->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            /*if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
             {
             ((Sprite*)img->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
             }*/
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                panel->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : error creating panel with name %s", ((CCString*)values->objectForKey("Name"))->getCString());
        }
#endif
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createPanel aborted, incorrect parameter types");
    }
#endif
    return panel;
}

Panel* GraphicLayer::createPanelFromNode(Node* cocosNode, Panel* parent)
{
    Panel* panel = NULL;
    panel = new Panel(cocosNode);
    if(panel != NULL)
    {
        storedObjects->addObject(panel);
        storedPanels->addObject(panel);
        if(parent != NULL)
        {
            parent->addChild(panel);
            childParent->setObject(parent, panel->getID());
        }
        panel->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosNode), panel);
    loadNodeToFenneX(panel->getNode(), panel);
    return panel;
}

Panel* GraphicLayer::createPanelWithNode(const char* name, Node* panelNode, int zOrder)
{
    Panel* panel = NULL;
    panel = new Panel(panelNode, name);
    if(panel != NULL)
    {
        this->addObject(panel, zOrder);
        panel->release();
    }
    return panel;
}

DropDownList* GraphicLayer::createDropDownList(Ref* firstObject, ... )
{
    Ref* eachObject;
    va_list argumentList;
    bool key = true;
    Ref* object;
    if (firstObject)                      // The first argument isn't part of the varargs list,
    {                                   // so we'll handle it separately.
        //put all parameters in a Dictionary to access them as key/value pairs
        CCDictionary* values = CCDictionary::create();
        object = firstObject;
        va_start(argumentList, firstObject);          // Start scanning for arguments after firstObject.
        while ((eachObject = va_arg(argumentList, Ref*)) != NULL) // As many times as we can get an argument of type "id"
        {
            if(key)
            {
                //keys should be Strings
                if(!isKindOfClass(eachObject, CCString))
                {
#if VERBOSE_WARNING
                    CCLOG("Warning : not a key, value ignored");
#endif
                }
                else
                {
                    CCString* key = (CCString*)eachObject;
                    values->setObject(object, key->_string);
                }
            }
            else
            {
                object = eachObject;
            }
            key = !key;
        }
        va_end(argumentList);
        return this->createDropDownList(values);
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createDropDownList called with no firstObject");
    }
#endif
    return NULL;
}

DropDownList* GraphicLayer::createDropDownList(CCDictionary* values)
{
    DropDownList* dropDownList = NULL;
    
    //try to create an image : each value is checked : if it exists and if it is of the right type
    //an image should have at least an ImageFile (String) or ImageData and cocosName (String) for texture
    if((values->objectForKey("SpriteSheetFile") != NULL
        &&  isKindOfClass(values->objectForKey("SpriteSheetFile"), CCString)
        && values->objectForKey("Capacity") != NULL
        &&  isKindOfClass(values->objectForKey("Capacity"), CCInteger)))
    {
        Vec2 pos = Vec2(0, 0);
        if(values->objectForKey("PositionX") != NULL
           && isKindOfClass(values->objectForKey("PositionX"), CCInteger))
        {
            pos.x = ((CCInteger*)values->objectForKey("PositionX"))->getValue();
        }
        if(values->objectForKey("PositionY") != NULL
           && isKindOfClass(values->objectForKey("PositionY"), CCInteger))
        {
            pos.y =  ((CCInteger*)values->objectForKey("PositionY"))->getValue();
        }
        if(values->objectForKey("Position") != NULL
           && isKindOfClass(values->objectForKey("Position"), TMPPoint))
        {
            pos.x =  ((TMPPoint*)values->objectForKey("Position"))->x;
            pos.y =  ((TMPPoint*)values->objectForKey("Position"))->y;
        }
        if(values->objectForKey("ImageFile") != NULL
           && isKindOfClass(values->objectForKey("ImageFile"), CCString))
        {
#if VERBOSE_LOAD_CCB
            CCLOG("creating DropDownList %s", ((CCString*)values->objectForKey("ImageFile"))->getCString());
#endif
            dropDownList = new DropDownList(((CCString*)values->objectForKey("ImageFile"))->getCString(), pos);
        }
        else
        {
            dropDownList = new DropDownList(((CCString*)values->objectForKey("SpriteSheetFile"))->getCString(), pos, ((CCInteger*)values->objectForKey("Capacity"))->getValue());
        }
        
        
        if(dropDownList != NULL)
        {
            if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
            {
                this->addObject(dropDownList, ((CCInteger*)values->objectForKey("Zindex"))->getValue());
            }
            else
            {
                this->addObject(dropDownList);
            }
            dropDownList->release();
            
            //try to process optional parameters if they exist : Name (String), EventName (String)  Panel (Panel)
            if(values->objectForKey("Name") != NULL && isKindOfClass(values->objectForKey("Name"), CCString))
            {
                dropDownList->setName(((CCString*)values->objectForKey("Name"))->getCString());
            }
            if(values->objectForKey("EventName") != NULL && isKindOfClass(values->objectForKey("EventName"), CCString))
            {
                dropDownList->setEventName(((CCString*)values->objectForKey("EventName"))->getCString());
            }
            if(values->objectForKey("Scale") != NULL && isKindOfClass(values->objectForKey("Scale"), CCFloat))
            {
                dropDownList->setScale(((CCFloat*)values->objectForKey("Scale"))->getValue());
            }
            if(values->objectForKey("EventInfos") != NULL && isKindOfClass(values->objectForKey("EventInfos"), CCDictionary))
            {
                CCDictionary* parameters = (CCDictionary*)values->objectForKey("EventInfos");
                CCArray* keys = parameters->allKeys();
                for(int i = 0; i < keys->count(); i++)
                {
                    std::string key = ((CCString*)keys->objectAtIndex(i))->_string;
                    dropDownList->setEventInfo(parameters->objectForKey(key), key);
                }
            }
            
            if(values->objectForKey("Panel") != NULL && isKindOfClass(values->objectForKey("Panel"), Panel))
            {
#if VERBOSE_WARNING
                if(values->objectForKey("Zindex") != NULL && isKindOfClass(values->objectForKey("Zindex"), CCInteger))
                {
                    CCLOG("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
                }
#endif
                this->placeObject(dropDownList, (Panel*)values->objectForKey("Panel"));
            }
            if(values->objectForKey("Visible") != NULL && isKindOfClass(values->objectForKey("Visible"), CCInteger))
            {
                dropDownList->setVisible(((CCInteger*)values->objectForKey("Visible"))->getValue());
            }
            if(values->objectForKey("Opacity") != NULL && isKindOfClass(values->objectForKey("Opacity"), CCInteger))
            {
                ((Sprite*)dropDownList->getNode())->setOpacity(((CCInteger*)values->objectForKey("Opacity"))->getValue());
            }
            if(values->objectForKey("Help") != NULL && isKindOfClass(values->objectForKey("Help"), CCString))
            {
                dropDownList->setHelp(((CCString*)values->objectForKey("Help"))->getCString());
            }
        }
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : createDropDownList aborted, incorrect parameter types");
    }
#endif
#if VERBOSE_LOAD_CCB
    CCLOG("Ended creating DropDownList");
#endif
    return dropDownList;
}

DropDownList* GraphicLayer::createDropDownListFromSprite(Sprite* sprite, Panel* parent)
{
    DropDownList* dropDownList = NULL;
    dropDownList = new DropDownList(sprite);
    if(dropDownList != NULL)
    {
        storedObjects->addObject(dropDownList);
        if(parent != NULL)
        {
            parent->addChild(dropDownList);
            childParent->setObject(parent, dropDownList->getID());
        }
        dropDownList->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(sprite), dropDownList);
    return dropDownList;
}

RawObject* GraphicLayer::duplicateObject(RawObject* otherObject)
{
    RawObject* obj = NULL;
    //Create the object, set Position + other properties
    if(isKindOfClass(otherObject, Image))
    {
        Image* otherImage = (Image*)otherObject;
        if(otherImage->isAnimation())
        {
#if VERBOSE_WARNING
            CCLOG("Warning: animated Image not supported for duplicate yet");
#endif
            return NULL;
        }
        else
        {
            obj = new Image(otherImage->getImageFile(), otherImage->getPosition());
        }
    }
    else if(isKindOfClass(otherObject, LabelTTF))
    {
        LabelTTF* otherLabel = (LabelTTF*)otherObject;
        //const char* labelString, const char* filename, Vec2 location, Size dimensions, TextHAlignment format
        obj = new LabelTTF(otherLabel->getLabelValue(), otherLabel->getFullFontFile()->getCString(), otherLabel->getPosition(), otherLabel->getDimensions(), otherLabel->getAlignment());
        ((Label*)obj->getNode())->setColor(((Label*)otherLabel->getNode())->getColor());
        ((LabelTTF*)obj)->setFitType(otherLabel->getFitType());
        ((LabelTTF*)obj)->setFontSize(((LabelTTF*)otherObject)->getFontSize());
        ((Label*)obj->getNode())->setSystemFontName(((Label*)otherObject->getNode())->getSystemFontName());
    }
    else if(isKindOfClass(otherObject, Panel))
    {
        obj = new Panel(otherObject->getName(), otherObject->getPosition());
        obj->getNode()->setContentSize(otherObject->getNode()->getContentSize());
    }
    else if(isKindOfClass(otherObject, CustomObject) && isKindOfClass(otherObject->getNode(), ui::Scale9Sprite))
    {
        ui::Scale9Sprite* otherNode = (ui::Scale9Sprite*)otherObject->getNode();
        ui::Scale9Sprite* node = ui::Scale9Sprite::create(TOCSTRING(otherObject->getEventInfos()->objectForKey("spriteFrame")), Rect(0, 0, 0, 0), otherNode->getCapInsets());
        node->setPosition(otherNode->getPosition());
        node->setPreferredSize(otherNode->getPreferredSize());
        node->setAnchorPoint(otherNode->getAnchorPoint());
        obj = new CustomObject(node);
    }
    else
    {
#if VERBOSE_WARNING
        CCLOG("Warning: object type not supported for duplicate yet");
#endif
        return NULL;
    }
    
    //Actually add the object
    this->addObject(obj, otherObject->getZOrder());
    this->placeObject(obj, getContainingPanel(otherObject));
    obj->release();
    
    //Set other infos
    obj->setName(otherObject->getName());
    obj->setEventName(otherObject->getEventName());
    obj->setVisible(otherObject->isVisible());
    obj->getNode()->setAnchorPoint(otherObject->getNode()->getAnchorPoint());
    obj->addEventInfos(otherObject->getEventInfos());
    obj->setScaleX(otherObject->getScaleX());
    obj->setScaleY(otherObject->getScaleY());
    obj->getNode()->setRotation(otherObject->getNode()->getRotation());
    
    //Recursively add children for Panel
    if(isKindOfClass(otherObject, Panel))
    {
        CCArray* children = ((Panel*)otherObject)->getChildren();
        for(int i = 0; i < children->count(); i++)
        {
            RawObject* otherChild = (RawObject*)children->objectAtIndex(i);
            RawObject* child = duplicateObject(otherChild);
            placeObject(child, (Panel*)obj);
        }
    }
    return obj;
}

RawObject* GraphicLayer::placeObject(RawObject* obj, Panel* panel)
{
    if(storedObjects->containsObject(obj))
    {
        if(this->getContainingPanel(obj) != NULL)
        {
            this->removeObjectFromPanel(obj, this->getContainingPanel(obj));
        }
        if(panel != NULL)
        {
            if(obj->getNode() != NULL)
            {
                layer->removeChild(obj->getNode(), false);
                panel->addChild(obj);
                //re-insert child at a specific position as children need to be ordered by zIndex
                storedObjects->removeObject(obj);
                storedObjects->insertObject(obj, storedObjects->indexOfObject(panel));
                //TODO : problem with obj zOrder : it does not correspond to panel zOrder
                childParent->setObject(panel, obj->getID());
            }
#if VERBOSE_WARNING
            else
            {
                CCLOG("Warning : child %s doesn't have a Node, you shouldn't try to place it on a panel", obj->getName().c_str());
            }
#endif
        }
    }
    else if(panel != NULL)
    {
        for(int i = 0; i < objectsToAdd->count(); i++)
        {
            CCArray* objInfos = (CCArray*)objectsToAdd->objectAtIndex(i);
            if(objInfos->objectAtIndex(0) == obj)
            {
                objInfos->addObject(panel);
            }
        }
    }
    return obj;
}

void GraphicLayer::removeObjectFromPanel(RawObject* obj, Panel* panel)
{
    if(obj->getNode() != NULL)
    {
        panel->removeChild(obj);
        layer->addChild(obj->getNode());
        if(childParent->objectForKey(obj->getID()) != NULL)
        {
            childParent->removeObjectForKey(obj->getID());
        }
        else
        {
            CCASSERT(1, "Problem with childParent");
        }
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : child %s doesn't have a Node, you shouldn't try to remove it from a panel", obj->getName().c_str());
    }
#endif
}

void GraphicLayer::removeAllObjectsFromPanel(Panel* panel)
{
    while(panel->getChildren()->count() > 0)
    {
        this->removeObjectFromPanel((RawObject*)panel->getChildren()->objectAtIndex(0), panel);
    }
}

Panel* GraphicLayer::getContainingPanel(RawObject* obj)
{
    if(obj == NULL) return NULL;
    return (Panel*)childParent->objectForKey(obj->getID());
    /* OLD unefficient way to get the containing panel by trying all panels
     Panel* containingPanel = NULL;
     for(int i = 0; i < storedPanels->count(); i++)
     {
     Panel* panel = (Panel*)storedPanels->objectAtIndex(i);
     if(panel->containsObject(obj))
     {
     containingPanel = panel;
     }
     }
     return containingPanel;*/
}

void GraphicLayer::destroyObject(RawObject* obj)
{
    if(obj != NULL && storedObjects->containsObject(obj))
    {
        if(isUpdating)
        {
            objectsToRemove->addObject(obj);
        }
        else
        {
            if(isKindOfClass(obj, Panel))
            {
                //we can't use copy anymore, but clone doesn't work with RawObjects. Manually clone instead
                Array* childrenCopy = Acreate();
                CCArray* childrenToClear = ((Panel*)obj)->getChildren();
                for(int i = 0; i < childrenToClear->count(); i++)
                {
                    childrenCopy->addObject(childrenToClear->objectAtIndex(i));
                }
                this->destroyObjects(childrenCopy);
                storedPanels->removeObject(obj);
                CCArray* children = ((Panel*)obj)->getChildren();
                CCASSERT(children == NULL || children->count() == 0, "Problem with panel children when releasing panel");
                CCArray* keys = childParent->allKeysForObject(obj);
                CCASSERT(keys == NULL || keys->count() == 0, "Problem with child parent when releasing panel");
            }
            this->placeObject(obj);
            if(obj->getNode() != NULL)
            {
                layer->removeChild(obj->getNode(), true);
            }
            SynchronousReleaser::sharedReleaser()->addObjectToReleasePool(obj);
            storedObjects->removeObject(obj);
        }
    }
    else
    {
#if VERBOSE_WARNING
        CCLOG("Warning : trying to destroy not valid object at adress %p", obj);
#endif
        if(obj != NULL && storedPanels->containsObject(obj))
        {
            storedPanels->removeObject(obj);
        }
    }
}

void GraphicLayer::destroyObjects(CCArray* array)
{
    for(int i = array->count() - 1; i >= 0; i--)
    {
        this->destroyObject((RawObject*)array->objectAtIndex(i));
    }
}

void GraphicLayer::destroyObjects(Vector<RawObject*> array)
{
    for(auto obj : array)
    {
        this->destroyObject(obj);
    }
}

void GraphicLayer::destroyObjectEvent(EventCustom* event)
{
    if(event != NULL && event->getUserData() != NULL)
    {
        if(isKindOfClass((Ref*)event->getUserData(), RawObject))
        {
            this->destroyObject((RawObject*)event->getUserData());
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : sent a non RawObject to destroyObject, ignoring it");
        }
#endif
    }
}
void GraphicLayer::destroyObjectsEvent(EventCustom* event)
{
    if(event != NULL && event->getUserData() != NULL)
    {
        if(isKindOfClass((Ref*)event->getUserData(), CCArray))
        {
            CCArray* array = (CCArray*)event->getUserData();
            for(int i = 0; i < array->count(); i++)
            {
                if(isKindOfClass(array->objectAtIndex(i), RawObject))
                {
                    this->destroyObject((RawObject*)array->objectAtIndex(i));
                }
#if VERBOSE_WARNING
                else
                {
                    CCLOG("Warning : array contains non RawObject in destroyObjects, ignoring that value");
                }
#endif
            }
        }
#if VERBOSE_WARNING
        else
        {
            CCLOG("Warning : sent a non CCArray to destroyObjects, ignoring it");
        }
#endif
    }
}

void GraphicLayer::clear()
{
    while (storedObjects->count() > 0)
    {
        this->destroyObject((RawObject*)storedObjects->objectAtIndex(0));
    }
    storedPanels->removeAllObjects();
    childParent->removeAllObjects();
    nextAvailableId = 0;
}

RawObject* GraphicLayer::getById(int id)
{
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->getID() == id)
        {
            return obj;
        }
    }
    return NULL;
}


RawObject* GraphicLayer::firstObjectWithName(CCString* name, bool cache)
{
    return this->firstObjectWithName(name->getCString());
}

RawObject* GraphicLayer::firstObjectWithName(std::string name, bool cache)
{
    static SceneName lastScene = SceneSwitcher::sharedSwitcher()->getCurrentSceneName();
    static CCDictionary* staticCache = new CCDictionary();
    if(lastScene != SceneSwitcher::sharedSwitcher()->getCurrentSceneName())
    {
        staticCache->removeAllObjects();
        lastScene = SceneSwitcher::sharedSwitcher()->getCurrentSceneName();
    }
    else
    {
        RawObject* cachedObject = (RawObject*)staticCache->objectForKey(name);
        if(cachedObject != NULL && containsObject(cachedObject))
        {
            return cachedObject;
        }
    }
    RawObject* result = NULL;
    for(int i =  storedObjects->count() - 1; i >= 0  && result == NULL; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(name.compare(obj->getName()) == 0)
        {
            result = obj;
        }
    }
    if(cache && result != NULL)
    {
        staticCache->setObject(result, name);
    }
    return result;
}

RawObject* GraphicLayer::firstObjectWithNameInPanel(std::string name, Panel* panel)
{
    RawObject* result = NULL;
    for(int i =  storedObjects->count() - 1; i >= 0  && result == NULL; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(panel->containsObject(obj) && name == obj->getName())
        {
            result = obj;
        }
    }
    return result;
}

RawObject* GraphicLayer::firstObjectAtPosition(Vec2 position)
{
    RawObject* result = NULL;
    for(int i =  storedObjects->count() - 1; i >= 0  && result == NULL; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            result = obj;
        }
    }
    return result;
}

RawObject* GraphicLayer::firstObjectInRect(Rect rect)
{
    RawObject* result = NULL;
    for(int i =  storedObjects->count() - 1; i >= 0  && result == NULL; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        Rect realRect = rect;
        realRect.origin = this->getPositionRelativeToObject(rect.origin, obj);
        if(obj->collision(realRect))
        {
            result = obj;
        }
    }
    return result;
}

RawObject* GraphicLayer::firstObjectContainingRect(Rect rect)
{
    RawObject* result = NULL;
    for(int i =  storedObjects->count() - 1; i >= 0  && result == NULL; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        Rect realRect = rect;
        realRect.origin = this->getPositionRelativeToObject(rect.origin, obj);
        if(obj->containsRect(realRect))
        {
            result = obj;
        }
    }
    return result;
}

RawObject* GraphicLayer::objectAtIndex(int index)
{
    CCAssert(index >= 0, "in GraphicLayer objectAtIndex : invalid index, it should be positive");
    CCAssert(index < this->count(), "in GraphicLayer objectAtIndex : invalid index, it should be inferior to count");
    return (RawObject*)storedObjects->objectAtIndex(index);
}

CCArray* GraphicLayer::allObjectsWithName(CCString* name)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(name->_string == obj->getName())
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjectsWithName(std::string name)
{
    return this->allObjectsWithName(Screate(name));
}

CCArray* GraphicLayer::allObjectsWithNameInPanel(std::string name, Panel* panel)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(panel->containsObject(obj) && name == obj->getName())
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjectsStartingWithString(CCString* name)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        std::string objName = obj->getName();
        if(objName.compare(0, name->length(), name->getCString()) == 0)
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjectsStartingWithString(std::string name)
{
    return this->allObjectsStartingWithString(Screate(name));
}

CCArray* GraphicLayer::allObjectsAtPosition(Vec2 position)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjectsInRect(Rect rect)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        Rect realRect = rect;
        realRect.origin = this->getPositionRelativeToObject(rect.origin, obj);
        if(obj->collision(realRect))
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjectsContainingRect(Rect rect)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        Rect realRect = rect;
        realRect.origin = this->getPositionRelativeToObject(rect.origin, obj);
        if(obj->containsRect(realRect))
        {
            result->addObject(obj);
        }
    }
    return result;
}

CCArray* GraphicLayer::allVisibleObjectsAtPosition(Vec2 position)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->getNode() != NULL && obj->isVisible() && obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            bool parentVisible = true;
            RawObject* parent = this->getContainingPanel(obj);
            while(parent != NULL && parentVisible)
            {
                if(parent->getNode() == NULL || !parent->isVisible())
                {
                    parentVisible = false;
                }
                parent = this->getContainingPanel(parent);
            }
            if(parentVisible)
            {
                result->addObject(obj);
            }
        }
    }
    return result;
}

CCArray* GraphicLayer::allActionnableObjects()
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->getNode() != NULL && obj->isVisible() && !obj->getEventName().empty() && obj->getEventName()[0] != '\0' && obj->getEventActivated())
        {
            bool parentVisible = true;
            RawObject* parent = this->getContainingPanel(obj);
            while(parent != NULL && parentVisible)
            {
                if(parent->getNode() == NULL || !parent->isVisible())
                {
                    parentVisible = false;
                }
                parent = this->getContainingPanel(parent);
            }
            if(parentVisible)
            {
                result->addObject(obj);
            }
        }
    }
    return result;
}

CCArray* GraphicLayer::allObjects(const std::function<bool(RawObject*)>& filter)
{
    CCArray* result = CCArray::create();
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(filter(obj))
        {
            result->addObject(obj);
        }
    }
    return result;
}

bool GraphicLayer::collision(Vec2 position, RawObject* obj)
{
    return obj->collision(this->getPositionRelativeToObject(position, obj));
}

bool GraphicLayer::isOnScreen(RawObject* obj, Size size)
{
    Vec2 position = this->getRealPosition(obj);
    if(size.width == 0 && size.height == 0)
    {
        size = SizeMult(obj->getSize(), this->getRealScale(obj));
    }
    Vec2 anchorPoint = obj->getNode()->getAnchorPoint();
    Size bounds = Director::getInstance()->getOpenGLView()->getFrameSize();
    if(bounds.width > position.x - size.width * anchorPoint.x
       && 0 < position.x + size.width * (1-anchorPoint.x)
       && bounds.height > position.y - size.height * anchorPoint.y
       && 0 < position.y + size.height * (1-anchorPoint.y))
    {
        return true;
    }
    return false;
}

bool GraphicLayer::containsObject(RawObject* obj)
{
    return storedObjects->containsObject(obj);
}

CCArray* GraphicLayer::allPanelsWithName(std::string name)
{
    return allObjectsWithName(Screate(name));
}

CCArray* GraphicLayer::allPanelsWithName(CCString* name)
{
    CCArray* result = CCArray::create();
    for(int i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(name->_string == obj->getName() && storedObjects->containsObject(obj))
        {
            result->addObject(obj);
        }
    }
    return result;
}

Panel* GraphicLayer::firstPanelWithName(std::string name)
{
    return this->firstPanelWithName(Screate(name));
}

Panel* GraphicLayer::firstPanelWithName(CCString* name)
{
    Panel* result = NULL;
    for(int i =  storedPanels->count() - 1; i >= 0  && result == NULL; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(name->_string == obj->getName() && storedObjects->containsObject(obj))
        {
            result = obj;
        }
    }
    return result;
}

Panel* GraphicLayer::firstVisiblePanelWithName(CCString* name)
{
    Panel* result = NULL;
    for(int i =  storedPanels->count() - 1; i >= 0  && result == NULL; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(name->_string == obj->getName() && obj->isVisible() && storedObjects->containsObject(obj))
        {
            result = obj;
        }
    }
    return result;
}

Vec2 GraphicLayer::getPositionRelativeToObject(Vec2 point, RawObject* obj)
{
    Vec2 realPosition = point;
    RawObject* parent = this->getContainingPanel(obj);
    //construct an array of parents to traverse them in reverse order
    CCArray* parents = Acreate();
    while(parent != NULL)
    {
        parents->addObject(parent);
        parent = this->getContainingPanel(parent);
    }
    for(int i = parents->count() - 1; i >= 0; i--)
    {
        parent = (Panel*)parents->objectAtIndex(i);
        realPosition.x = (realPosition.x - parent->getPosition().x + parent->getNode()->getAnchorPoint().x * parent->getSize().width) / parent->getScale();
        realPosition.y = (realPosition.y - parent->getPosition().y + parent->getNode()->getAnchorPoint().y * parent->getSize().height) / parent->getScale();
    }
    return realPosition;
}

Vec2 GraphicLayer::getRealPosition(RawObject* obj)
{
    Vec2 realPosition = obj->getPosition();
    RawObject* parent = this->getContainingPanel(obj);
    while(parent != NULL)
    {
        realPosition.x = realPosition.x * parent->getScale() + parent->getPosition().x;
        realPosition.y = realPosition.y * parent->getScale() + parent->getPosition().y;
        parent = this->getContainingPanel(parent);
    }
    return realPosition;
}

Vec2 GraphicLayer::getCenterRealPosition(RawObject* obj)
{
    Vec2 realPosition = Vec2(obj->getPosition().x + obj->getSize().width * (0.5 - obj->getNode()->getAnchorPoint().x)  * obj->getScaleX(),
                               obj->getPosition().y + obj->getSize().height * (0.5 - obj->getNode()->getAnchorPoint().y)  * obj->getScaleY());
    RawObject* parent = this->getContainingPanel(obj);
    while(parent != NULL)
    {
        realPosition.x = (realPosition.x - parent->getNode()->getAnchorPoint().x * parent->getSize().width) * parent->getScale() + parent->getPosition().x;
        realPosition.y = (realPosition.y - parent->getNode()->getAnchorPoint().y * parent->getSize().height) * parent->getScale() + parent->getPosition().y;
        parent = this->getContainingPanel(parent);
    }
    return realPosition;
}

float GraphicLayer::getRealScale(RawObject* obj)
{
    float realScale = obj->getScale();
    RawObject* parent = this->getContainingPanel(obj);
    while(parent != NULL)
    {
        realScale *= parent->getScale();
        parent = this->getContainingPanel(parent);
    }
    return realScale;
}

float GraphicLayer::getRealScaleX(RawObject* obj)
{
    float realScale = obj->getScaleX();
    RawObject* parent = this->getContainingPanel(obj);
    while(parent != NULL)
    {
        realScale *= parent->getScaleX();
        parent = this->getContainingPanel(parent);
    }
    return realScale;
}

float GraphicLayer::getRealScaleY(RawObject* obj)
{
    float realScale = obj->getScaleY();
    RawObject* parent = this->getContainingPanel(obj);
    while(parent != NULL)
    {
        realScale *= parent->getScaleY();
        parent = this->getContainingPanel(parent);
    }
    return realScale;
}

bool GraphicLayer::touchAtPosition(Vec2 position, bool event)
{
#if VERBOSE_GENERAL_INFO
    CCLOG("Before trying touchAtPosition, obj order :");
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        CCLOG("obj name: %s", obj->getName().c_str());
    }
#endif
    for(int i =  storedObjects->count() - 1; i >= 0; i--)
    {
        if(!isKindOfClass(storedObjects->objectAtIndex(i), RawObject))
        {
            CCLOG("Problem with object at index %d, not a valid RawObject", i);
        }
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        Node* node = obj->getNode();
        if(node != NULL && node->isVisible() && obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            bool parentVisible = true;
            RawObject* parent = this->getContainingPanel(obj);
            while(parent != NULL && parentVisible)
            {
                if(parent->getNode() == NULL || !parent->isVisible())
                {
                    parentVisible = false;
                }
                parent = this->getContainingPanel(parent);
            }
            if(parentVisible)
            {
                if(this->touchObject(obj, event, position))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool GraphicLayer::touchObject(RawObject* obj, bool event, Vec2 position)
{
    //TODO : check opacity ?
    if(!obj->getEventName().empty() && obj->getEventName()[0] != '\0' && obj->getEventActivated())
    {
        if(event)
        {
            CCDictionary* infos = obj->getEventInfos();
            infos->setObject(Pcreate(position), "TouchPosition");
            IFEXIST(tapObserver)->onButtonTapped(obj, obj->getEventName(), infos);
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(obj->getEventName(), infos);
            CCString* trackingName = (CCString*)obj->getEventInfos()->objectForKey("TrackingName");
            SceneName currentScene = SceneSwitcher::sharedSwitcher()->getCurrentSceneName();
            //In VideoView, buttons are only tracked if the video meet the minimum duration
            if(trackingName != NULL)
            {
                CCString* trackingInfo = (CCString*) obj->getEventInfos()->objectForKey("TrackingInfo");
                if(trackingInfo != NULL)
                {
                    trackingInfo = (CCString*) obj->getEventInfos()->objectForKey(trackingInfo->getCString());
                }
                CCString* trackingLabel = (CCString*) obj->getEventInfos()->objectForKey("TrackingLabel");
                AnalyticsWrapper::logEvent(trackingName->_string, trackingInfo != NULL ? trackingInfo->getCString() : trackingLabel != NULL ? trackingLabel->_string : "");
            }
        }
        else if(!obj->getHelp().empty() && obj->getHelp()[0] != '\0')
        {
            CCDictionary* helpInfos = CCDictionary::create();
            helpInfos->setObject(Icreate(obj->getID()), "Sender");
            helpInfos->setObject(Screate(obj->getHelp()), "RequestedHelp");
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("DisplayHelp", helpInfos);
        }
        return true;
    }
    return false;
}

void GraphicLayer::reorderChild(RawObject* child, int zOrder)
{
    if(this->containsObject(child) && child->getNode() != NULL)
    {
        Panel* parent = this->getContainingPanel(child);
        child->retain();
        if(parent == NULL)
        {
            layer->reorderChild(child->getNode(), zOrder);
        }
        else
        {
            parent->reorderChild(child, zOrder);
        }
        storedObjects->removeObject(child);
        this->addObject(child, zOrder);
        child->release();
        if(isKindOfClass(child, Panel))
        {
            this->reorderChildrenOfPanel((Panel*)child);
        }
    }
}

void GraphicLayer::reorderChildAfter(RawObject* child, RawObject* otherChild)
{
}

void GraphicLayer::reorderChildBefore(RawObject* child, RawObject* otherChild)
{
}

void GraphicLayer::reorderChildrenOfPanel(Panel* panel)
{
    CCArray* alreadyReordered = Acreate();
    for(int i = 0; i < storedObjects->count(); i++)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(this->getContainingPanel(obj) == panel && !alreadyReordered->containsObject(obj))
        {
            //re-insert child at a specific position as children need to be ordered by zIndex
            //DON'T use [self reorderChild:beforeChild] as there is a special behavior for childs inside panel (here, only the order in storedObjects should be changed)
            storedObjects->removeObject(obj);
            storedObjects->insertObject(obj, storedObjects->indexOfObject(panel));
            alreadyReordered->addObject(obj);
            //redo this index as the object is not the same.
            i--;
        }
    }
    for(int i = 0; i < storedPanels->count(); i++)
    {
        Panel* subPanel = (Panel*)storedPanels->objectAtIndex(i);
        if(this->getContainingPanel(subPanel) == panel)
        {
            this->reorderChildrenOfPanel(subPanel);
        }
    }
}
/* back up version of the method : scale working properly, position not so much
 void GraphicLayer::applyDisplayScaling(RawObject* obj, DisplayScaling options)
 {
 Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
 float shinzuScale = 1;
 float scaleFactor = Director::getInstance()->getContentScaleFactor();
 Size designSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
 Size usedSize;
 usedSize.width = designSize.width * scaleFactor * shinzuScale;
 usedSize.height = designSize.height * scaleFactor * shinzuScale;
 Vec2 position = obj->getPosition();
 if(options & AnchorLeft && position.x < designSize.width/2)
 {
 position.x -= (frameSize.width - usedSize.width)/2 / (scaleFactor*shinzuScale);
 }
 if(options & AnchorRight && position.x > designSize.width/2)
 {
 position.x += (frameSize.width - usedSize.width)/2 / (scaleFactor*shinzuScale);
 }
 if(options & AnchorTop && position.y > designSize.height/2)
 {
 position.y += (frameSize.height - usedSize.height)/2 / (scaleFactor*shinzuScale);
 }
 if(options & AnchorBottom && position.y < designSize.height/2)
 {
 position.y -= (frameSize.height - usedSize.height)/2 / (scaleFactor*shinzuScale);
 }
 obj->setPosition(position);
 float scale = obj->getScale();
 if(options & FitWidth)
 {
 float newScale = (obj->getSize().width * obj->getScale() + (frameSize.width - usedSize.width))/obj->getSize().width;
 scale = MAX(scale, newScale);
 }
 if(options & FitHeight)
 {
 float newScale = (obj->getSize().height * obj->getScale() + (frameSize.height - usedSize.height))/obj->getSize().height;
 scale = MAX(scale, newScale);
 }
 obj->setScale(scale);
 }*/

/*
 float GraphicLayer::closeMainPanels()
 {
 return 0;
 }*/

void GraphicLayer::addObject(RawObject* obj, int z)
{
    if(obj != NULL)
    {
        if(isUpdating)
        {
            objectsToAdd->addObject(CCArray::create(obj, Icreate(z), NULL));
        }
        else
        {
            if(isKindOfClass(obj, Panel) && !storedPanels->containsObject(obj))
            {
                storedPanels->addObject(obj);
            }
            //insert objects at a specific position as they need to be ordered by zOrder
            unsigned int index = 0;
            //search for the right position : use z instead of obj.zOrder, because obj.zOrder is not set yet.
            //TODO : improve this mess : return an array of z and compare them in order ...
            while(index < storedObjects->count() &&
                  (( this->getContainingPanel((RawObject*)storedObjects->objectAtIndex(index)) != NULL
                    && this->getContainingPanel((RawObject*)storedObjects->objectAtIndex(index))->getZOrder() <= z)
                   || (this->getContainingPanel((RawObject*)storedObjects->objectAtIndex(index)) == NULL
                       && ((RawObject*)storedObjects->objectAtIndex(index))->getZOrder() <= z)))
            {
                index++;
            }
            storedObjects->insertObject(obj, index);
            if(obj->getNode() != NULL && obj->getNode()->getParent() == NULL)
            {
                layer->addChild(obj->getNode(), z);
            }
#if VERBOSE_WARNING
            else if(obj->getNode() == NULL)
            {
                CCLOG("Warning : Child %s doesn't have a Node, it will not be displayed by cocos2d", obj->getName().c_str());
            }
#endif
        }
    }
#if VERBOSE_WARNING
    else
    {
        CCLOG("Warning : trying to add a nil object, check if it has been created correctly (in particular if the resource exists)");
    }
#endif
}

void GraphicLayer::update(float deltaTime)
{
    isUpdating = true;
    Ref* obj;
    CCARRAY_FOREACH(storedObjects, obj)
    {
        if(isKindOfClass(obj, RawObject))
        {
            ((RawObject*)obj)->update(deltaTime);
        }
        else
        {
            CCLOG("Warning: wrong object type in storedObjects");
        }
    }
    isUpdating = false;
    
    CCARRAY_FOREACH(objectsToAdd, obj)
    {
        CCArray* objInfos = (CCArray*)obj;
        this->addObject((RawObject*)objInfos->objectAtIndex(0), ((CCInteger*)objInfos->objectAtIndex(1))->getValue());
        if(objInfos->count() > 2)
        {
            this->placeObject((RawObject*)objInfos->objectAtIndex(0), (Panel*)objInfos->objectAtIndex(2));
        }
    }
    objectsToAdd->removeAllObjects();
    
    CCARRAY_FOREACH(objectsToAdd, obj)
    {
        this->destroyObject((RawObject*)obj);
    }
    objectsToRemove->removeAllObjects();
    
    clock += deltaTime;
}

void GraphicLayer::refreshRenderTextures(Ref* obj)
{
    for(int i = 0; i < storedObjects->count(); i++)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(isKindOfClass(obj, CustomObject) && isKindOfClass(((CustomObject*)obj)->getNode(), RenderTexture))
        {
            CustomObject* custObj = ((CustomObject*)obj);
            RenderTexture* renderText = (RenderTexture*)custObj->getNode();
            RenderTexture* newText = RenderTexture::create(renderText->getSprite()->getContentSize().width, renderText->getSprite()->getContentSize().height);
            custObj->setNode(newText);
        }
    }
}

void GraphicLayer::loadBaseNodeAttributes(CustomBaseNode* node, RawObject* obj)
{
    //Always check for NULL since node is the result of a dynamic cast
    if(node != NULL)
    {
        if(node->getName() != NULL)
        {
#if VERBOSE_LOAD_CCB
            CCLOG("setting name : %s", node->getName()->getCString());
#endif
            obj->setName(node->getName()->getCString());
        }
        if(node->getEventName() != NULL)
        {
#if VERBOSE_LOAD_CCB
            CCLOG("setting event name : %s", node->getEventName()->getCString());
#endif
            obj->setEventName(node->getEventName()->getCString());
        }
        if(node->getScene() != 0)
        {
#if VERBOSE_LOAD_CCB
            CCLOG("setting scene : %d", node->getScene());
#endif
            obj->setEventInfo(Icreate(node->getScene()), "Scene");
        }
        if(node->getZindex() != 0)
        {
            this->reorderChild(obj, node->getZindex());
        }
        obj->addEventInfos(node->getParameters());
    }
}


int GraphicLayer::getNextId()
{
    nextAvailableId++;
    return nextAvailableId - 1;
}
NS_FENNEX_END
