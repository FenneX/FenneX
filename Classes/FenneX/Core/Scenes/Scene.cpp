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
    delegate = CCScene::create();
    delegate->retain();
    if(CCDirector::sharedDirector()->getNotificationNode()->getParent() != NULL)
    {
        CCDirector::sharedDirector()->getNotificationNode()->removeFromParentAndCleanup(true);
    }
    delegate->addChild(CCDirector::sharedDirector()->getNotificationNode());
    delegate->addChild(this);
    updateList = new CCArray();
    touchReceiversList = new CCArray();
    updatablesToRemove = new CCArray();
    updatablesToAdd = new CCArray();
    receiversToAdd = new CCArray();
    receiversToRemove = new CCArray();
    //TODO : add touchlinker
    linker = new TouchLinker();
    this->setKeypadEnabled(true);
}

Scene::Scene(SceneName identifier, CCDictionary* parameters) :
sceneName(identifier)
{
    this->initScene();
    this->parameters = CCDictionary::createWithDictionary(parameters);
    this->parameters->retain();
    numberOfTouches = 0;
    updateList->addObject(GraphicLayer::sharedLayer());
    
    //The order is very important : TapRecognized must be registered before InertiaGenerator (generally added in linkToScene), because it can cancel inertia
    this->addTouchreceiver(TapRecognizer::sharedRecognizer());
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Scene::tapRecognized), "TapRecognized", NULL);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Scene::dropAllTouches), "AppWillResignActive", NULL);
    
    GraphicLayer::sharedLayer()->renderOnLayer(this, 0);
    LayoutHandler::sharedHandler()->linkToScene(this);
    
    //TODO : add selectionrecognizer here + suscribe to it
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = CC_CALLBACK_2(FenneX::Scene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(FenneX::Scene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(FenneX::Scene::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(FenneX::Scene::onTouchCancelled, this);
    listener->retain();
    
    CCDirector::sharedDirector()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, -1);
    listener->setEnabled(true);
    this->scheduleUpdateWithPriority(-1);
}
Scene::~Scene()
{
    if(listener != NULL)
    {
        CCDirector::sharedDirector()->getEventDispatcher()->removeEventListener(listener);
        listener->release();
        listener = NULL;
    }
    this->setKeypadEnabled(false);
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
    parameters->release();
    delegate->release();
    updateList->release();
    touchReceiversList->release();
    linker->release();
    updatablesToAdd->release();
    updatablesToRemove->release();
    receiversToAdd->release();
    receiversToRemove->release();
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
    CCObject* obj = NULL;
#if VERBOSE_GENERAL_INFO
    CCLOG("Begin scene update");
#endif
    CCARRAY_FOREACH(updateList, obj)
    {
        //CCLOG("Updating object of type: %s", typeid(*obj).name());
        dynamic_cast<Pausable*>(obj)->update(deltaTime);
    }
#if VERBOSE_GENERAL_INFO
    CCLOG("scene update: second part");
#endif
    SceneSwitcher::sharedSwitcher()->trySceneSwitch(deltaTime);
    if(updatablesToRemove->count() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToRemove->count());
        updateList->removeObjectsInArray(updatablesToRemove);
        updatablesToRemove->removeAllObjects();
    }
    if(updatablesToAdd->count() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToAdd->count());
        updateList->addObjectsFromArray(updatablesToAdd);
        updatablesToAdd->removeAllObjects();
    }
    if(receiversToRemove->count() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToRemove->count());
        touchReceiversList->removeObjectsInArray(receiversToRemove);
        receiversToRemove->removeAllObjects();
    }
    if(receiversToAdd->count() > 0)
    {
        //CCLOG("Removing %d updatables", updatablesToAdd->count());
        touchReceiversList->addObjectsFromArray(receiversToAdd);
        for(int i = 0; i < receiversToAdd->count(); i++)
        {
            ((GenericRecognizer*)receiversToAdd->objectAtIndex(i))->setLinker(linker);
        }
        receiversToAdd->removeAllObjects();
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
        CCLog("Frame %d of scene %s loaded in %f ms", frameNumber, formatSceneToString(sceneName), getTimeDifferenceMS(startTime, endTime));
    }
#endif
}

void Scene::pause()
{
    CCObject* obj = NULL;
    CCARRAY_FOREACH(updateList, obj)
    {
        dynamic_cast<Pausable*>(obj)->pause();
    }
    this->unscheduleUpdate();
}

