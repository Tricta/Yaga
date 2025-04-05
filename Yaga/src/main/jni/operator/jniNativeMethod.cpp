#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>

#include "jniNativeMethod.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Yaga", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Yaga", __VA_ARGS__)

void nativeForkAndSpecialize_pre(JNIEnv *env, jclass clazz, jint uid, jint gid,
    jintArray gids,
    jint runtime_flags, jobjectArray rlimits,
    jint mount_external, jstring se_info, jstring se_name,
    jintArray fdsToClose, jintArray fdsToIgnore,
    jboolean is_child_zygote,
    jstring instructionSet, jstring appDataDir) {
    
    //int id = uid % 100000;
    //if (id < 10000 || id > 19999) return;

    LOGI("forkAndSpecializePre");
}

void nativeForkAndSpecialize_post(JNIEnv *env, jclass clazz, jint uid, jint res) {
    //int id = uid % 100000;
    //if (id < 10000 || id > 19999) return;

    LOGI("forkAndSpecializePost");
}

jint nativeForkAndSpecialize_p(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids,
    jint runtime_flags, jobjectArray rlimits, jint mount_external,
    jstring se_info, jstring se_name, jintArray fdsToClose,
    jintArray fdsToIgnore, jboolean is_child_zygote,
    jstring instructionSet, jstring appDataDir) {

    nativeForkAndSpecialize_pre(env, clazz, uid, gid, gids, runtime_flags, rlimits, mount_external,
        se_info, se_name, fdsToClose, fdsToIgnore, is_child_zygote,
        instructionSet, appDataDir);

    jint res = ((nativeForkAndSpecialize_p_t) _nativeForkAndSpecialize)(env, clazz, uid, gid, gids,
        runtime_flags, rlimits,
        mount_external, se_info,
        se_name, fdsToClose,
        fdsToIgnore,
        is_child_zygote,
        instructionSet, appDataDir);

    nativeForkAndSpecialize_post(env, clazz, uid, res);

    return res;
}