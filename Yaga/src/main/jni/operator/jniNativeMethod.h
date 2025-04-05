extern void* _nativeForkAndSpecialize;

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