void Scene::resume()
{
    listener->setEnabled(true);
    this->scheduleUpdateWithPriority(-1);
    CCObject* obj = NULL;
    CCARRAY_FOREACH(updateList, obj)
    {
        dynamic_cast<Pausable*>(obj)->resume();
    }
    LayoutHandler::sharedHandler()->linkToScene(this, false);
}

void Scene::stop()
{
    listener->setEnabled(false);
    CCDirector::sharedDirector()->getNotificationNode()->stopAllActions();
    this->unscheduleUpdate();
    
    CCObject* obj = NULL;
    CCARRAY_FOREACH(updateList, obj)
    {
        dynamic_cast<Pausable*>(obj)->stop();
    }
    updateList->removeAllObjects();
    delegate->removeChild(this, false);
    delegate->removeChild(CCDirector::sharedDirector()->getNotificationNode(), false);
}

//TODO : requires GraphicLayer and TouchLinker
bool Scene::onTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("onTouchBegan started...");
    linker->recordTouch(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("recorded touch at position : %f, %f, ID : %d", Scene::touchPosition(touch).x, Scene::touchPosition(touch).y, touch->getID());
#endif
    
    this->switchButton(Scene::touchPosition(touch), true, touch);
    
    //CCLOG("sending to receivers ...");
    CCObject* CCobj;
    CCARRAY_FOREACH(touchReceiversList, CCobj)
    {
        GenericRecognizer* receiver = (GenericRecognizer*)CCobj;
        receiver->onTouchBegan(touch, pEvent);
    }
    //TODO : cancel selection if needed
    numberOfTouches++;
    //CCLOG("onTouchBegan ended");
    return true;
}

void Scene::onTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("onTouchMoved started...");
    if(linker->linkedObjectOf(touch) != NULL
       && isKindOfClass(linker->linkedObjectOf(touch), const Image))
    {
        Image* toggle = (Image*)linker->linkedObjectOf(touch);
        GraphicLayer* layer = GraphicLayer::sharedLayer();
        char *end = strrchr(toggle->getImageFile(), '-');
        if(end && strcmp(end, "-on") == 0 && !layer->allObjectsAtPosition(Scene::touchPosition(touch))->containsObject(toggle))
        {
            this->switchButton(toggle, false);
        }
    }
    CCObject* CCobj;
    CCARRAY_FOREACH(touchReceiversList, CCobj)
    {
        GenericRecognizer* receiver = (GenericRecognizer*)CCobj;
        receiver->onTouchMoved(touch, pEvent);
    }
    //CCLOG("onTouchMoved ended");
}

void Scene::onTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("onTouchEnded started...");
    if(linker->linkedObjectOf(touch) != NULL
       && isKindOfClass(linker->linkedObjectOf(touch), const Image)
       && GraphicLayer::sharedLayer()->containsObject((RawObject*)linker->linkedObjectOf(touch)))
    {
        Image* toggle = (Image*)linker->linkedObjectOf(touch);
        linker->unlinkTouch(touch);
        char *end = strrchr(toggle->getImageFile(), '-');
        if(end && strcmp(end, "-on") == 0 && toggle->getEventInfos()->objectForKey("_OriginalImageFile") != NULL && linker->touchesLinkedTo(toggle)->count() == 0)
        {
            this->switchButton(toggle, false);
        }
    }
    CCObject* CCobj;
    CCARRAY_FOREACH(touchReceiversList, CCobj)
    {
        GenericRecognizer* receiver = (GenericRecognizer*)CCobj;
        receiver->onTouchEnded(touch, pEvent);
    }
    numberOfTouches--;
    linker->removeTouch(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("onTouchEnded ended at position : %f, %f, ID : %d", Scene::touchPosition(touch).x, Scene::touchPosition(touch).y, touch->getID());
#endif
}

void Scene::onTouchCancelled(CCTouch *touch, CCEvent *pEvent)
{
    this->onTouchEnded(touch, pEvent);
    
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("ended from cancel");
#endif
}

void Scene::switchButton(CCPoint position, bool state, CCTouch* touch)
{
    Image* target = getButtonAtPosition(position, state);
    //CCLOG("checked if toggle");
    if(target != NULL)
    {
        this->switchButton(target, state, touch);
    }
}

