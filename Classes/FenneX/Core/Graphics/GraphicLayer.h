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

#ifndef __FenneX__GraphicLayer__
#define __FenneX__GraphicLayer__

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "Pausable.h"
#include "Scene.h"
#include "RawObject.h"
#include "CustomObject.h"
#include "Image.h"
#include "LabelTTF.h"
#include "Panel.h"
#include "InputLabel.h"
#include "CustomBaseNode.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
class GraphicLayer : public CCObject, public Pausable
{
public:
    static GraphicLayer* sharedLayer();
    
    CCArray* getMainPanels() { return mainPanels; }
    CCLayer* getLayer() { return layer; }
    int getDepthInScene() { return depthInScene; }
    bool IsPaused() { return isPaused; }
    float getClock() { return clock; }
    CCArray* getChildren() { return storedObjects; }
    
public:
    ~GraphicLayer();
    
    //Used when loading an entire scene from CCB
    void useBaseLayer(CCLayer* otherLayer);
    //Specify on which layer (or scene) it should be rendered and its depth
    void renderOnLayer(Scene* destination, int depth);
    
    //Should be called to detach it before being render on another layer
    void stopRenderOnLayer(Scene* destination, bool cleanup);
    virtual void stop();
    
    //create an object according parameters Type (CCString => Label or Image)
    //required parameters are the same as this object, see below
    //TODO : implement it if some automatic creation is needed
    //RawObject* createObject(CCObject* firstObject, ... );
    //RawObject* createObject(CCDictionary* values);
    
    /*Parameters must contain ImageFile (CCString) or TODO ??ImageData (casted CGImageRef)?? + cocosName (CCString) : each cocosName MUST be unique and MUST NOT correspond to an image file in use
     Optional Parameters :
     - Position (CCPoint) default (0, 0)
     - Zindex (CCInteger as int) default 0
     - Name (CCString) default ImageFile or cocosName
     - EventName (CCString) default nil
     - Panel (Panel) default nil
     - EventInfos (CCDictionary) default nothing
     - Visible (set CCInteger(0) for invisible) default visible
     */
    Image* createImage(CCObject* firstObject, ... );
    Image* createImage(CCDictionary* values);
    Image* createImageFromCCSprite(CCSprite* sprite, Panel* parent);
    
    /*Parameters must contain Delegate (CCNode)
     Optional Parameters :
     - Position (CCPoint) default (0, 0)
     - Zindex (CCInteger as int) default 0
     - Name (CCString) default ImageFile or cocosName
     - EventName (CCString) default nil
     - Panel (Panel) default nil
     - EventInfos (CCDictionary) default nothing
     - Visible (set CCInteger(0) for invisible) default visible
     */
    CustomObject* createCustomObject(CCObject* firstObject, ... );
    CustomObject* createCustomObject(CCDictionary* values);
    CustomObject* createCustomObjectFromCCNode(CCNode* node, Panel* parent);
    
    /*Label vs LabelTTF ?
     see cocos2d documentation about CCLabelBMFont vs Label
     in short, use Label when you are sure about the characters needed (no user generated content) to have better performance (with a slight loading time)
     use LabelTTF to have every sinle character available. May have a performance drop especially if you change the labels a lot
     */
    
    /*Parameters must contain Label (CCString) and FontFile (CCString) : FontFile must be formatted as FontnameSizeColor (example : Verdana30Black, recognized colors are : Black, White, Gray)
     Optional Parameters :
     - Position (CCPoint) default (0, 0)
     - Dimensions (CCSize) and TextFormat (CCInteger as enum TextFormat) together or just Dimensions default NO_MAX_LINE_WIDTH and AlignCenter  DO NOT USE SCALE IF YOU WANT DIMENSIONS TO HAVE AN EFFECT
     - Zindex (CCInteger as int) default 0
     - Name (CCString) default ImageFile or cocosName
     - EventName (CCString) default nil
     - Panel (Panel) default nil
     - EventInfos (CCDictionary) default nothing
     - Visible (set CCInteger(0) for invisible) default visible
     */
    LabelTTF* createLabelTTF(CCObject* firstObject, ... );
    LabelTTF* createLabelTTF(CCDictionary* values);
    LabelTTF* createLabelTTFromLabel(Label* cocosLabel, Panel* parent);
    
