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
    storedObjects = new CCArray();
    storedPanels = new CCArray();
    layer = Layer::create();
    layer->retain();
    clock = 0;
    isUpdating = false;
}

GraphicLayer::~GraphicLayer()
{
#if VERBOSE_DEALLOC
    log("Layer dealloc");
#endif
    this->clear();
    s_SharedLayer = NULL;
    storedObjects->release();
    storedPanels->release();
    layer->release();
}

void GraphicLayer::useBaseLayer(Layer* otherLayer)
{
    Node* parent = layer->getParent();
    parent->addChild(otherLayer);
    parent->removeChild(layer, true);
    //Ensure the layer is properly released: actually, only the first one (at launch) will have a retain count of 2 here
    if(layer->getReferenceCount() == 2)
    {
        layer->release();
    }
    layer = otherLayer;
    layer->retain();
}

void GraphicLayer::renderOnLayer(Scene* destination)
{
    relatedScene = destination;
    //Ensure that the layer is not on another Scene
    if(layer->getParent() != NULL)
    {
        layer->removeFromParentAndCleanup(true);
    }
    storedObjects->removeAllObjects();
    destination->addChild(layer);
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

Vec2 getPos(ValueMap values)
{
    Vec2 pos = Vec2(0, 0);
    if(values.find("X") != values.end() && values.at("X").getType() == Value::Type::FLOAT)
    {
        pos.x = values.at("X").asFloat();
    }
    if(values.find("Y") != values.end() && values.at("Y").getType() == Value::Type::FLOAT)
    {
        pos.y = values.at("Y").asFloat();
    }
    return pos;
}

int getZindex(ValueMap values)
{
    if(values.find("Zindex") != values.end() && values.at("Zindex").getType() == Value::Type::INTEGER)
    {
        return values.at("Zindex").asInt();
    }
    return 0;
}

Image* GraphicLayer::createImage(std::string imageFile, ValueMap values)
{
#if VERBOSE_LOAD_CCB
    log("Creating Image %s", imageFile.c_str());
#endif
    Image* obj = new Image(imageFile, getPos(values));
    if(obj != NULL)
    {
        this->addObject(obj, getZindex(values));
        obj->release();
        setObjectFields(obj, values);
    }
    else
    {
#if VERBOSE_WARNING
        log("Warning : error creating Image with file %s, perhaps the file name is incorrect or the file is too large", imageFile.c_str());
#endif
    }
#if VERBOSE_LOAD_CCB
    log("Ended creating Image");
#endif
    return obj;
}

Image* GraphicLayer::createAnimatedImage(std::string spriteSheetFile, int capacity, ValueMap values)
{
#if VERBOSE_LOAD_CCB
    log("Creating animated Image %s", spriteSheetFile.c_str());
#endif
    Image* obj = new Image(spriteSheetFile, getPos(values), capacity);
    if(obj != NULL)
    {
        this->addObject(obj, getZindex(values));
        obj->release();
        setObjectFields(obj, values);
    }
    else
    {
#if VERBOSE_WARNING
        log("Warning : error creating animated Image with file %s, perhaps the file name is incorrect or the file is too large", spriteSheetFile.c_str());
#endif
    }
#if VERBOSE_LOAD_CCB
    log("Ended creating animated Image");
#endif
    return obj;
}

Image* GraphicLayer::createImageFromSprite(Sprite* sprite, Panel* parent)
{
    Image* obj = new Image(sprite);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(sprite), obj);
    return obj;
}

CustomObject* GraphicLayer::createCustomObject(Node* delegate, ValueMap values)
{
#if VERBOSE_LOAD_CCB
    log("Creating CustomObject");
#endif
    CustomObject* obj = new CustomObject(delegate, getPos(values));
    if(obj != NULL)
    {
        this->addObject(obj, getZindex(values));
        obj->release();
        setObjectFields(obj, values);
    }
    else
    {
#if VERBOSE_WARNING
        log("Warning : error creating CustomObject");
#endif
    }
#if VERBOSE_LOAD_CCB
    log("Ended creating CustomObject");
#endif
    return obj;
}

CustomObject* GraphicLayer::createCustomObjectFromNode(Node* node, Panel* parent)
{
    CustomObject* obj = new CustomObject(node);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(node), obj);
    return obj;
}

