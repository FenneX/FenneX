# Read if you landed here

cocos2d-x is not maintained since 2016. At the time it used openssl 1.1.0c, which does not have an Android build script.
Google Play Store now requires something that force us to build with a new NDK and thus build OpenSSL with new options.
That's why it was needed to build openssl again, and we used openssl 1.1.1w. There seems to be no meaningful change to headers so that version .a files can be used without changing headers
Since we only use armeabi-V7a and arm64-v8a, only those were rebuilt. armeabi is ancient and has not been used in forever, and x86 is a tiny portion of actual devices and not worth building for. I'm not even sure it's still used, as there is now x86-64.
