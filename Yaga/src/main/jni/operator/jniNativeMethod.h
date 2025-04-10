extern void* _nativeForkAndSpecialize;
extern void* _nativeForkSystemServer;
extern void *_nativeSpecializeAppProcess;

const static char* nativeForkAndSpecialize_p_sig = "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;)I";

typedef jint (*nativeForkAndSpecialize_p_t)(JNIEnv*, jclass, jint, jint, jintArray, jint, jobjectArray,
                jint, jstring, jstring, jintArray, jintArray, jboolean,
                jstring, jstring);

extern jint nativeForkAndSpecialize_p(JNIEnv *env, jclass clazz, jint uid, jint gid,
                jintArray gids,
                jint runtime_flags, jobjectArray rlimits,
                jint mount_external, jstring se_info, jstring se_name,
                jintArray fdsToClose, jintArray fdsToIgnore,
                jboolean is_child_zygote,
                jstring instructionSet, jstring appDataDir);

const static char* nativeForkSystemServer_sig = "(II[II[[IJJ)I";

typedef jint (*nativeForkSystemServer_t)(JNIEnv*, jclass, uid_t, gid_t, jintArray,
                                            jint, jobjectArray, jlong, jlong);

extern jint nativeForkSystemServer(JNIEnv* env, jclass, uid_t uid, gid_t gid, jintArray gids,
                                    jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities,
                                    jlong effectiveCapabilities);


const static char *nativeSpecializeAppProcess_sig = "(II[II[[IILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)V";

typedef void (*nativeSpecializeAppProcess_t)(
        JNIEnv *, jclass, jint, jint, jintArray, jint, jobjectArray, jint, jstring, jstring,
        jboolean, jstring, jstring, jstring, jobjectArray, jstring);

extern void nativeSpecializeAppProcess(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtimeFlags,
    jobjectArray rlimits, jint mountExternal, jstring seInfo, jstring niceName,
    jboolean startChildZygote, jstring instructionSet, jstring appDataDir, jstring packageName,
    jobjectArray packagesForUID, jstring sandboxId);