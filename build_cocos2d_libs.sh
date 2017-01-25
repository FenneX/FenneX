cd cocos2d
LOCAL_PATH=$(pwd)

#sdks should be iphoneos and iphonesimulator
libs=""
for sdk in $(xcodebuild -showsdks | grep -o '\biphone[a-z]*')
do
    #Create a lib for each sdk
    xcodebuild -project build/cocos2d_libs.xcodeproj -sdk ${sdk} -configuration "Debug" -target "libcocos2d iOS" CONFIGURATION_BUILD_DIR=$LOCAL_PATH/cocos-lib/
    mv "$LOCAL_PATH/cocos-lib/libcocos2d iOS.a" "$LOCAL_PATH/cocos-lib/libcocos2d${sdk}.a"
    libs="${libs} $LOCAL_PATH/cocos-lib/libcocos2d${sdk}.a"
done

# create a fat library with both iphoneOS (armv6, arm64) and iphoneSimulator (i386, x86_64)
lipo -create $libs -output "$LOCAL_PATH/cocos-lib/libcocos2d.a"

#Build Android lib. To do that, build an empty project and get the compiled .a from the obj folder
#This is far from effective, but it works. It should be migrated to cocos gen-lib when possible, see http://discuss.cocos2d-x.org/t/how-to-speed-up-cocos2d-x-build-with-prebuilt-lib/23402
# "debug" can be replaced by "release" for release lib. For now, the lib should only be used during debug, release apps get a full compilation from scratch
python build/android-build.py cpp-empty-test -b debug

#Compile all those into a single prebuilt lib per architecture
#Be sure to match what's inside APP_ABI in file tests/cpp-empty-test/proj.android/jni/Application.mk (default to armeabi)
#Also modify your Android.mk to match target application
ar=$(ndk-which ar)
for arch in armeabi-v7a x86
do
mkdir -p cocos-lib/${arch}
for dir in box2d_static cocos_extension_static cocos_network_static cocos_ui_static cocos2dx_internal_static cocos2dxandroid_static cocosbuilder_static cpufeatures spine_static
do
    ${ar} rs cocos-lib/${arch}/libcocos2d-prebuilt.a $(find ${LOCAL_PATH}/tests/cpp-empty-test/proj.android/obj/local/${arch}/objs-debug/${dir} -name *.o)
done
done

#Cleanup build directories
for sdk in $(xcodebuild -showsdks | grep -o '\biphone[a-z]*')
do
    rm "$LOCAL_PATH/cocos-lib/libcocos2d${sdk}.a"
done
rm -rf ${LOCAL_PATH}/build/build
rm -rf ${LOCAL_PATH}/tests/cpp-empty-test/proj.android/bin
rm -rf ${LOCAL_PATH}/tests/cpp-empty-test/proj.android/gen
rm -rf ${LOCAL_PATH}/tests/cpp-empty-test/proj.android/obj