void Scene::switchButton(Image* obj, bool state, CCTouch* touch)
{
    if(state)
    {
        if(obj->getEventInfos()->objectForKey("_OriginalImageFile") == NULL)
        {
            obj->setEventInfo(Screate(obj->getImageFile()), "_OriginalImageFile");
            obj->replaceTexture(ScreateF("%s-on", obj->getImageFile())->getCString());
        }
        //If it was actually replaced, it will end by -on
        char *end = strrchr(obj->getImageFile(), '-');
        if (end && strcmp(end, "-on") == 0)
        {
            linker->linkTouch(touch, obj);
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

void Scene::tapRecognized(CCObject* obj)
{
    CCTouch* touch = (CCTouch*)((CCDictionary*)obj)->objectForKey("Touch");
    
    CCPoint pos = Scene::touchPosition(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("Tap recognized at pos %f, %f, forwarding to layer ...", pos.x, pos.y);
#endif
    RawObject* target = getButtonAtPosition(pos, false);
    if(target != NULL && linker->touchesLinkedTo(target)->count() > 0)
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

void Scene::dropAllTouches(Ref* obj)
{
    CCArray* touches = linker->allTouches();
    for(int i = touches->count() - 1; i > 0; i--)
    {
        this->onTouchEnded((CCTouch*)touches->objectAtIndex(i), NULL);
    }
}

void Scene::addUpdatable(CCObject* obj)
{
    if(obj != NULL && !updateList->containsObject(obj) && !updatablesToAdd->containsObject(obj))
    {
        updatablesToAdd->addObject(obj);
    }
}

void Scene::removeUpdatable(CCObject* obj)
{
    if(obj != NULL && updateList->containsObject(obj) && !updatablesToRemove->containsObject(obj))
    {
        updatablesToRemove->addObject(obj);
    }
}

void Scene::addTouchreceiver(GenericRecognizer* obj)
{
    if(obj != NULL && !touchReceiversList->containsObject(obj) && !receiversToAdd->containsObject(obj))
    {
        receiversToAdd->addObject(obj);
    }
}

void Scene::removeTouchreceiver(GenericRecognizer* obj)
{
    if(obj != NULL && touchReceiversList->containsObject(obj) && !receiversToRemove->containsObject(obj))
    {
        receiversToRemove->addObject(obj);
    }
}

CCPoint Scene::touchPosition(CCTouch* touch)
{
    //invert y because the given position is inverted ...
    CCPoint pos = touch->getLocationInView();
    int height = CCDirector::sharedDirector()->getWinSize().height;//cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos = ccpMult(pos, 1/SceneSwitcher::sharedSwitcher()->getScale());
    //pos = ccpSub(pos, SceneSwitcher::sharedSwitcher()->getOrigin());
    return pos;
}

CCPoint Scene::previousTouchPosition(CCTouch* touch)
{
    //invert y because the given position is inverted ...
    CCPoint pos = touch->getPreviousLocationInView();
    int height = CCDirector::sharedDirector()->getWinSize().height;//cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos = ccpSub(pos, SceneSwitcher::sharedSwitcher()->getOrigin());
    return pos;
}

CCPoint Scene::touchOffset(CCTouch* touch)
{
    return CCPoint(Scene::touchPosition(touch).x - Scene::previousTouchPosition(touch).x, Scene::touchPosition(touch).y - Scene::previousTouchPosition(touch).y);
}

void Scene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        CCNotificationCenter::sharedNotificationCenter()->postNotification("KeyBackClicked");
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_HOME)
    {
        CCNotificationCenter::sharedNotificationCenter()->postNotification("KeyMenuClicked");
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

Image* Scene::getButtonAtPosition(CCPoint position, bool state)
{
    Image* target = NULL;
    CCArray* objects = GraphicLayer::sharedLayer()->allVisibleObjectsAtPosition(position);
    for(int i = 0; i < objects->count() && target == NULL; i++)
    {
        RawObject* obj = (RawObject*)objects->objectAtIndex(i);
        if(obj->getNode()->isVisible() && obj->getEventActivated() && obj->getEventName() != NULL && obj->getEventName()[0] != '\0' && isKindOfClass(obj, Image))
        {
            //If state = false, the object imagefile must finish by "-on" and and have an _OriginalImageFile
            char *end = strrchr(((Image*)obj)->getImageFile(), '-');
            if(state || (end && strcmp(end, "-on") == 0 && obj->getEventInfos()->objectForKey("_OriginalImageFile") != NULL))
                target = (Image*)obj;
        }
    }
    return target;
}
NS_FENNEX_END
