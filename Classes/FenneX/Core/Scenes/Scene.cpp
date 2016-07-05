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

#include "Scene.h"
#include "SceneSwitcher.h"
#include "SynchronousReleaser.h"
#include "LayoutHandler.h"
#include "TapRecognizer.h"
#include "Shorteners.h"
#include "AppMacros.h"
#include "NativeUtility.h"

NS_FENNEX_BEGIN
void Scene::initScene()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    startSceneInitialisation();
#endif
    currentTime = 0;
    frameNumber = 0;
    delegate = cocos2d::Scene::create();
    delegate->retain();
    if(Director::getInstance()->getNotificationNode()->getParent() != NULL)
    {
        Director::getInstance()->getNotificationNode()->removeFromParentAndCleanup(true);
    }
    delegate->addChild(Director::getInstance()->getNotificationNode());
    delegate->addChild(this);
    linker = new TouchLinker();
}

Scene::Scene(SceneName identifier, CCDictionary* parameters) :
sceneName(identifier)
{
    this->initScene();
    this->parameters = CCDictionary::createWithDictionary(parameters);
    this->parameters->retain();
    numberOfTouches = 0;
    
    //GraphicLayer is a Ref*, retain it for updateList
    GraphicLayer::sharedLayer()->retain();
    updateList.push_back(GraphicLayer::sharedLayer());
    
    //The order is very important : TapRecognized must be registered before InertiaGenerator (generally added in linkToScene), because it can cancel inertia
    this->addTouchreceiver(TapRecognizer::sharedRecognizer());
    tapListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("TapRecognized", std::bind(&Scene::tapRecognized, this, std::placeholders::_1));
    
    appWillResignListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("AppWillResignActive", std::bind(&Scene::dropAllTouches, this, std::placeholders::_1));
    
    GraphicLayer::sharedLayer()->renderOnLayer(this, 0);
    LayoutHandler::sharedHandler()->linkToScene(this);
    
    //TODO : add selectionrecognizer here + suscribe to it
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(false);
    touchListener->onTouchBegan = CC_CALLBACK_2(FenneX::Scene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(FenneX::Scene::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(FenneX::Scene::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(FenneX::Scene::onTouchCancelled, this);
    touchListener->retain();
    touchListener->setEnabled(true);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, -1);
    
    
    keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(FenneX::Scene::onKeyReleased, this);
    keyboardListener->retain();
    keyboardListener->setEnabled(true);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(keyboardListener, -1);
}
Scene::~Scene()
{
    for(Pausable* obj : updateList)
    {
        if(isKindOfClass(obj, Ref))
        {
            dynamic_cast<Ref*>(obj)->release();
        }
    }
    updateList.clear();
    if(touchListener != NULL)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        touchListener->release();
        touchListener = NULL;
    }
    if(keyboardListener != NULL)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(keyboardListener);
        keyboardListener->release();
        keyboardListener = NULL;
    }
    Director::getInstance()->getEventDispatcher()->removeEventListener(tapListener);
    Director::getInstance()->getEventDispatcher()->removeEventListener(appWillResignListener);
    parameters->release();
    delegate->release();
    linker->release();
}

