#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <thread>
#include <unistd.h>

#include <string>
#include <vector>

#include "jniNativeMethod.h"
#include "libxhook/xhook.h"
#include "dobby/dobby.h"
#include "libartHook.h"
#include "artMethodHooking.h"
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

    if (strcmp(packageNameChars, "com.example.dummy3") == 0) {
        if (!g_vm) {
            env->GetJavaVM(&g_vm);
            LOGI("Cached JavaVM: %p", g_vm);
        }

        if(!g_appDataDirChars){
            g_appDataDirChars = appDataDirChars;
            LOGI("Cached appDataDirChars: %p", g_appDataDirChars);
        }

        unsigned long int PrettyMethodAddr;

        if (find_name("_ZN3art9ArtMethod12PrettyMethodEPS0_b", "libart.so", &PrettyMethodAddr) < 0) {
            LOGI("can't find: _ZN3art9ArtMethod12PrettyMethodEPS0_b");
            return;
        }

        if (DobbyHook((void*)PrettyMethodAddr, (void*)PrettyMethod, (void**)&orig_PrettyMethod) == 0) {
            LOGI("PrettyMethod hook successful!");
        } else {
            LOGE("PrettyMethod hook failed.");
        }

        char* doCallSymbols[4] = {
            "_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE",
            "_ZN3art11interpreter6DoCallILb0ELb1EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE",
            "_ZN3art11interpreter6DoCallILb1ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE",
            "_ZN3art11interpreter6DoCallILb1ELb1EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE"
        };

        void* trampolines[] = { (void*)hooked_doCall_0, (void*)hooked_doCall_1, (void*)hooked_doCall_2, (void*)hooked_doCall_3 };

        for (int i = 0; i < 4; ++i) {
            unsigned long int doCallAddr;
            if (find_name(doCallSymbols[i], "libart.so", &doCallAddr) < 0) {
                LOGI("can't find: %s", doCallSymbols[i]);
            } else if (DobbyHook((void*)doCallAddr, trampolines[i], (void**)&orig_doCall[i]) == 0) {
                LOGI("doCall hook successful for: %s", doCallSymbols[i]);
            } else {
                LOGE("doCall hook failed for: %s", doCallSymbols[i]);
            }
        }
    }
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