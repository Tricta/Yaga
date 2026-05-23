#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <thread>
#include <unistd.h>

#include <string>
#include <vector>

#include "jniNativeMethod.h"
#include "xhook.h"
#include "../logUtils.h"

void nativeForkAndSpecialize_pre(JNIEnv *env, jclass clazz, jint uid, jint gid,
    jintArray gids,
    jint runtime_flags, jobjectArray rlimits,
    jint mount_external, jstring se_info, jstring se_name,
    jintArray fdsToClose, jintArray fdsToIgnore,
    jboolean is_child_zygote,
    jstring instructionSet, jstring appDataDir) {
    
    LOGI("forkAndSpecializePre");
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

static void nativeSpecializeAppProcess_post(JNIEnv *env, jclass clazz, jstring niceName, jstring appDataDir) {
    LOGI("nativeSpecializeAppProcess_post");

    const char* packageNameChars = env->GetStringUTFChars(niceName, nullptr);
    const char* appDataDirChars  = env->GetStringUTFChars(appDataDir, nullptr);

    LOGI("App package name: %s", packageNameChars);
    LOGI("App data dir: %s", appDataDirChars);

    env->ReleaseStringUTFChars(niceName, packageNameChars);
    env->ReleaseStringUTFChars(appDataDir, appDataDirChars);
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

    nativeSpecializeAppProcess_post(env, clazz, niceName, appDataDir);
}