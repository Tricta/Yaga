#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

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
    
    LOGI("forkAndSpecializePre");
}

void* my_thread_func(void* args) {
    while (1) {
        LOGI("TEST MTF!!!");
        sleep(1);
    }
    return NULL;
}

void nativeForkAndSpecialize_post(JNIEnv *env, jclass clazz, jint uid, jint res) {
    if (res == 0) {
        LOGI("forkAndSpecializePost");
    }
}

jint nativeForkAndSpecialize_p(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids,
    jint runtime_flags, jobjectArray rlimits, jint mount_external,
    jstring se_info, jstring se_name, jintArray fdsToClose,
    jintArray fdsToIgnore, jboolean is_child_zygote,
    jstring instructionSet, jstring appDataDir) {;
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

void nativeForkSystemServer_pre(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids,
    jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities,
    jlong effectiveCapabilities) {
    
    LOGI("nativeForkSystemServerPre");
}

void nativeForkSystemServer_post(JNIEnv *env, jclass clazz, jint res) {
    LOGI("nativeForkSystemServerPost");
}

jint nativeForkSystemServer(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids,
    jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities,
    jlong effectiveCapabilities) {

    nativeForkSystemServer_pre(env, clazz, uid, gid, gids, debug_flags, rlimits,
       permittedCapabilities,
       effectiveCapabilities);

    jint res = ((nativeForkSystemServer_t) _nativeForkSystemServer)(env, clazz, uid, gid, gids,
                                        debug_flags, rlimits,
                                        permittedCapabilities,
                                        effectiveCapabilities);

    nativeForkSystemServer_post(env, clazz, res);

    return res;
}

static void nativeSpecializeAppProcess_pre(
    JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtimeFlags,
    jobjectArray rlimits, jint mountExternal, jstring seInfo, jstring niceName,
    jboolean startChildZygote, jstring instructionSet, jstring appDataDir, jstring packageName,
    jobjectArray packagesForUID, jstring sandboxId) {

    LOGI("nativeSpecializeAppProcess_pre");
}

static void nativeSpecializeAppProcess_post(JNIEnv *env, jclass clazz) {
    LOGI("nativeSpecializeAppProcess_post");
}

void nativeSpecializeAppProcess(
    JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtimeFlags,
    jobjectArray rlimits, jint mountExternal, jstring seInfo, jstring niceName,
    jboolean startChildZygote, jstring instructionSet, jstring appDataDir, jstring packageName,
    jobjectArray packagesForUID, jstring sandboxId) {
        
    nativeSpecializeAppProcess_pre(
            env, clazz, uid, gid, gids, runtimeFlags, rlimits, mountExternal, seInfo, niceName,
            startChildZygote, instructionSet, appDataDir, packageName, packagesForUID,
            sandboxId);

    ((nativeSpecializeAppProcess_t) _nativeSpecializeAppProcess)(
            env, clazz, uid, gid, gids, runtimeFlags, rlimits, mountExternal, seInfo, niceName,
            startChildZygote, instructionSet, appDataDir, packageName, packagesForUID,
            sandboxId);

    nativeSpecializeAppProcess_post(env, clazz);
}