Size getDimensions(ValueMap values)
{
    if(values.find("DimX") != values.end() && values.at("DimX").getType() == Value::Type::FLOAT &&
       values.find("DimY") != values.end() && values.at("DimY").getType() == Value::Type::FLOAT)
    {
        return Size(values.at("DimX").asFloat(), values.at("DimY").asFloat());
    }
    return Size(0, 0);
}

TextHAlignment getAlignment(ValueMap values)
{
    if(values.find("TextFormat") != values.end() && values.at("TextFormat").getType() == Value::Type::INTEGER)
    {
        return (TextHAlignment)values.at("TextFormat").asInt();
    }
    return TextHAlignment::CENTER;
}

LabelTTF* GraphicLayer::createLabelTTF(std::string label, std::string fontFile, ValueMap values)
{
#if VERBOSE_LOAD_CCB
    log("Creating LabelTTF %s", label.c_str());
#endif
    LabelTTF* obj = new LabelTTF(label, fontFile, getPos(values), getDimensions(values), getAlignment(values));
    if(obj != NULL)
    {
        this->addObject(obj, getZindex(values));
        obj->release();
        setObjectFields(obj, values);
    }
    else
    {
#if VERBOSE_WARNING
        log("Warning : error creating LabelTTF %s with font %s, perhaps the font is not properly added to project", label.c_str(), fontFile.c_str());
#endif
    }
#if VERBOSE_LOAD_CCB
    log("Ended creating LabelTTF");
#endif
    return obj;
}

LabelTTF* GraphicLayer::createLabelTTFromLabel(Label* cocosLabel, Panel* parent)
{
    LabelTTF* obj = new LabelTTF(cocosLabel);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosLabel), obj);
    return obj;
}

InputLabel* GraphicLayer::createInputLabelFromScale9Sprite(ui::Scale9Sprite* cocosSprite, Panel* parent)
{
    InputLabel* obj = new InputLabel(cocosSprite);
    if(obj != NULL)
    {
        //TODO : find a way to add the label at the right place in cocos hierarchy
        if(parent != NULL)
        {
            storedObjects->addObject(obj);
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        else
        {
            this->addObject(obj);
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosSprite), obj);
    
    return obj;
}

Panel* GraphicLayer::createPanel(std::string name, ValueMap values)
{
    
#if VERBOSE_LOAD_CCB
    log("Creating Panel %s", name.c_str());
#endif
    Panel* obj = new Panel(name, getPos(values));
    if(obj != NULL)
    {
        this->addObject(obj, getZindex(values));
        obj->release();
        setObjectFields(obj, values);
    }
    else
    {
#if VERBOSE_WARNING
        log("Warning : error creating Panel %s", name.c_str());
#endif
    }
#if VERBOSE_LOAD_CCB
    log("Ended creating Panel");
#endif
    return obj;
}

Panel* GraphicLayer::createPanelFromNode(Node* cocosNode, Panel* parent)
{
    Panel* obj = new Panel(cocosNode);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        storedPanels->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(cocosNode), obj);
    loadNodeToFenneX(obj->getNode(), obj);
    return obj;
}

Panel* GraphicLayer::createPanelWithNode(std::string name, Node* panelNode, int zOrder)
{
    Panel* obj = new Panel(panelNode, name);
    if(obj != NULL)
    {
        this->addObject(obj, zOrder);
        obj->release();
    }
    return obj;
}

