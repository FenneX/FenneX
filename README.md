FenneX
=========

Introduction
--

FenneX goal is to allow faster development of cocos2d-x apps, by exposing native OS features as easy to use C++ API and by adding support classes for a specific workflow.

It currently uses the obsolete version 2.2 of cocos2d-x, and is going to undergo a lot of changes to make the interface easier to use and the code cleaner. The current version was originally not planned to be open-sourced. The goal of open-sourcing it is to provide samples for people trying to use native OS features. 

Building
--
To build, you need to take example on the sample project (not yet available, will be posted after V3 release).

Support
--
Building tested on Mac OS X 10.8 and 10.9 with Xcode 5.1 and NDK r9 and r9d
Runtime iOS 5.0+ and Android 2.3.3+

Roadmap
--
* update to cocos2d-x V3.2
* provide a sample project
* refactor the API
* switch to a library
* reduce duplicates with cocos2d-x
* develop a test project

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
* Alexandre Mili
* Vincent Te

Acknoledgements
--
FenneX include several other open-source projects, without which it wouldn't be possible:
* Cocos2d-x: http://cocos2d-x.org/
* SuperAnim: http://raymondlu1983.blog.com/super-animation-converter/
* AnalyticX: https://github.com/diwu/AnalyticX
* pugixml: http://pugixml.org/

If you think you should appear here, please open an issue and I'll correct it.

Auticiel
--
The company behind FenneX, Auticiel, is recruiting an experienced developer in Paris, France. Contact us at jobs[at]auticiel[dot]com
