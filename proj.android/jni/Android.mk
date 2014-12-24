LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := yourlibrary_shared

LOCAL_MODULE_FILENAME := libyourlibrary

#use a find command to have all cpp files in Classes and in jni (current folder)
CPP_FILES := $(shell find $(LOCAL_PATH)/../../Classes -name "*.cpp")
JNI_CPP_FILES := $(shell find $(LOCAL_PATH) -name "*.cpp")
LOCAL_SRC_FILES := $(JNI_CPP_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(CPP_FILES:$(LOCAL_PATH)/%=%)

#use a find command to have all header files in Classes
HEADER_FILES := $(shell find $(LOCAL_PATH)/../../Classes -type d)
LOCAL_C_INCLUDES := $(HEADER_FILES:$(LOCAL_PATH)/%=%)

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES := cocos_ui_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dxandroid_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_CXXFLAGS += -fexceptions

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
$(call import-module,editor-support/cocosbuilder)
$(call import-module,editor-support/spine)
# $(call import-module,editor-support/cocostudio)
$(call import-module,network)
$(call import-module,extensions)
