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
    time = 0;
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
name(identifier)
{
    this->initScene();
    this->parameters = CCDictionary::createWithDictionary(parameters);
    this->parameters->retain();
    numberOfTouches = 0;
    updateList->addObject(GraphicLayer::sharedLayer());
    
    //The order is very important : TapRecognized must be registered before InertiaGenerator (generally added in linkToScene), because it can cancel inertia
    this->addTouchreceiver(TapRecognizer::sharedRecognizer());
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Scene::tapRecognized), "TapRecognized", NULL);
    
    GraphicLayer::sharedLayer()->renderOnLayer(this, 0);
    LayoutHandler::sharedHandler()->linkToScene(this);
    
    //TODO : add selectionrecognizer here + suscribe to it
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    this->scheduleUpdateWithPriority(-1);
}
Scene::~Scene()
{
    this->setKeypadEnabled(false);
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this);
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
    cc_timeval startTime;
    if(frameNumber <= 3)
        CCTime::gettimeofdayCocos2d(&startTime, NULL);
#endif
    time += deltaTime;
    CCObject* obj = NULL;
#if VERBOSE_GENERAL_INFO
    CCLOG("Begin scene update");
#endif
    CCARRAY_FOREACH(updateList, obj)
    {
        //CCLOG("Updating object of type: %s", typeid(*obj).name());
        dynamic_cast<CCObject*>(obj)->update(deltaTime);
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
    cc_timeval endTime;
    if(frameNumber <= 3)
    {
        CCTime::gettimeofdayCocos2d(&endTime, NULL);
        CCLog("Frame %d of scene %s loaded in %f ms", frameNumber, formatSceneToString(name), CCTime::timersubCocos2d(&startTime, &endTime));
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
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
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
bool Scene::ccTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("ccTouchBegan started...");
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
        receiver->ccTouchBegan(touch, pEvent);
    }
    //TODO : cancel selection if needed
    numberOfTouches++;
    //CCLOG("ccTouchBegan ended");
    return true;
}

void Scene::ccTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("ccTouchMoved started...");
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
        receiver->ccTouchMoved(touch, pEvent);
    }
    //CCLOG("ccTouchMoved ended");
}

void Scene::ccTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
    //CCLOG("ccTouchEnded started...");
    if(linker->linkedObjectOf(touch) != NULL
       && isKindOfClass(linker->linkedObjectOf(touch), const Image))
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
        receiver->ccTouchEnded(touch, pEvent);
    }
    numberOfTouches--;
    linker->removeTouch(touch);
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("ccTouchEnded ended at position : %f, %f, ID : %d", Scene::touchPosition(touch).x, Scene::touchPosition(touch).y, touch->getID());
#endif
}

void Scene::ccTouchCancelled(CCTouch *touch, CCEvent *pEvent)
{
    this->ccTouchEnded(touch, pEvent);
    
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("ended from cancel");
#endif
}

void Scene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("On Scene : ccTouchesBegan with set called");
#endif
    CCSetIterator it = pTouches->begin();
    CCPoint pt;
    CCTouch* touch;
    
    for( int iTouchCount = 0; iTouchCount < pTouches->count(); iTouchCount++ )
    {
        touch = (CCTouch*)(*it);
        this->ccTouchBegan(touch, pEvent);
        it++;
    }
}

void Scene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("On Scene : ccTouchesMoved with set called");
#endif
    CCSetIterator it = pTouches->begin();
    CCPoint pt;
    CCTouch* touch;
    
    for( int iTouchCount = 0; iTouchCount < pTouches->count(); iTouchCount++ )
    {
        touch = (CCTouch*)(*it);
        this->ccTouchMoved(touch, pEvent);
        it++;
    }
}

void Scene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLOG("On Scene : ccTouchesEnded with set called");
#endif
    CCSetIterator it = pTouches->begin();
    CCPoint pt;
    CCTouch* touch;
    
    for( int iTouchCount = 0; iTouchCount < pTouches->count(); iTouchCount++ )
    {
        touch = (CCTouch*)(*it);
        this->ccTouchEnded(touch, pEvent);
        it++;
    }
}

void Scene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
#if VERBOSE_TOUCH_RECOGNIZERS
    CCLog("On Scene : ccTouchesCancelled with set called");
#endif
    CCSetIterator it = pTouches->begin();
    CCPoint pt;
    CCTouch* touch;
    
    for( int iTouchCount = 0; iTouchCount < pTouches->count(); iTouchCount++ )
    {
        touch = (CCTouch*)(*it);
        this->ccTouchCancelled(touch, pEvent);
        it++;
    }
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
    int height = CCDirector::sharedDirector()->getWinSize().height;//cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos = ccpMult(pos, 1/SceneSwitcher::sharedSwitcher()->getScale());
    //pos = ccpSub(pos, SceneSwitcher::sharedSwitcher()->getOrigin());
    return pos;
}

CCPoint Scene::previousTouchPosition(CCTouch* touch)
{
    //invert y because the given position is inverted ...
    CCPoint pos = touch->getPreviousLocationInView();
    int height = CCDirector::sharedDirector()->getWinSize().height;//cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().height;
    pos.y = height - pos.y;
    //pos = ccpSub(pos, SceneSwitcher::sharedSwitcher()->getOrigin());
    return pos;
}

CCPoint Scene::touchOffset(CCTouch* touch)
{
    return CCPoint(Scene::touchPosition(touch).x - Scene::previousTouchPosition(touch).x, Scene::touchPosition(touch).y - Scene::previousTouchPosition(touch).y);
}

void Scene::keyBackClicked()
{
    CCNotificationCenter::sharedNotificationCenter()->postNotification("KeyBackClicked");
}

void Scene::keyMenuClicked()
{
    CCNotificationCenter::sharedNotificationCenter()->postNotification("KeyMenuClicked");
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