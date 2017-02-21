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

#ifndef __FenneX__Scene__
#define __FenneX__Scene__

#include "cocos2d.h"
USING_NS_CC;
#include "Pausable.h"
#include "SynthesizeString.h"
#include "SceneName.h"
#include "TouchLinker.h"
#include "GenericRecognizer.h"
#include "Image.h"
#include "FenneXMacros.h"

#define LAYER (GraphicLayer::sharedLayer())
#define ADD_OBSERVER(func, notifName) (eventListeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener(notifName, std::bind(&func, this, std::placeholders::_1))))
#define ADD_SIMPLE_OBSERVER(func, notifName) (eventListeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener(notifName, std::bind(&func, this))))

NS_FENNEX_BEGIN
//Warning : it is mandatory to stop() a scene before stopping using it, because there is a cyclic reference
class Scene : public Pausable, public Layer
{
    CC_SYNTHESIZE(SceneName, sceneName, SceneName);
    CC_SYNTHESIZE_READONLY(cocos2d::Scene*, delegate, CocosScene);
    CC_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<Pausable*>, updateList, UpdateList);
    CC_SYNTHESIZE_READONLY_PASS_BY_REF(Vector<GenericRecognizer*>, touchReceiversList, TouchReceiversList);
    //TODO : add touchlinker
    CC_SYNTHESIZE_READONLY(float, currentTime, CurrentTime);
    CC_SYNTHESIZE_READONLY(CCDictionary*, parameters, Parameters);
public:
    Scene(SceneName identifier, CCDictionary* parameters);
    virtual ~Scene();
    
    static Scene* createScene(SceneName name, CCDictionary* param);
    
    virtual void update(float deltaTime);
    virtual void pause();
    virtual void resume();
    virtual void stop();
    
    virtual bool onTouchBegan(Touch *touch, Event *pEvent);
    virtual void onTouchMoved(Touch *touch, Event *pEvent);
    virtual void onTouchEnded(Touch *touch, Event *pEvent);
    virtual void onTouchCancelled(Touch *touch, Event *pEvent);
    
    void switchButton(Vec2 position, bool state, Touch* touch = NULL);
    void switchButton(Image* obj, bool state, Touch* touch = NULL);
    
    
    void tapRecognized(EventCustom* event);
    void dropAllTouches(EventCustom* event);
    
    //Pausable will be retain/released if they are of type Ref*
    void addUpdatable(Pausable* obj);
    void removeUpdatable(Pausable* obj);
    
    //GenericRecognizer will always be retain/released
    void addTouchreceiver(GenericRecognizer* obj);
    void removeTouchreceiver(GenericRecognizer* obj);
    
    static Vec2 touchPosition(Touch* touch);
    static Vec2 previousTouchPosition(Touch* touch);
    static Vec2 touchOffset(Touch* touch);
    
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
    TouchLinker* getLinker();
    
    int getFrameNumber();
    
    //TODO : interface mode ?
    //TODO : on selection recognized, cancel tap recognition of that touch
protected:
    void initScene();
    Image* getButtonAtPosition(Vec2 position, bool state);
    int numberOfTouches;
    TouchLinker* linker;
    std::vector<Pausable*> updatablesToRemove;
    std::vector<Pausable*> updatablesToAdd;
    Vector<GenericRecognizer*> receiversToRemove;
    Vector<GenericRecognizer*> receiversToAdd;
    int frameNumber;
    EventListenerTouchOneByOne* touchListener;
    EventListenerKeyboard* keyboardListener;
    EventListenerCustom* tapListener;
    EventListenerCustom* appWillResignListener;
    
    Vector<EventListenerCustom*> eventListeners;
};
NS_FENNEX_END

#endif /* defined(__FenneX__Scene__) */
