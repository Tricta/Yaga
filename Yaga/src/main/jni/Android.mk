LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaloader
LOCAL_SRC_FILES := loader/loader.cpp
LOCAL_STATIC_LIBRARIES := libcxx
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaoperator
LOCAL_SRC_FILES := operator/main.cpp
LOCAL_STATIC_LIBRARIES := libcxx libxhook
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include jni/libcxx/Android.mk
include jni/libxhook/Android.mk