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

#ifndef __FenneX__LayoutHandler__
#define __FenneX__LayoutHandler__

#include "cocos2d.h"
USING_NS_CC;
#include "GraphicLayer.h"
#include "Scene.h"
#include "FenneXMacros.h"

class EventResponder;

NS_FENNEX_BEGIN

class LayoutHandler : public Ref
{
public:
    static LayoutHandler* sharedHandler(void);
    void linkToScene(Scene* target, bool initDisplay = true);
    ~LayoutHandler();
    EventResponder* getResponder();
    //Those funcs are added for the app lifetime, they will be executed at each scene creation
    //The first one will be executed only during Scene creation, the second one both during Scene creation and when app resume from background
    void addInitSceneDisplayFunc(std::function<void(Scene*)> func);
    void addCatchSceneEventsFunc(std::function<void(Scene*)> func);
private:
    void init();
    
    void createSceneGraphics(Scene* target);
    void catchEvents(Scene* target);
    
    GraphicLayer* layer;
    Scene* currentScene;
    EventResponder* responder;
    Vector<EventListenerCustom*> listeners;
    std::vector<std::function<void(Scene*)>> initSceneDisplayFunctions;
    std::vector<std::function<void(Scene*)>> catchSceneEventsFunctions;
};
NS_FENNEX_END

#endif /* defined(__FenneX__LayoutHandler__) */