    /*Parameters must contain PlaceHolder (CCString), which is a placeholder, FontFile (CCString) and FontSize (CCInteger)
     Optional Parameters :
     - KeyboardType (CCInteger as EditBoxInputMode) default kEditBoxInputModeAny
     - MaxDigits (CCInteger as int) default 0
     - Label (CCString) default empty
     - Position (CCPoint) default (0, 0)
     - Dimensions (CCSize) and TextFormat (CCInteger as enum TextFormat) together or just Dimensions default NO_MAX_LINE_WIDTH and AlignCenter
     - Zindex (CCInteger as int) default 0
     - Name (CCString) default ImageFile or cocosName
     - EventName (CCString) default OpenKeyboard
     - Panel (Panel) default nil
     - EventInfos (CCDictionary) default nothing
     - Visible (set CCInteger(0) for invisible) default visible
     */
    InputLabel* createInputLabel(CCObject* firstObject, ... );
    InputLabel* createInputLabel(CCDictionary* values);
    InputLabel* createInputLabelFromScale9Sprite(Scale9Sprite* cocosSprite, Panel* parent);
    
    /*Parameters must contain Name (CCString)
     Optional Parameters :
     - Position (CCPoint) default (0, 0)
     - Zindex (CCString as int) default 0
     - EventName (CCString) default nil
     - Panel (Panel) default nil
     - EventInfos (CCDictionary) default nothing
     - Visible (set CCString(0) for invisible) default visible
     */
    Panel* createPanel(CCObject* firstObject, ... );
    Panel* createPanel(CCDictionary* values);
    Panel* createPanelFromCCNode(CCNode* cocosNode, Panel* parent);
    //Special case : when you want the panel to be the host for a .ccbi file, the CCNode is already created
    Panel* createPanelWithNode(const char* name, CCNode* panelNode, int zOrder = 0);
    
    //Duplicate an object, currently supported : Image (not animated), LabelTTF, Panel (including its children, by recursive call)
    //Apart from specific type infos, copied properties are : Position, Zindex, Panel, Name, EventName, EventInfos, Visible, Scale
    RawObject* duplicateObject(RawObject* otherObject);
    //Leave panel at NULL to remove it from any panel it may be on
    RawObject* placeObject(RawObject* obj, Panel* panel = NULL);
    
    //Remove will put objects from panel on the manager, use destroy if you want to destroy them
    void removeObjectFromPanel(RawObject* obj, Panel* panel);
    void removeAllObjectsFromPanel(Panel* panel);
    
    Panel* getContainingPanel(RawObject* obj);
    
    //Destroy an object of the manager
    //It is not possible to remove an object without destroying it, as only the manager should store RawObject references
    void destroyObject(RawObject* obj);
    //Destroy an array of objects.
    CC_DEPRECATED_ATTRIBUTE void destroyObjects(CCArray* array); //use next method instead
    void destroyObjects(Vector<RawObject*> array);
    
    //Convenience method for performSelector method. Will check the type before passing.
    void destroyObject(CCObject* obj);
    void destroyObjects(CCObject* obj);
    
    int count() { return storedObjects->count(); }
    
    void clear();
    
    RawObject* getById(int id);
    
    //Different ways of querying objects
    //The first object found is always returned to avoid managing arrays, or nil if no result
    //Cache is a way to tell GraphicLayer to cache this particular object for the current scene, so that subsequent call will be very fast
    RawObject* firstObjectWithName(CCString* name, bool cache = false);
    RawObject* firstObjectWithName(std::string name, bool cache = false);
    RawObject* firstObjectWithNameInPanel(std::string name, Panel* panel);
    RawObject* firstObjectAtPosition(CCPoint position);
    RawObject* firstObjectInRect(CCRect rect);
    RawObject* firstObjectContainingRect(CCRect rect);
    RawObject* objectAtIndex(int index);
    
