Modifications by Cocos2d versions
====

All paths are from cocos2d/ folder

Cocos2d-x V3.2 Modifications
====

* build/cocos2d_lib/xcodeproj/project.pbxproj => updated project using Xcode suggestion
* cocos/math/CCGeometry.h => Size now subclass Ref (to allow using it in structures)
* [REMOVED]cocos/math/Vec2.h => Vec2 now subclass Ref (to allow using it in structures) => **don't do that, since it causes problem with DrawNode**
* cocos/2d/actions/CCActionManager.h/.cpp => add Vector<Action\*> ActionManager::getAllActionsForTarget(const Node* target)
* cocos/2d/actions/CCActionInterval.h/.cpp => add FiniteTimeAction\* getCurrentAction() const; and FiniteTimeAction\* getNextAction() const;
* cocos/2d/actions/CCActionInstant.h/.cpp => add SEL_CallFuncO getSelectorTarget() const;
* cocos/renderer/CCTextureCache.h/.cpp => add std::string getKeyForTexture(Texture2D* tex) const;
* cocos/deprecated/CCArray.cpp => add lazy init in addObject, addObjectsFromArray and insertObject to avoid crash
* cocos/deprecated/CCArray.cpp => secure containsObject, remove*, count, capacity for no data
* cocos/deprecated/CCArray.h => secure CC_ARRAY_FOREACH to work with no data
* cocos/2d/misc-nodes/CCClippingNode.h/.cpp => subclass CCActionTweenDelegate to be able to change width/height, add setSquareStencil() to use a square the size of ContentSize
* cocos/base/CCValue.h/.cpp => add VEC2 type
* cocos/platform/android/java/Cocos2dxActivity.java => change GLSurfaceView EGLConfigChooser for real devices (for ClippingNode)
* cocos/base/CCDirector.h/.cpp => add support for build version via setDisplayStats
* cocos/editor-support/cocosbuilder/CCNodeLoader.cpp => fix a problem with CCScale9Sprite preferedSize
* extensions/GUI/CCeditBox/CCEditBox.cpp => fix scaling problems for fontSize and contentSize
* extensions/GUI/CCeditBox/* => Add support for keyboard return type "Next"
* extensions/GUI/CCeditBox/* => editBoxReturn delegate is now called only for return button on iOS
* network/* => add POSTFILE request type


Cocos2d-x V3.3 Modifications
====

* cocos/math/CCGeometry.h => Size now subclass Ref (to allow using it in structures)
* cocos/2d/actions/CCActionManager.h/.cpp => add Vector<Action\*> ActionManager::getAllActionsForTarget(const Node* target)
* cocos/2d/actions/CCActionInterval.h/.cpp => add FiniteTimeAction\* getCurrentAction() const; and FiniteTimeAction\* getNextAction() const;
* cocos/2d/actions/CCActionInstant.h/.cpp => add SEL_CallFuncO getSelectorTarget() const;
* cocos/renderer/CCTextureCache.h/.cpp => add std::string getKeyForTexture(Texture2D* tex) const;
* cocos/deprecated/CCArray.cpp => add lazy init in addObject, addObjectsFromArray and insertObject to avoid crash
* cocos/deprecated/CCArray.cpp => secure containsObject, remove*, count, capacity for no data
* cocos/deprecated/CCArray.h => secure CC_ARRAY_FOREACH to work with no data
* cocos/2d/misc-nodes/CCClippingNode.h/.cpp => subclass CCActionTweenDelegate to be able to change width/height, add setSquareStencil() to use a square the size of ContentSize
* cocos/base/CCValue.h/.cpp => add VEC2 type
* cocos/base/CCDirector.h/.cpp => add support for build version via setDisplayStats
* cocos/editor-support/cocosbuilder/CCNodeLoader.cpp => fix a problem with CCScale9Sprite preferedSize
* extensions/GUI/CCeditBox/CCEditBox.cpp => fix scaling problems for fontSize and contentSize
* ui/UIEditBox//* => Add support for keyboard return type "Next"
* ui/UIEditBox/UIEditBox.h/.cpp => add closeKeyboard()
* network/* => add POSTFILE request type
* ui/UIEditBox/UIEditBox.h => add getBackgroundSprite() method
* cocos/base/ccConfig.h => disable physics, tiff, webp, script binding
* build/cocos2d_libs.xcodeproj/project.pbxproj => add armv7s architecture
* cocos/platform/android/* (modified Android.mk) => add cpu-feature.h/cpp from ndk-r10d because of compilation problem
* cocos/Android.mk => disable tiff, webp, chipmunk and cocostudio
* cocos/ui/UIEditBox.cpp => add editBoxEditingWillBegin to UIEditBoxDelegate
* cocos/ui/UIEditBox.cpp => change openKeyboard to throw editBoxEditingDidBegin after Java Dialog creation
* cocos/ui/UIEditBox.cpp + cocos/platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h/.cpp + cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dx[Helper/Handler/Activity].java => implement closeKeyboard
* cocos/ui/UIEditBox/UIEditBoxImpl-android.cpp => change editBoxCallbackFunc to test back if it's a cancell key 
* cocos/base/CCEventCustom.h/cpp => add EventCustom::create
* cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHelper.java remove if (!sInited) condition in init procedure
* external/unzip.h => change include to "platform/CCPlatformConfig.h"


Cocos2d-x V3.8.1 Modifications
====

* cocos/math/CCGeometry.h => Size now subclass Ref (to allow using it in structures)
* cocos/CCDirector -> add setDisplayStatsWithBuild
* cocos/renderer/CCTextureCache.h/.cpp => add std::string getKeyForTexture(Texture2D* tex) const;
* cocos/base/CCDirector.h/.cpp => add support for build version via setDisplayStats
* ui/UIEditBox/UIEditBox.h/.cpp => add closeKeyboard()
* cocos/base/CCEventCustom.h/cpp => add EventCustom::create
* external/unzip.h => change include to "platform/CCPlatformConfig.h"
* cocos/deprecated/CCArray.cpp => add lazy init in addObject, addObjectsFromArray and insertObject to avoid crash
* cocos/deprecated/CCArray.cpp => secure containsObject, remove*, count, capacity for no data
* cocos/deprecated/CCArray.h => secure CC_ARRAY_FOREACH to work with no data
* build/cocos2d_libs.xcodeproj/project.pbxproj => add armv7s architecture
* cocos/base/ccConfig.h => disable physics, tiff, webp, script binding
* cocos/Android.mk => disable tiff, webp, chipmunk and cocostudio
* cocos/Android.mk => add cpu-feature
* extanstions/android.mk => remove all Particle3D files from LOCAL_SRC_FILES
* Remove all code between //Enhance comment and "import com.enhance.gameservice.IGameTuningService;" in cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHelper.java
* cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHelper.java remove if (!sInited) condition in init procedure
* ui/UIEditBox/UIEditBox.h => define int values of InputMode enum
* ui/UIEditBox/UIEditBox.h/cpp => add getBackgroundSprite(), getFontColor and getPlaceholderFontColor methods
* ui/UIEditBox/UIEditBox.h/platform impl => add date and time pickers
* cocos/ui/UIEditBox.cpp => add editBoxEditingWillBegin to UIEditBoxDelegate
* extensions/GUI/CCeditBox/CCEditBox.cpp => fix scaling problems for fontSize (not contentSize anymore)
* Scale9Sprite::updateWithSprite  => comment _preferredSize = size; cause it erase information for some reason.
* extensions/GUI/CCeditBox/* => Add support for keyboard return type "Next"
* network/HttpClient/HttpRequest => change _requestData from std::vector<char> to std::string and add inline void setRequestData(const std::string& data)
* network/* => add POSTFILE request type
* network/HttpClient-Apple.mm => remove forced Content-type: application/x-www-form-urlencoded for PUT requests
* Remove AudioEngine
* AppController::didFinishLaunchingWithOptions add eaglView.opaque = NO;
* cocos/ui/UIEditBox/UiEditBox.cpp and UIEditBoxImpl-common.cpp => fix fontSize and contentSize using nodeToWorldTransform correctly
* cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHttpURLConnection.java => change setVerifySSL to return a boolean with true when succeed and false otherwise
* cocos/network/HttpClient-android.cpp => change setVerifySSL to return a boolean with true when succeed and false otherwise
* cocos2D/build/cocos2d_libs.xcodeproj/project.pbxproj => Update to Xcode 8.0 project, inhibit all warnings
* cocos/network/HttpAsynConnection-apple.m  => Fix crash issue due to a release (backported from cocos2d-x 3.11, commit 200c9fda59f7a5fbece4ba4fa3a3729e38de6998)

Cocos2d-x V3.16 Modifications
====

* cocos/math/CCGeometry.h => Size now subclass Ref (to allow using it in structures)
* cocos/base/CCDirector.h/.cpp => add support for build version via setDisplayStats (setDisplayStatsWithBuild, with add of _buildLabel and _buildLabelString)
* cocos/renderer/CCTextureCache.h/.cpp => add std::string getKeyForTexture(Texture2D* tex) const;
* cocos/base/CCEventCustom.h/cpp => add EventCustom::create
* cocos/base/ccConfig.h => disable physics, tiff, webp, script binding
* cocos/Android.mk => disable tiff, webp, chipmunk and cocostudio
* cocos/Android.mk => add cpu-feature
* cocos/base/ccConfig.h => disable physics, tiff, webp, script binding
* extensions/Android.mk => remove all Particle3D files from LOCAL_SRC_FILES
* Remove all code between //Enhance comment and "import com.enhance.gameservice.IGameTuningService;" in cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHelper.java
* cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHelper.java remove if (!sInited) condition in init procedure
* network/HttpClient/HttpRequest => change _requestData from std::vector<char> to std::string and add inline void setRequestData(const std::string& data)
* network/* => add POSTFILE request type (include the add of set/getFilePath)
* Remove AudioEngine
* cocos/platform/android/java/src/org/cocos2dx/lib/Cocos2dxHttpURLConnection.java => change setVerifySSL to return a boolean with true when succeed and false otherwise
* cocos/network/HttpClient-android.cpp => change setVerifySSL to return a boolean with true when succeed and false otherwise
* cocos/2d/misc-nodes/CCClippingNode.h/.cpp => subclass CCActionTweenDelegate to be able to change width/height, add setSquareStencil() to use a square the size of ContentSize
* Moves all libs from cocos/platform/android/java/libs/ to proj.android/libs/
* Remove AudioFocusManager and all usages in cocos2dxActivity
* ui/UIEditBox/UIEditBox.h/.cpp => add closeKeyboard()
* ui/UIEditBox/UIEditBox.h/platform impl => add date and time pickers
* cocos/ui/UIEditBox.cpp => add editBoxEditingWillBegin to UIEditBoxDelegate
* remove web folder and templates folder
* cocos/2d/CCRenderTexture.cpp => in saveToFile, avoid using writable path when it's already an absolute path
* cocos/platform/CCFileUtils.cpp => in renameFile, make the cclogerror useful by showing errno
