Migration progression
====

TODO
----

* Port SuperAnim to V3 or use Spine/CocoStudio/CocosBuilder
	*  probably a problem with new Renderer 
	*  sent message to Raymond (SuperAnim maintainer) => no answer
	* tried to modify draw() => doesn't work
	* posted on cocos2d-x forum => no answer
	

CAN BE DONE LATER
----

* use new Vector and Map structures
* switch to std::function
* use std::string everywhere possible, CCValue for the rest
* use CCValue instead of CCFloat, CCBool and CCString
* review all code to check that it follows new guidelines
* refactor stats using DrawNode
* re-add ExpansionSupport on Android as a module
* re-add VideoPlayer and VideoRecorder on Android