Cocos2d-x migration from V2.2 to V3.2
====

First steps:
----

* create a new project using cocos new (refer to README from cocos2d-x)
* replace application icons, splashcreens, prefix.pch and infos.plist
* fix errors from blank project, if any
* apply FenneX cocos2d-x modification
* add FenneX
* add your code and assets
* fix errors by following the following steps

CCTime:
----

* replace cc_timeval  by  timeval   (C++ structure)
* replace CCTime::gettimeofdayCocos2d  by  gettimeofday    (C++ function)
* replace CCTime::timersubCocos2d  by  getTimeDifferenceMS   (FenneX function)

CCString:
---

* replace CCString* when calling wrappers to std::string
* replace CCString* to std::string when getting string from wrappers
* pay attention not to send NULL but empty string instead in wrappers function

Labels:
----

* replace CCLabelTTF by Label

Fennex RawObjects:
----

* prefix Image with FenneX::
* prefix LabelTTF with FenneX::
* "Sender" eventInfos now contains an ID (integer), use GraphicLayer::getById to get the object
* RawObject can't be passed everywhere because it doesn't implemente Clonable (which would have side-effects), pass its ID instead (and retrieve it with GraphicLayer::getById)

Includes:
----

* replace #include "../extensions/CCBReader/CCBReader.h"  by  #include "editor-support/cocosbuilder/CocosBuilder.h"
* replace #include "../extensions/network/HttpClient.h"  by  #include "network/HttpClient.h"

Cocos2d-x:
----

* replace CCEGLView::sharedOpenGLView();  by  CCDirector::sharedDirector()->getOpenGLView();

Touches:
----

* replace ccTouchBegan  by  onTouchBegan
* replace ccTouchMoved  by  onTouchMoved
* replace ccTouchEnded  by  onTouchEnded
* Scrolling/Inertia receiver must check for TMPPoint instead of CCPoint

CallFunc:
----

* replace methods called by callfunc to prototype void YourMethod(Ref* obj)
* class called by callfunc must subclass Ref* (or CCObject)
* later on, replace callFunc by std:function using CC_CALLBACK_X (where X is the number of arguments)

CCNotificationCenter:
----

* change removeObserver(target) to removeAllObservers(target)

RenderTexture:
----

* use different Sprites for each RenderTexture, as the actual visit() is now delayed (so if you modify a Sprite you visit position, it affect all other RenderTextures)

ClippingNode:
----
* FenneX ClippingNode was removed. Use cocos2d-x ClippingNode using a DrawNode as a stencil
* setSquareStencil in ClippingNode mimic the old behavior. It was simplified as well: use Panel::setClippingNode


Data structures:
----

* a lot of objects which are passed in array/dictionary need to be Clonable
* Vec2 (CCPoint) can't be passed anymore, separate X and Y instead
	* in Recognizers, replaced by std::map<int, Vec2>
	* macro Pcreate/TOPOINT use custom type TMPPoint (stop-gap) => mostly invisible for app, **except for isKindOfClass**
	
Info.plist:
----

* include it in the ios folder for easier maintenance
* be sure to set Executable file to ${EXECUTABLE_NAME}
* be sure to set Bundle name to ${PRODUCT_NAME}

Android main activity
----

* remove the static { System.loadLibrary("yourlibrary"); }
* add the metadata in application in AndroidManifest.xml, example :
<meta-data android:name="android.app.lib_name"
	              android:value="yourlibrary" />
	
	
Android removed support
----

Some features were removed during migration, they must be re-added:
* Expansion support must be a separate module
* VideoPlayer and VideoRecorder require a change on Cocos2d-x lib
* Label optimization was removed

Assets
----

* to accommodate Gradle, it is necessary that all the app assets be moved to an AppAssets folder (including ccbi, ipadhad, ipad, iphone folders, and any folder you need copied in the app)
	
Android.mk
----

* update to new script (take example on SampleProject) to resolve .h/.cpp files

AppDelegate/AppMacros
----

* add BUILD_VERSION in AppMacros, should be Dev by default
* add STRINGIFY macro in AppMacros for BUILD_VERSION
* change code in AppDelegate to use it for displayStats

