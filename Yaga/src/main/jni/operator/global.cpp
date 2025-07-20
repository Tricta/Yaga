#include "global.h"

JavaVM* g_vm = nullptr;
const char* g_appDataDirChars = nullptr;

JNIEnv* GetEnv() {
    if (!g_vm) {
        LOGE("JavaVM is null");
        return nullptr;
    }

    JNIEnv* env = nullptr;
    if (g_vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE("Failed to get JNIEnv");
        return nullptr;
    }
    return env;
}