LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libxhook
LOCAL_SRC_FILES := xhook.c \
                   xh_core.c \
                   xh_elf.c \
                   xh_jni.c \
                   xh_log.c \
                   xh_util.c \
                   xh_version.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := -Wall -Wextra -Werror -fvisibility=hidden -llog
LOCAL_CONLYFLAGS := -std=c11
include $(BUILD_STATIC_LIBRARY)