    //The resulting array should be released after being used
    CCArray* allObjectsWithName(CCString* name);
    CCArray* allObjectsWithName(std::string name);
    CCArray* allObjectsWithNameInPanel(std::string name, Panel* panel);
    CCArray* allObjectsStartingWithString(CCString* name);
    CCArray* allObjectsStartingWithString(std::string name);
    CCArray* allObjectsAtPosition(CCPoint position);
    CCArray* allObjectsInRect(CCRect rect);
    CCArray* allObjectsContainingRect(CCRect rect);
    CCArray* allActionnableObjects();
    
    CCArray* allVisibleObjectsAtPosition(CCPoint position);
    
    bool collision(CCPoint position, RawObject* obj);
    
    bool isOnScreen(RawObject* obj, CCSize size = CCSize(0, 0));
    
    bool containsObject(RawObject* obj);
    CCArray* allPanelsWithName(std::string name);
    CCArray* allPanelsWithName(CCString* name);
    Panel* firstPanelWithName(std::string name);
    Panel* firstPanelWithName(CCString* name);
    Panel* firstVisiblePanelWithName(CCString* name);
    
    CCPoint getPositionRelativeToObject(CCPoint point, RawObject* obj);
    CCPoint getRealPosition(RawObject* obj);
    CCPoint getCenterRealPosition(RawObject* obj);
    float getRealScale(RawObject* obj);
    
    //Call this method to transmit a touch to it's children
    //The touch will be transmitted to each child which have position in it's bounding box
    //Return true if at least one child claimed it
    //if event is true, it will post a notification according the object. Else, it will ask for informations about the object
    bool touchAtPosition(CCPoint position, bool event);
    
    //Should be by superclass called to perform object touch action, return YES to claim touch
    bool touchObject(RawObject* obj, bool event, CCPoint position);
    
    
    //TODO : implement reorder, which are not done yet, requires Panel implementation of reorder too
    //reoderChild using zOrder
    void reorderChild(RawObject* child, int zOrder);
    
    //provide an easier way to reorder childs according other childs instead of zOrder.
    //It is an efficient way to reorder childs having the same zOrder.
    void reorderChildAfter(RawObject* child, RawObject* otherChild);
    void reorderChildBefore(RawObject* child, RawObject* otherChild);
    
    //TODO : implement if needed
    //return the time it will take to close all panels. 0 mean there are no panels to close
    //float closeMainPanels();
    
    virtual void update(float deltaTime);
    
    //On resume app, some platforms (Android so far) need to refresh render texture because the openGL context is dropped and the render textures are invalid
    void refreshRenderTextures(CCObject* obj);
    
    //note : a few class may need to friend GraphicLayer to use that method. Its usage may be unsafe but necessary
    void addObject(RawObject* obj, int z = 0);
    int getNextId();
protected:
private:
    void init();
    void reorderChildrenOfPanel(Panel* panel);
    
    void loadBaseNodeAttributes(CustomBaseNode* node, RawObject* obj);
    //Array containing graphic objects and motion panels. Image children are sort according their zOrder (ascending)
    CCArray* storedObjects;
    
    //the actual rendering layer for graphic objects stored
    CCLayer* layer;
    
    //depth in Scene, used to conserve it while saving state, default value = 0
    int depthInScene;
    bool isPaused;
    Scene* relatedScene;
    
    //Array containing Motion panels
    CCArray* storedPanels;
    
    float clock;
    
    CCArray* mainPanels;
    
    bool isUpdating;
    CCArray* objectsToAdd; //contains NSMutableArrays, which contains, in order : a RawObject, its index and its panel (optional)
    CCArray* objectsToRemove; //only contains RawObjects directly
    
    CCDictionary* childParent; //keys are objects ID, values are Panel
    int nextAvailableId;
};
NS_FENNEX_END

#endif /* defined(__FenneX__GraphicLayer__) */
