#APP_STL := stlport_static #STL port needed for std::sort
APP_STL := gnustl_static # default STL implementation
APP_CPPFLAGS := -frtti -std=c++11#not supported by stlport
APP_CFLAGS += -Wno-error=format-security
APP_PLATFORM := android-10
APP_ABI := armeabi armeabi-v7a x86
NDK_TOOLCHAIN_VERSION := 4.8
APP_OPTIM := debug
