LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := xhook
LOCAL_SRC_FILES := libxhook/libxhook/jni/xhook.c \
                   libxhook/libxhook/jni/xh_core.c \
                   libxhook/libxhook/jni/xh_elf.c \
                   libxhook/libxhook/jni/xh_jni.c \
                   libxhook/libxhook/jni/xh_log.c \
                   libxhook/libxhook/jni/xh_util.c \
                   libxhook/libxhook/jni/xh_version.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libxhook/libxhook/jni
LOCAL_CFLAGS := -Wall -Wextra -fvisibility=hidden
LOCAL_CONLYFLAGS := -std=c11
LOCAL_LDLIBS := -llog
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaloader
LOCAL_SRC_FILES := loader/loader.cpp
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := yagaoperator
LOCAL_SRC_FILES := operator/main.cpp operator/jniNativeMethod.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libxhook/libxhook/jni
LOCAL_STATIC_LIBRARIES := xhook
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)