void Scene::update(float deltaTime)
{
    frameNumber++;
#if VERBOSE_PERFORMANCE_TIME
    timeval startTime;
    if(frameNumber <= 3)
        gettimeofday(&startTime, NULL);
#endif
    currentTime += deltaTime;
#if VERBOSE_GENERAL_INFO
    CCLOG("Begin scene update");
#endif
    for(Pausable* obj : updateList)
    {
        //CCLOG("Updating object of type: %s", typeid(*obj).name());
        obj->update(deltaTime);
    }
#if VERBOSE_GENERAL_INFO
    CCLOG("scene update: second part");
#endif
    SceneSwitcher::sharedSwitcher()->trySceneSwitch(deltaTime);
    if(updatablesToRemove.size() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToRemove->count());
        //Manual release for updateList Ref*
        for(Pausable* obj : updatablesToRemove)
        {
            if(isKindOfClass(obj, Ref))
            {
                dynamic_cast<Ref*>(obj)->release();
            }
        }
        
        updateList.erase(std::remove_if(updateList.begin(),
                                        updateList.end(),
                                        [&](Pausable* obj)
                                        {
                                            return std::find(updatablesToRemove.begin(), updatablesToRemove.end(), obj) != updatablesToRemove.end();
                                        }),
                         updateList.end());
        updatablesToRemove.clear();
    }
    if(updatablesToAdd.size() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToAdd->count());
        //Manual retain for updateList Ref*
        for(Pausable* obj : updatablesToAdd)
        {
            updateList.push_back(obj);
        }
        updatablesToAdd.clear();
    }
    if(receiversToRemove.size() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToRemove->count());
        for(GenericRecognizer* recognizer : receiversToRemove)
        {
            touchReceiversList.eraseObject(recognizer);
        }
        receiversToRemove.clear();
    }
    if(receiversToAdd.size() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToAdd->count());
        for(GenericRecognizer* newReceiver : receiversToAdd)
        {
            newReceiver->setLinker(linker);
        }
        touchReceiversList.pushBack(receiversToAdd);
        receiversToAdd.clear();
    }
    SynchronousReleaser::sharedReleaser()->emptyReleasePool();
#if VERBOSE_GENERAL_INFO
    CCLOG("end scene update");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(frameNumber == 3)
    { //The first 2 frames are left for scene creation and defered label/image loading
        runGarbageCollector();
    }
#endif
#if VERBOSE_PERFORMANCE_TIME
    timeval endTime;
    if(frameNumber <= 3)
    {
        gettimeofday(&endTime, NULL);
        CCLOG("Frame %d of scene %s loaded in %f ms", frameNumber, formatSceneToString(sceneName), getTimeDifferenceMS(startTime, endTime));
    }
#endif
}

void Scene::pause()
{
    _running = false;
    for(Pausable* obj : updateList)
    {
        obj->pause();
    }
    this->unscheduleUpdate();
}

void Scene::resume()
{
    _running = true; //Required for scheduleUpdate
    touchListener->setEnabled(true);
    keyboardListener->setEnabled(true);
    this->scheduleUpdateWithPriority(-1);
    for(Pausable* obj : updateList)
    {
        obj->resume();
    }
    LayoutHandler::sharedHandler()->linkToScene(this, false);
}

void Scene::stop()
{
    touchListener->setEnabled(false);
    keyboardListener->setEnabled(false);
    Director::getInstance()->getNotificationNode()->stopAllActions();
    this->unscheduleUpdate();
    
    for(Pausable* obj : updateList)
    {
        obj->stop();
        if(isKindOfClass(obj, Ref))
        {
            dynamic_cast<Ref*>(obj)->release();
        }
    }
    updateList.clear();
    
    for(GenericRecognizer* obj : touchReceiversList)
    {
        obj->cleanTouches();
    }
    touchReceiversList.clear();
    
    delegate->removeChild(this, false);
    delegate->removeChild(Director::getInstance()->getNotificationNode(), false);
    
    //Clean Renderer because commands may rely on a Node, which was removed after the update, but before the render, which causes a crash
    //For example, without this command, updating a RenderTexture during the same update will crash during SceneSwitch
    Director::getInstance()->getRenderer()->clean();
}

