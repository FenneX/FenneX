FenneX
=========

Introduction
--

FenneX goal is to allow faster development of cocos2d-x apps, by exposing native OS features as easy to use C++ API and by adding support classes for a specific workflow.

It currently uses V3.2 of cocos2d-x, and is going to undergo a lot of changes to make the interface easier to use and the code cleaner. The current version was originally not planned to be open-sourced. The goal of open-sourcing it is to provide samples for people trying to use native OS features. 

Building
--
To build, you need to take example on the sample project at https://github.com/FenneX/FenneXEmptyProject

Support
--
Building tested on Mac OS X 10.8 and 10.9 with Xcode 5.1 and NDK r9d
Runtime iOS 5.0+ and Android 2.3.3+

Roadmap
--
* upgrade to Modern C++ and remove most deprecated warnings
* refactor the API
* switch to a library
* reduce duplicates with cocos2d-x
* develop a test project
* separate wrappers as modules you can activate/deactivate

Changelog from V2.2
--
* Updated to cocos2d-x V3.2
* Removed SuperAnim (not working on V3.2, really hard to port)
* ExpansionSupport Java files removed because of dependencies to other projects. Will be re-added as a module
* VideoPlayer and VideoRecorder support removed on Android. To be re-added in the future
* Switched to Gradle for Android compilation


Q/A
--

##### What's the origin of Fennex?
FenneX is a "scratch your own itch" framework, and as such may not be useful to lots of persons. The basis dates back from before cocos2d-iphone V1, and as such a lot of methods have become redundant with current cocos2d-x implementation.

##### Why does it include a full cocos2d-x version?
FenneX includes a full customized version of cocos2d-x, because it requires additionnal methods not present in standard cocos2d-x. The full list of modification by file can be found in cocos2d-xmodifications.txt

##### How about building on other platforms?
You are welcomed to try to build on other platforms and report the result and what's needed.

##### How about other OS (Windows Phone, Mac, Linux, Windows ...) that cocos2d-x support?
Other deploy platforms are not planned anytime soon, as it requires a lot of effort to port all the native wrappers

##### How can I contribute?
If you notice a bug, please open an issue
If you fixed a bug or added a feature, send a pull request, explaining what you fixed/added.
All code must follow the same coding style as the rest of FenneX

Contributors
--
* François Dupayrat
* Pierre Bertinet
* Alexandre Mili
* Vincent Te

Acknoledgements
--
FenneX include several other open-source projects, without which it wouldn't be possible:
* Cocos2d-x: http://cocos2d-x.org/
* AnalyticX: https://github.com/diwu/AnalyticX
* pugixml: http://pugixml.org/

If you think you should appear here, please open an issue and I'll correct it.

Auticiel
--
The company behind FenneX, Auticiel, is recruiting an experienced developer in Paris, France. Contact us at jobs[at]auticiel[dot]com
