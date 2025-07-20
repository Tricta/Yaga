#include "artMethodHooking.h"

#include <jni.h>
#include <android/log.h>

#include "global.h"
#include "../logUtils.h"

int (*orig_doCall[4])(artMethod* arg1, void* arg2, void* arg3, void* arg4, int arg5, void* arg6) = {nullptr, nullptr, nullptr, nullptr};
std::string(*orig_PrettyMethod)(void* arg1, bool arg2) = nullptr;

std::string PrettyMethod(void* arg1, bool arg2){
    return orig_PrettyMethod(arg1, arg2);
}

jobject loadDexAndGetMethod(JNIEnv* env, const char* workingDir) {
    LOGI("loading Dex and get new method...");
    char dexFullPathStr[PATH_MAX];
    snprintf(dexFullPathStr, sizeof(dexFullPathStr), "%s/mydex.dex", workingDir);

    jclass dexLoaderClass = env->FindClass("dalvik/system/DexClassLoader");
    jmethodID dexLoaderInit = env->GetMethodID(dexLoaderClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");

    jclass contextClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentApplication = env->GetStaticMethodID(contextClass, "currentApplication", "()Landroid/app/Application;");
    jobject app = env->CallStaticObjectMethod(contextClass, currentApplication);

    jclass appClass = env->GetObjectClass(app);
    jmethodID getClassLoader = env->GetMethodID(appClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject appClassLoader = env->CallObjectMethod(app, getClassLoader);

    jstring dexPathStr = env->NewStringUTF(dexFullPathStr);

    char cacheFullPathStr[PATH_MAX];
    snprintf(cacheFullPathStr, sizeof(cacheFullPathStr), "%s/code_cache", workingDir);
    jstring odexDirStr = env->NewStringUTF(cacheFullPathStr);

    jobject dexClassLoader = env->NewObject(dexLoaderClass, dexLoaderInit, dexPathStr, odexDirStr, NULL, appClassLoader);

    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID loadClassMethod = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring className = env->NewStringUTF("com.example.dummy3.mydex");
    jobject loadedClass = env->CallObjectMethod(dexClassLoader, loadClassMethod, className);

    return (jclass)loadedClass;
}

jobject getMethodObject(JNIEnv* env, jclass clazz, const char* methodName, const char* methodSig) {
    jclass classClass = env->FindClass("java/lang/Class");
    jmethodID getMethod = env->GetMethodID(classClass, "getDeclaredMethod", "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");

    jstring nameStr = env->NewStringUTF(methodName);
    jobjectArray emptyParams = env->NewObjectArray(0, env->FindClass("java/lang/Class"), NULL);

    return env->CallObjectMethod(clazz, getMethod, nameStr, emptyParams);
}

void* hookARTMethod(JNIEnv* env, jobject targetObject){
    void* hookedARTMethod = nullptr;

    jclass executableClass = env->FindClass("java/lang/reflect/Executable");
    jfieldID artMethodID = env->GetFieldID(executableClass, "artMethod", "J");

    hookedARTMethod = (void*) env->GetLongField(targetObject, artMethodID);

    return hookedARTMethod;
}

int hooked_doCall_indexed(int index, artMethod* arg1, void* arg2, void* arg3, void* arg4, int arg5, void* arg6) {
    std::string pretty = PrettyMethod(arg1, 0);
    //LOGI("hooked doCall[%d]: %s", index, pretty.c_str());
    
    if (pretty.find("com.example.dummy3") != std::string::npos) {
        LOGI("dummy3 method called: %s", pretty.c_str());
        
        if (pretty == "com.example.dummy3.MainActivity.isRoot") {
            JNIEnv* env = GetEnv();
            if (env == nullptr) return orig_doCall[index](arg1, arg2, arg3, arg4, arg5, arg6);

            char dexPathStr[PATH_MAX];
            snprintf(dexPathStr, sizeof(dexPathStr), "%s/files", g_appDataDirChars);

            jclass newDexClass = (jclass)loadDexAndGetMethod(env, dexPathStr);
            jobject newMethodObj = getMethodObject(env, newDexClass, "isRoot", "()Ljava/lang/Boolean;");

            LOGI("patching original method: %s", pretty.c_str());
            artMethod* newArtMethod = (artMethod*) hookARTMethod(env, newMethodObj);
            return orig_doCall[index](newArtMethod, arg2, arg3, arg4, arg5, arg6);
        }
    }
    
    return orig_doCall[index](arg1, arg2, arg3, arg4, arg5, arg6);
} 

int hooked_doCall_0(artMethod* a, void* b, void* c, void* d, int e, void* f) { return hooked_doCall_indexed(0, a, b, c, d, e, f); }
int hooked_doCall_1(artMethod* a, void* b, void* c, void* d, int e, void* f) { return hooked_doCall_indexed(1, a, b, c, d, e, f); }
int hooked_doCall_2(artMethod* a, void* b, void* c, void* d, int e, void* f) { return hooked_doCall_indexed(2, a, b, c, d, e, f); }
int hooked_doCall_3(artMethod* a, void* b, void* c, void* d, int e, void* f) { return hooked_doCall_indexed(3, a, b, c, d, e, f); }