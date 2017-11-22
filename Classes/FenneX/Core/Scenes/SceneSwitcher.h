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

#ifndef __FenneX__SceneSwitcher__
#define __FenneX__SceneSwitcher__

#include "cocos2d.h"
USING_NS_CC;
#include "Pausable.h"
#include "SynthesizeString.h"
#include "SceneName.h"
#include "Scene.h"
#include "FenneXMacros.h"

#define SCENE_SWITCH_OFFSET 0.1f

NS_FENNEX_BEGIN
class SceneSwitcher : public Ref
{
    CC_SYNTHESIZE_READONLY(SceneName, currentSceneName, CurrentSceneName);
    CC_SYNTHESIZE_READONLY(Scene*, currentScene, CurrentScene);
    CC_SYNTHESIZE(float, scale, Scale);
    //CC_SYNTHESIZE(float, cocosScale, CocosScale);
    CC_SYNTHESIZE(Vec2, origin, Origin);
    
public:
    static SceneSwitcher* sharedSwitcher();
    void initWithScene(SceneName nextScene, ValueMap param = ValueMap());
    void trySceneSwitch(float deltaTime = 0);
    void planSceneSwitch(EventCustom* event);
    //A cancel will ignore next scene and directly jump to the queued scene
    //You can only cancel one scene per frame
    void cancelSceneSwitch();
    ~SceneSwitcher();
    
    bool isSwitching();
    
    // This function allow you to reload a scene that is already loaded. Be careful, it's your reponsibility to reload the state of the scene.
    void allowReload();
protected:
    void init();
    void replaceScene();
    void takeQueuedScene();
    SceneName nextScene;
    /*There must be a one frame delay before going to the next scene, to allow a single draw before loading the next scene : for example, you want the button you pressed to be up when loading next scene, for perception problems
     It is true when the delay has been executed, false when it needs to be executed, and undefined the rest of the time
     */
    bool frameDelay;
    bool sceneSwitchCancelled;
    SceneName queuedScene; //in case there is already a nextScene (meaning you call next scene switch during a scene switch), queue another one. In case there is already one queued, it's replaced
    ValueMap queuedParam;
    ValueMap nextSceneParam;
    bool processingSwitch;
    bool isEventFired;
    float delayReplace;
    //The key for keyboard lock during scene switch
    int keyboardLock;
    bool reloadAllowed = false;
    
    EventListenerCustom* planSceneSwitchListener;
};
NS_FENNEX_END

#endif /* defined(__FenneX__SceneSwitcher__) */
