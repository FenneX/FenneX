APP_STL := c++_static
NDK_TOOLCHAIN_VERSION=clang

APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=0 -std=c++11 -fsigned-char -Wno-nonportable-include-path
APP_LDFLAGS := -latomic
APP_ALLOW_MISSING_DEPS=true


APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
  APP_ABI :=  armeabi-v7a arm64-v8a
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
  APP_ABI :=  armeabi-v7a arm64-v8a
endif