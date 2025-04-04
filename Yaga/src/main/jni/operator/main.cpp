#include <jni.h>
#include <string>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <elf.h>
#include <android/log.h>
#include "libxhook/xhook.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygote Injection", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Zygote Injection", __VA_ARGS__)

#define XHOOK_REGISTER(PATH_REGEX, NAME) \
    if (xhook_register(PATH_REGEX, #NAME, (void*) new_##NAME, (void **) &old_##NAME) == 0) \
        LOGI("Registered hook " #NAME); \

#define NEW_FUNC_DEF(ret, func, ...) \
    static ret (*old_##func)(__VA_ARGS__); \
    static ret new_##func(__VA_ARGS__)

NEW_FUNC_DEF(int, _ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv, JNIEnv *env) {
    LOGI("register_com_android_internal_os_Zygote");

    int res = old__ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv(env);

    if(res >= 0){
        //get zygote java class
        jclass clazz = env->FindClass("com/android/internal/os/Zygote");

        if(!clazz){
            LOGE("class com/android/internal/os/Zygote not found");
        }else{
            LOGI("class com/android/internal/os/Zygote WORKED!!!");
        }
    }

    return res;
}

__attribute__((constructor)) __attribute__((used)) void OnLoad() {
    LOGI("app-lib running!!! pid=%d", getpid());

    XHOOK_REGISTER(".*\\libandroid_runtime.so$", _ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv);

    if (xhook_refresh(0) == 0) {
        xhook_clear();
        LOGI("hook installed");
    } else {
        LOGE("failed to refresh hook");
    }
}