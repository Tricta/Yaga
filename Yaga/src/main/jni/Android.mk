LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaloader
LOCAL_SRC_FILES := loader/loader.cpp
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := local_dobby
LOCAL_SRC_FILES := dobby/libs/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaoperator
LOCAL_SRC_FILES := operator/main.cpp operator/jniNativeMethod.cpp operator/libartHook.cpp operator/artMethodHooking.cpp
LOCAL_STATIC_LIBRARIES := libxhook local_dobby
LOCAL_C_INCLUDES := dobby/libs
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include jni/libxhook/Android.mk