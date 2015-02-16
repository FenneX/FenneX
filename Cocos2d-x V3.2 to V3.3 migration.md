Cocos2d-x migration from V3.2 to V3.3
====


General steps
----

The update should be straightforward, pull directly FenneX cocosV3.3 branch after upgrading to cocos2d-x 3.2.
Don't forget to reload Xcode so that changes on cocos2d-x project are taken into account.

Advised steps
----

* switch to Gradle build 1.0.0
* switch to ndk-r10d, which is now properly supported


Android.mk
----

Android.mk update should be automatic, as it is part of FenneX. In case something goes wrong during the merge:

You will need to add ```LOCAL_CXXFLAGS += -fexceptions``` to compile.

You will need to add cocos_ui, as EditBox was migrated from extensions to cocos/ui:

```
LOCAL_WHOLE_STATIC_LIBRARIES := cocos_ui_static
```

If you need Spine, add:

```
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
```

```
$(call import-module,editor-support/spine)
```

FenneX has disabled tiff, webp, chipmunk and cocostudio. If they are needed they can be re-enabled on cocos2d/cocos/Android.mk. Compilation was not tested with them.


Fixing Clipping
----

V3.3 has proper support for clipping. You need to add the following lines:

AppDelegate.h

```
virtual void initGLContextAttrs();
```

AppDelegate.cpp

```
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}
```

Fixing SuperAnim compilation
----

GetFileData must have the size parameter be a ssize_t and calls to CCFileUtils::sharedFileUtils()->getFileData must use a ssize_t as last parameter

CCNotificationCenter to EventDispatcher
----

As CCNotificationCenter is deprecated, it is required to migrate to EventDispatcher. This migration is time-consuming, as it's not a 1 to 1 equivalence.

Instructions:

* Replace CCNotificationCenter::sharedNotificationCenter() by Director::getInstance()->getEventDispatcher(). It is recommended to do that last to be able to search globally for CCNotificationCenter
* replace addObserver by addCustomEventListener, which takes a function instead of selector + object + filter. You will generally use std::bind(&ObjectClass:funcname, this, std::placeholders::_1) (skip the placeholder if the function doesn't take an argument)
* you will need to save the created listeners somewhere to be able to remove them, as they are no longer directly linked to an object
* function prototype must be changed from Ref\* (or CCObject\*) to EventCustom\* event. To get the informations, you must now get event->getUserData(), which is of type void* (instead of Ref*)
* to send a notification, replace postNotification by dispatchCustomEvent
* perfomNotificationAfterDelay and performSelectorAfterDelay have been replaced by DelayedDispatcher class. See Shorteners.h and DelayedDispatcher.h for more infos
