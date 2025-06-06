#include <jni.h>
#include <string>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <elf.h>
#include <android/log.h>
#include <vector>

#include "libxhook/xhook.h"
#include "jniNativeMethod.h"
#include "../logUtils.h"

void *_nativeForkAndSpecialize = NULL;
void *_nativeForkSystemServer = NULL;
void *_nativeSpecializeAppProcess = NULL;

#define XHOOK_REGISTER(PATH_REGEX, NAME) \
    if (xhook_register(PATH_REGEX, #NAME, (void*) new_##NAME, (void **) &old_##NAME) == 0) \
        LOGI("Registered hook " #NAME); \

#define NEW_FUNC_DEF(ret, func, ...) \
    static ret (*old_##func)(__VA_ARGS__); \
    static ret new_##func(__VA_ARGS__)

NEW_FUNC_DEF(int, _ZN7android14AndroidRuntime21registerNativeMethodsEP7_JNIEnvPKcPK15JNINativeMethodi, JNIEnv *env, const char *className, const JNINativeMethod *methods, int numMethods) {
    auto newMethods = std::make_unique<JNINativeMethod[]>(numMethods);
    memcpy(newMethods.get(), methods, sizeof(JNINativeMethod) * numMethods);

    JNINativeMethod method;
    for (int i = 0; i < numMethods; ++i) {
        method = methods[i];

        if (strcmp("com/android/internal/os/Zygote", className) == 0) {
            if (strcmp(method.name, "nativeForkAndSpecialize") == 0) {
                _nativeForkAndSpecialize = method.fnPtr;
                newMethods[i].fnPtr = (void *) nativeForkAndSpecialize_p;
                LOGI("replaced nativeForkAndSpecialize");
            } else if (strcmp(method.name, "nativeSpecializeAppProcess") == 0) {
                _nativeSpecializeAppProcess = method.fnPtr;
                newMethods[i].fnPtr = (void *) nativeSpecializeAppProcess;
                LOGI("replaced nativeSpecializeAppProcess");
            } else if (strcmp(method.name, "nativeForkSystemServer") == 0) {
                _nativeForkSystemServer = method.fnPtr;
                newMethods[i].fnPtr = (void *) nativeForkSystemServer;
                LOGI("replaced nativeForkSystemServer");
            }
        }
    }

    return old__ZN7android14AndroidRuntime21registerNativeMethodsEP7_JNIEnvPKcPK15JNINativeMethodi(env, className, newMethods.get(), numMethods);;
}

__attribute__((constructor)) __attribute__((used)) void OnLoad() {
    LOGI("app-lib running!!! pid=%d", getpid());

    XHOOK_REGISTER(".*\\libandroid_runtime.so$", _ZN7android14AndroidRuntime21registerNativeMethodsEP7_JNIEnvPKcPK15JNINativeMethodi);

    if (xhook_refresh(0) == 0) {
        xhook_clear();
        LOGI("hook installed");
    } else {
        LOGE("failed to refresh hook");
    }
}