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

#include "Logs.h"
#include "cocos2d.h"
USING_NS_CC;
#include "Pausable.h"
#include "SynthesizeString.h"
#include "SceneName.h"
#include "TouchLinker.h"
#include "GenericRecognizer.h"
#include "Image.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
//Warning : it is mandatory to stop() a scene before stopping using it, because there is a cyclic reference
class Scene : public Pausable, public CCLayer
{
    CC_SYNTHESIZE(SceneName, name, Name);
    CC_SYNTHESIZE_READONLY(CCScene*, delegate, CocosScene);
    CC_SYNTHESIZE_READONLY(CCArray*, updateList, UpdateList);
    CC_SYNTHESIZE_READONLY(CCArray*, touchReceiversList, TouchReceiversList);
    //TODO : add touchlinker
    CC_SYNTHESIZE_READONLY(float, time, CurrentTime);
    CC_SYNTHESIZE_READONLY(CCDictionary*, parameters, Parameters);
public:
    Scene(SceneName identifier, CCDictionary* parameters);
    virtual ~Scene();
    
    static Scene* createScene(SceneName name, CCDictionary* param);
    
    virtual void update(float deltaTime);
    virtual void pause();
    virtual void resume();
    virtual void stop();
    
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent *pEvent);
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
    
    void switchButton(CCPoint position, bool state, CCTouch* touch = NULL);
    void switchButton(Image* obj, bool state, CCTouch* touch = NULL);
    
    
    void tapRecognized(CCObject* obj);
    void addUpdatable(CCObject* obj);
    void removeUpdatable(CCObject* obj);
    void addTouchreceiver(GenericRecognizer* obj);
    void removeTouchreceiver(GenericRecognizer* obj);
    
    static CCPoint touchPosition(CCTouch* touch);
    static CCPoint previousTouchPosition(CCTouch* touch);
    static CCPoint touchOffset(CCTouch* touch);
    
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    TouchLinker* getLinker();
    
    int getFrameNumber();
    
    //TODO : interface mode ?
    //TODO : on selection recognized, cancel tap recognition of that touch
protected:
    void initScene();
    Image* getButtonAtPosition(CCPoint position, bool state);
    int numberOfTouches;
    TouchLinker* linker;
    CCArray* updatablesToRemove;
    CCArray* updatablesToAdd;
    CCArray* receiversToRemove;
    CCArray* receiversToAdd;
    int frameNumber;
};
NS_FENNEX_END

#endif /* defined(__FenneX__Scene__) */