//TODO : requires GraphicLayer and TouchLinker
bool Scene::onTouchBegan(Touch *touch, Event *pEvent)
{
    //CCLOG("onTouchBegan started...");
    linker->recordTouch(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("recorded touch at position : %f, %f, ID : %d", Scene::touchPosition(touch).x, Scene::touchPosition(touch).y, touch->getID());
#endif
    
    this->switchButton(Scene::touchPosition(touch), true, touch);
    
    //CCLOG("sending to receivers ...");
    for(GenericRecognizer* receiver : touchReceiversList)
    {
        receiver->onTouchBegan(touch, pEvent);
    }
    //TODO : cancel selection if needed
    numberOfTouches++;
    //CCLOG("onTouchBegan ended");
    return true;
}

void Scene::onTouchMoved(Touch *touch, Event *pEvent)
{
    //CCLOG("onTouchMoved started...");
    if(linker->linkedObjectOf(touch) != NULL
       && isKindOfClass(linker->linkedObjectOf(touch), const Image))
    {
        Image* toggle = (Image*)linker->linkedObjectOf(touch);
        GraphicLayer* layer = GraphicLayer::sharedLayer();
        char *end = strrchr(toggle->getImageFile().c_str(), '-');
        if(end && strcmp(end, "-on") == 0 && !layer->allObjectsAtPosition(Scene::touchPosition(touch))->containsObject(toggle))
        {
            this->switchButton(toggle, false);
        }
    }
    
    for(GenericRecognizer* receiver : touchReceiversList)
    {
        receiver->onTouchMoved(touch, pEvent);
    }
    //CCLOG("onTouchMoved ended");
}

void Scene::onTouchEnded(Touch *touch, Event *pEvent)
{
    //CCLOG("onTouchEnded started...");
    if(linker->linkedObjectOf(touch) != NULL
       && isKindOfClass(linker->linkedObjectOf(touch), const Image)
       && GraphicLayer::sharedLayer()->containsObject((RawObject*)linker->linkedObjectOf(touch)))
    {
        Image* toggle = (Image*)linker->linkedObjectOf(touch);
        linker->unlinkTouch(touch);
        char *end = strrchr(toggle->getImageFile().c_str(), '-');
        if(end && strcmp(end, "-on") == 0 && toggle->getEventInfos()->objectForKey("_OriginalImageFile") != NULL && linker->touchesLinkedTo(toggle).size() == 0)
        {
            this->switchButton(toggle, false);
        }
    }
    
    for(GenericRecognizer* receiver : touchReceiversList)
    {
        receiver->onTouchEnded(touch, pEvent);
    }
    numberOfTouches--;
    linker->removeTouch(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("onTouchEnded ended at position : %f, %f, ID : %d", Scene::touchPosition(touch).x, Scene::touchPosition(touch).y, touch->getID());
#endif
}

void Scene::onTouchCancelled(Touch *touch, Event *pEvent)
{
    this->onTouchEnded(touch, pEvent);
    
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("ended from cancel");
#endif
}

void Scene::switchButton(Vec2 position, bool state, Touch* touch)
{
    Image* target = getButtonAtPosition(position, state);
    //CCLOG("checked if toggle");
    if(target != NULL)
    {
        this->switchButton(target, state, touch);
    }
}

void Scene::switchButton(Image* obj, bool state, Touch* touch)
{
    if(state)
    {
        if(obj->getEventInfos()->objectForKey("_OriginalImageFile") == NULL)
        {
            obj->setEventInfo(Screate(obj->getImageFile()), "_OriginalImageFile");
            obj->replaceTexture(obj->getImageFile() + "-on");
        }
        //If it was actually replaced, it will end by -on
        char *end = strrchr(obj->getImageFile().c_str(), '-');
        if (end && strcmp(end, "-on") == 0)
        {
            linker->linkTouch(touch, obj);
        }
        else
        {
            obj->removeEventInfo("_OriginalImageFile");
        }
    }
    else
    {
        if(obj->getEventInfos()->objectForKey("_OriginalImageFile") != NULL)
        {
            obj->replaceTexture(TOCSTRING(obj->getEventInfos()->objectForKey("_OriginalImageFile")));
            obj->removeEventInfo("_OriginalImageFile");
        }
        linker->unlinkObject(obj);
    }
}

void Scene::tapRecognized(EventCustom* event)
{
    Touch* touch = (Touch*)((CCDictionary*)event->getUserData())->objectForKey("Touch");
    
    Vec2 pos = Scene::touchPosition(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("Tap recognized at pos %f, %f, forwarding to layer ...", pos.x, pos.y);
#endif
    RawObject* target = getButtonAtPosition(pos, false);
    if(target != NULL && linker->touchesLinkedTo(target).size() > 0)
    {
#if VERBOSE_TOUCH_RECOGNIZERS
        CCLOG("Tap intercepted by button still linked");
#endif
    }
    else if(GraphicLayer::sharedLayer()->touchAtPosition(pos, true))
    {
#if VERBOSE_TOUCH_RECOGNIZERS
        CCLOG("Tap used!");
#endif
    }
    else
    {
#if VERBOSE_TOUCH_RECOGNIZERS
        CCLOG("No target found for tap");
#endif
    }
}

void Scene::dropAllTouches(EventCustom* event)
{
    Vector<Touch*> touches = linker->allTouches();
    for(long i = touches.size() - 1; i > 0; i--)
    {
        this->onTouchEnded(touches.at(i), NULL);
    }
}

void Scene::addUpdatable(Pausable* obj)
{
    if(obj != NULL
       && std::find(updateList.begin(), updateList.end(), obj) == updateList.end()
       && std::find(updatablesToAdd.begin(), updatablesToAdd.end(), obj) == updatablesToAdd.end())
    {
        updatablesToAdd.push_back(obj);
        if(isKindOfClass(obj, Ref))
        {
            dynamic_cast<Ref*>(obj)->retain();
        }
    }
}

void Scene::removeUpdatable(Pausable* obj)
{
    if(obj != NULL
       && std::find(updateList.begin(), updateList.end(), obj) != updateList.end()
       && std::find(updatablesToRemove.begin(), updatablesToRemove.end(), obj) == updatablesToRemove.end())
    {
        updatablesToRemove.push_back(obj);
        if(isKindOfClass(obj, Ref))
        {
            dynamic_cast<Ref*>(obj)->retain();
        }
    }
}

void Scene::addTouchreceiver(GenericRecognizer* obj)
{
    if(obj != NULL && !touchReceiversList.contains(obj) && !receiversToAdd.contains(obj))
    {
        receiversToAdd.pushBack(obj);
    }
}

void Scene::removeTouchreceiver(GenericRecognizer* obj)
{
    if(obj != NULL && touchReceiversList.contains(obj) && !receiversToRemove.contains(obj))
    {
        receiversToRemove.pushBack(obj);
    }
    else if(obj != NULL && receiversToAdd.contains(obj))
    {
        receiversToAdd.eraseObject(obj);
    }
}

Vec2 Scene::touchPosition(Touch* touch)
{
    //invert y because the given position is inverted ...
    Vec2 pos = touch->getLocationInView();
    int height = Director::getInstance()->getWinSize().height;//cocos2d::Director::getInstance()->getOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos *= 1/SceneSwitcher::sharedSwitcher()->getScale();
    //pos -= SceneSwitcher::sharedSwitcher()->getOrigin());
    return pos;
}

Vec2 Scene::previousTouchPosition(Touch* touch)
{
    //invert y because the given position is inverted ...
    Vec2 pos = touch->getPreviousLocationInView();
    int height = Director::getInstance()->getWinSize().height;//cocos2d::Director::getInstance()->getOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos -= SceneSwitcher::sharedSwitcher()->getOrigin();
    return pos;
}

Vec2 Scene::touchOffset(Touch* touch)
{
    return Vec2(Scene::touchPosition(touch).x - Scene::previousTouchPosition(touch).x, Scene::touchPosition(touch).y - Scene::previousTouchPosition(touch).y);
}

void Scene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("KeyBackClicked");
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_HOME)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("KeyMenuClicked");
    }
}

TouchLinker* Scene::getLinker()
{
    return linker;
}

int Scene::getFrameNumber()
{
    return frameNumber;
}

Image* Scene::getButtonAtPosition(Vec2 position, bool state)
{
    Image* target = NULL;
    CCArray* objects = GraphicLayer::sharedLayer()->allVisibleObjectsAtPosition(position);
    for(int i = 0; i < objects->count() && target == NULL; i++)
    {
        RawObject* obj = (RawObject*)objects->objectAtIndex(i);
        if(obj->isVisible() && obj->getEventActivated() && !obj->getEventName().empty() && obj->getEventName()[0] != '\0' && dynamic_cast<Image*>(obj) != NULL)
        {
            //If state = false, the object imagefile must finish by "-on" and and have an _OriginalImageFile
            char *end = strrchr(((Image*)obj)->getImageFile().c_str(), '-');
            if(state || (end && strcmp(end, "-on") == 0 && obj->getEventInfos()->objectForKey("_OriginalImageFile") != NULL))
                target = (Image*)obj;
        }
    }
    return target;
}
NS_FENNEX_END
