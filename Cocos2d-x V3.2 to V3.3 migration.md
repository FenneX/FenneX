Cocos2d-x migration from V3.2 to V3.3
====


General steps
----

The update should be straightforward, pull directly FenneX cocosV3.3 branch after upgrading to cocos2d-x 3.2.

Advised steps
----

* switch to Gradle build 1.0.0
* switch to ndk-r10d, which is now properly supported


Android.mk
----

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