DropDownList* GraphicLayer::createDropDownListFromSprite(Sprite* sprite, Panel* parent)
{
    DropDownList* obj = new DropDownList(sprite);
    if(obj != NULL)
    {
        storedObjects->addObject(obj);
        if(parent != NULL)
        {
            parent->addChild(obj);
            childsParents[obj->getID()] = parent;
        }
        obj->release();
    }
    this->loadBaseNodeAttributes(dynamic_cast<CustomBaseNode*>(sprite), obj);
    return obj;
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
            log("Warning: animated Image not supported for duplicate yet");
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
        log("Warning: object type not supported for duplicate yet");
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
        for(RawObject* otherChild : ((Panel*)otherObject)->getChildren())
        {
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
                childsParents[obj->getID()] = panel;
            }
#if VERBOSE_WARNING
            else
            {
                log("Warning : child %s doesn't have a Node, you shouldn't try to place it on a panel", obj->getName().c_str());
            }
#endif
        }
    }
    else if(panel != NULL)
    {
        for(long i = 0; i < objectsToAdd.size(); i++)
        {
            if(objectsToAdd.at(i) == obj)
            {
                objectsToAddPanel[i] = panel;
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
        CCAssert(childsParents.find(obj->getID()) != childsParents.end(), "Cannot find object inf childsParents");
        childsParents.erase(obj->getID());
    }
#if VERBOSE_WARNING
    else
    {
        log("Warning : child %s doesn't have a Node, you shouldn't try to remove it from a panel", obj->getName().c_str());
    }
#endif
}

void GraphicLayer::removeAllObjectsFromPanel(Panel* panel)
{
    while(panel->getChildren().size() > 0)
    {
        this->removeObjectFromPanel(panel->getChildren().at(0), panel);
    }
}

Panel* GraphicLayer::getContainingPanel(RawObject* obj)
{
    if(obj == NULL) return NULL;
    if(childsParents.find(obj->getID()) == childsParents.end())
    {
        return NULL;
    }
    return childsParents[obj->getID()];
}

void GraphicLayer::destroyObject(RawObject* obj)
{
    if(obj != NULL && storedObjects->containsObject(obj))
    {
        if(isUpdating)
        {
            objectsToRemove.pushBack(obj);
        }
        else
        {
            if(isKindOfClass(obj, Panel))
            {
                //we can't use copy anymore, but clone doesn't work with RawObjects. Manually clone instead
                Vector<RawObject*> childrenCopy;
                for(RawObject* child : ((Panel*)obj)->getChildren())
                {
                    childrenCopy.pushBack(child);
                }
                this->destroyObjects(childrenCopy);
                storedPanels->removeObject(obj);
                CCASSERT(((Panel*)obj)->getChildren().size() == 0, "Problem with panel children when releasing panel");
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
        log("Warning : trying to destroy not valid object at adress %p", obj);
#endif
        if(obj != NULL && storedPanels->containsObject(obj))
        {
            storedPanels->removeObject(obj);
        }
    }
}

void GraphicLayer::destroyObjects(Vector<RawObject*> array)
{
    for(auto obj : array)
    {
        this->destroyObject(obj);
    }
}

void GraphicLayer::destroyObjects(Vector<Panel*> array)
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
            log("Warning : sent a non RawObject to destroyObject, ignoring it");
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
            for(long i = 0; i < array->count(); i++)
            {
                if(isKindOfClass(array->objectAtIndex(i), RawObject))
                {
                    this->destroyObject((RawObject*)array->objectAtIndex(i));
                }
#if VERBOSE_WARNING
                else
                {
                    log("Warning : array contains non RawObject in destroyObjects, ignoring that value");
                }
#endif
            }
        }
#if VERBOSE_WARNING
        else
        {
            log("Warning : sent a non CCArray to destroyObjects, ignoring it");
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
    childsParents.clear();
    objectsToRemove.clear();
    nextAvailableId = 0;
}

RawObject* GraphicLayer::first(int id)
{
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->getID() == id)
        {
            return obj;
        }
    }
    return NULL;
}

RawObject* GraphicLayer::first(std::string name, bool cache)
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
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(name.compare(obj->getName()) == 0)
        {
            if(cache)
            {
                staticCache->setObject(obj, name);
            }
            return obj;
        }
    }
    return NULL;
}

RawObject* GraphicLayer::first(std::string name, Panel* panel)
{
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(panel->containsObject(obj) && name == obj->getName())
        {
            return obj;
        }
    }
    return NULL;
}

RawObject* GraphicLayer::first(Vec2 position)
{
    for(long i =  storedObjects->count() - 1; i >= 0 ; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            return obj;
        }
    }
    return NULL;
}

RawObject* GraphicLayer::at(int index)
{
    CCAssert(index >= 0, "in GraphicLayer objectAtIndex : invalid index, it should be positive");
    CCAssert(index < storedObjects->count(), "in GraphicLayer objectAtIndex : invalid index, it should be inferior to count");
    return (RawObject*)storedObjects->objectAtIndex(index);
}

Vector<RawObject*> GraphicLayer::all(std::string name)
{
    Vector<RawObject*> result;
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(name == obj->getName())
        {
            result.pushBack(obj);
        }
    }
    return result;
}

