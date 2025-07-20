#include "nativeHooking.h"

#include "global.h"
#include "../logUtils.h"

void *(*orig_android_dlopen_ext)(const char *_Nullable __filename, int __flags, const android_dlextinfo *_Nullable __info, const void* caller_addr);
NativeFunc orig_nativeFunc = nullptr;

int64_t hooked_nativeFunc(char* arg1) {
    JNIEnv* env = GetEnv();
    if (env == nullptr) return (int64_t)orig_nativeFunc(arg1);

    jstring hackedStr = env->NewStringUTF("Hacked");
    return reinterpret_cast<int64_t>(hackedStr);
}

void *hooked_android_dlopen_ext(const char *_Nullable __filename, int __flags, const android_dlextinfo *_Nullable __info, const void* caller_addr)
{
    if (strstr(__filename, "libdummy3.so")) {
        LOGI("android_dlopen_ext: %s flags: %08x", __filename, __flags);

        void* res = orig_android_dlopen_ext(__filename, __flags, __info, caller_addr);

        LOGI("Manual base address of libdummy3.so: %p", res);

        void* sym = dlsym(res, "Java_com_example_dummy3_MainActivity_stringFromNativeCode");

        if (sym) {
            LOGI("Manual sym address of Java_com_example_dummy3_MainActivity_stringFromNativeCode: %p", sym);
            if (DobbyHook(sym, (void*)hooked_nativeFunc, (void**)&orig_nativeFunc) == 0) {
                LOGI("Hooked NativeFunc");
            } else {
                LOGE("Failed hooking NativeFunc");
            }
        } else {
            LOGE("Failed to resolve NativeFunc symbol");
        }

        return res;
    }

    return orig_android_dlopen_ext(__filename, __flags, __info, caller_addr);
}