Vector<RawObject*> GraphicLayer::all(std::string name, Panel* panel)
{
    Vector<RawObject*> result;
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(panel->containsObject(obj) && name == obj->getName())
        {
            result.pushBack(obj);
        }
    }
    return result;
}

Vector<RawObject*> GraphicLayer::all(Vec2 position)
{
    Vector<RawObject*> result;
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(obj->collision(this->getPositionRelativeToObject(position, obj)))
        {
            result.pushBack(obj);
        }
    }
    return result;
}

RawObject* GraphicLayer::first(const std::function<bool(RawObject*)>& filter)
{
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(filter(obj))
        {
            return obj;
        }
    }
    return NULL;
}

Panel* GraphicLayer::firstPanel(const std::function<bool(Panel*)>& filter)
{
    for(long i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(filter(obj))
        {
            return obj;
        }
    }
    return NULL;
}

Vector<RawObject*> GraphicLayer::all(const std::function<bool(RawObject*)>& filter)
{
    Vector<RawObject*> result;
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        if(filter(obj))
        {
            result.pushBack(obj);
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

bool GraphicLayer::isInFront(RawObject* obj1, RawObject* obj2)
{
    return storedObjects->indexOfObject(obj1) < storedObjects->indexOfObject(obj2);
}

bool GraphicLayer::containsObject(RawObject* obj)
{
    return storedObjects->containsObject(obj);
}

Vector<Panel*> GraphicLayer::allPanels(std::string name)
{
    Vector<Panel*> result;
    for(long i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(name == obj->getName() && storedObjects->containsObject(obj))
        {
            result.pushBack(obj);
        }
    }
    return result;
}

Vector<Panel*> GraphicLayer::allPanels(std::string name, Panel* panel)
{
    Vector<Panel*> result;
    for(long i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(panel->containsObject(obj) && name == obj->getName())
        {
            result.pushBack(obj);
        }
    }
    return result;
}

Vector<Panel*> GraphicLayer::allPanels(const std::function<bool(Panel*)>& filter)
{
    Vector<Panel*> result;
    for(long i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(filter(obj))
        {
            result.pushBack(obj);
        }
    }
    return result;
}

Panel* GraphicLayer::firstPanel(std::string name)
{
    for(long i =  storedPanels->count() - 1; i >= 0; i--)
    {
        Panel* obj = (Panel*)storedPanels->objectAtIndex(i);
        if(name == obj->getName() && storedObjects->containsObject(obj))
        {
            return obj;
        }
    }
    return NULL;
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
    for(long i = parents->count() - 1; i >= 0; i--)
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


bool GraphicLayer::isWorldVisible(RawObject* obj)
{
    if(!obj->isVisible())
    {
        return false;
    }
    RawObject* parent = GraphicLayer::sharedLayer()->getContainingPanel(obj);
    while(parent != NULL)
    {
        if(parent->getNode() == NULL || !parent->isVisible())
        {
            return false;
        }
        parent = GraphicLayer::sharedLayer()->getContainingPanel(parent);
    }
    return true;
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
    log("Before trying touchAtPosition, obj order :");
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        RawObject* obj = (RawObject*)storedObjects->objectAtIndex(i);
        log("obj name: %s", obj->getName().c_str());
    }
#endif
    for(long i =  storedObjects->count() - 1; i >= 0; i--)
    {
        if(!isKindOfClass(storedObjects->objectAtIndex(i), RawObject))
        {
            log("Problem with object at index %d, not a valid RawObject", i);
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
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(obj->getEventName(), infos);
            CCString* trackingName = (CCString*)obj->getEventInfos()->objectForKey("TrackingName");
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

void GraphicLayer::reorderChildrenOfPanel(Panel* panel)
{
    CCArray* alreadyReordered = Acreate();
    for(long i = 0; i < storedObjects->count(); i++)
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
    for(long i = 0; i < storedPanels->count(); i++)
    {
        Panel* subPanel = (Panel*)storedPanels->objectAtIndex(i);
        if(this->getContainingPanel(subPanel) == panel)
        {
            this->reorderChildrenOfPanel(subPanel);
        }
    }
}

void GraphicLayer::addObject(RawObject* obj, int z)
{
    if(obj != NULL)
    {
        if(isUpdating)
        {
            objectsToAdd.pushBack(obj);
            objectsToAddZindex.push_back(z);
            objectsToAddPanel.push_back(NULL);
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
                log("Warning : Child %s doesn't have a Node, it will not be displayed by cocos2d", obj->getName().c_str());
            }
#endif
        }
    }
#if VERBOSE_WARNING
    else
    {
        log("Warning : trying to add a nil object, check if it has been created correctly (in particular if the resource exists)");
    }
#endif
}

void GraphicLayer::setObjectFields(RawObject* obj, ValueMap values)
{
    if(values.find("Name") != values.end() && values.at("Name").getType() == Value::Type::STRING)
    {
        obj->setName(values.at("Name").asString());
    }
    if(values.find("EventName") != values.end() && values.at("EventName").getType() == Value::Type::STRING)
    {
        obj->setEventName(values.at("EventName").asString());
    }
    if(values.find("Scale") != values.end() && values.at("Scale").getType() == Value::Type::FLOAT)
    {
        obj->setScale(values.at("Scale").asFloat());
    }
    if(values.find("EventInfos") != values.end() && values.at("EventInfos").getType() == Value::Type::MAP)
    {
        obj->addEventInfos(values.at("EventInfos").asValueMap());
    }
    if(values.find("Panel") != values.end() && values.at("Panel").getType() == Value::Type::INTEGER)
    {
#if VERBOSE_WARNING
        if(values.find("Zindex") != values.end() && values.at("Zindex").getType() == Value::Type::INTEGER)
        {
            log("Warning : try to add image in a Panel with a Zindex : currently, depth is not supported in panels");
        }
#endif
        RawObject* target = first(values.at("Panel").asInt());
        CCAssert(isKindOfClass(target, Panel), "Trying to place on object on another object which is not a Panel");
        this->placeObject(obj, (Panel*)target);
    }
    if(values.find("Visible") != values.end() && values.at("Visible").getType() == Value::Type::BOOLEAN)
    {
        obj->setVisible(values.at("Visible").asBool());
    }
    if(values.find("Opacity") != values.end() && values.at("Opacity").getType() == Value::Type::INTEGER)
    {
        int opacity = values.at("Opacity").asInt();
        CCAssert(opacity >= 0 && opacity <= 255, "Invalid opacity value, should range between 0 to 255");
        obj->getNode()->setOpacity(opacity);
    }
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
            log("Warning: wrong object type in storedObjects");
        }
    }
    isUpdating = false;
    for(long i = 0; i < objectsToAdd.size(); i++)
    {
        this->addObject(objectsToAdd.at(i), objectsToAddZindex[i]);
        if(objectsToAddPanel[i] != NULL)
        {
            this->placeObject(objectsToAdd.at(i), objectsToAddPanel[i]);
        }
    }
    objectsToAdd.clear();
    objectsToAddZindex.clear();
    objectsToAddPanel.clear();
    
    for(RawObject* obj : objectsToRemove)
    {
        this->destroyObject(obj);
        
    }
    objectsToRemove.clear();
    
    clock += deltaTime;
}

void GraphicLayer::refreshRenderTextures(Ref* obj)
{
    for(long i = 0; i < storedObjects->count(); i++)
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

Vector<RawObject*> GraphicLayer::sortObjects(Vector<RawObject*> array)
{
    std::sort(array.begin(),
              array.end(),
              [](const void* a, const void* b) { return *((RawObject*)a) < *((RawObject*)b); });
    return array;
}

Vector<Panel*> GraphicLayer::sortObjects(Vector<Panel*> array)
{
    std::sort(array.begin(),
              array.end(),
              [](const void* a, const void* b) { return *((Panel*)a) < *((Panel*)b); });
    return array;
}

void GraphicLayer::loadBaseNodeAttributes(CustomBaseNode* node, RawObject* obj)
{
    //Always check for NULL since node is the result of a dynamic cast
    if(node != NULL)
    {
        if(!node->getName().empty())
        {
#if VERBOSE_LOAD_CCB
            log("setting name : %s", node->getName().c_str());
#endif
            obj->setName(node->getName());
        }
        if(!node->getEventName().empty())
        {
#if VERBOSE_LOAD_CCB
            log("setting event name : %s", node->getEventName().c_str());
#endif
            obj->setEventName(node->getEventName());
        }
        if(node->getScene() != 0)
        {
#if VERBOSE_LOAD_CCB
            log("setting scene : %d", node->getScene());
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
