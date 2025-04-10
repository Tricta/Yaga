#include <jni.h>
#include <string>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <elf.h>
#include <android/log.h>
#include <vector>

#include "libxhook/xhook.h"
#include "utils.h"
#include "jniNativeMethod.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Yaga", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Yaga", __VA_ARGS__)

#ifdef __LP64__
#define ANDROID_RUNTIME_LIBRARY "/system/lib64/libandroid_runtime.so"
#else
#define ANDROID_RUNTIME_LIBRARY "/system/lib/libandroid_runtime.so"
#endif

static int init_called = 0;
static JNINativeMethod gMethods[] = {{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}};
void *_nativeForkAndSpecialize = NULL;
void *_nativeForkSystemServer = NULL;
void *_nativeSpecializeAppProcess = NULL;

JNINativeMethod *search_method(int endian, std::vector<std::pair<uintptr_t, uintptr_t>> addresses, const char *name, size_t len) {
    //search for name
    uintptr_t str_addr = 0;
    for (auto address : addresses) {
        void *res = memsearch(address.first, address.second, (const void *) name, len);
        if (res) {
            str_addr = (uintptr_t) res;

            #ifdef __LP64__
            LOGI("found \"%s\" at 0x%lx", (char *) str_addr, str_addr);
            #else
            LOGI("found \"%s\" at 0x%x", (char *) str_addr, str_addr);
            #endif

            break;
        }
    }

    if (!str_addr) {
        LOGE("\" %s \" not found.", name);
        return NULL;
    }

    // search JNINativeMethod struct by address
    size_t size = sizeof(uintptr_t);
    unsigned char *data = new unsigned char[size];

    for (size_t i = 0; i < size; i++)
    data[endian == ELFDATA2LSB ? i : size - i - 1] = (unsigned char) (
            ((uintptr_t) 0xff << i * 8 & str_addr) >> i * 8);

    JNINativeMethod *method = NULL;
    for (auto address : addresses) {
        void *res = memsearch(address.first, address.second, data, size);
        if (res) {
            method = (JNINativeMethod *) res;

            #ifdef __LP64__
            LOGI("found {\"%s\", \"%s\", %p} at 0x%lx", method->name, method->signature,
                method->fnPtr, (uintptr_t) method);
            #else
            LOGI("found {\"%s\", \"%s\", %p} at 0x%x", method->name, method->signature,
                method->fnPtr, (uintptr_t) method);
            #endif

            break;
        }
    }
    if (!method) {
        LOGE("%s not found.", name);
        return NULL;
    }
    return method;
}

#define XHOOK_REGISTER(PATH_REGEX, NAME) \
    if (xhook_register(PATH_REGEX, #NAME, (void*) new_##NAME, (void **) &old_##NAME) == 0) \
        LOGI("Registered hook " #NAME); \

#define NEW_FUNC_DEF(ret, func, ...) \
    static ret (*old_##func)(__VA_ARGS__); \
    static ret new_##func(__VA_ARGS__)

NEW_FUNC_DEF(int, _ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv, JNIEnv *env) {
    LOGI("register_com_android_internal_os_Zygote");

    int res = old__ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv(env);

    if(res >= 0){
        //get zygote java class
        jclass clazz = env->FindClass("com/android/internal/os/Zygote");

        if(!clazz){
            LOGE("class com/android/internal/os/Zygote not found");
        }else{
            LOGI("class com/android/internal/os/Zygote WORKED!!!");

            if (init_called) {
                LOGI("init called");
                return 0;
            }

            //read elf file to get endian
            int endian;

            FILE *file = fopen(ANDROID_RUNTIME_LIBRARY, "r");
            #ifdef __LP64__
            Elf64_Ehdr header;
            #else
            Elf32_Ehdr header;
            #endif

            fread(&header, 1, sizeof(header), file);
            endian = header.e_ident[EI_DATA];

            fclose(file);

            //get android runtime
            std::vector<std::pair<uintptr_t, uintptr_t>> addresses;

            int fd = open("/proc/self/maps", O_RDONLY);
            if (fd == -1) {
                LOGE("open /proc/self/maps failed.");
                return 0;
            }
        
            #if __LP64__
                const char *s = "%lx-%lx %s %*s %*s %*s %s";
            #else
                const char *s = "%x-%x %s %*s %*s %*s %s";
            #endif
        
            char buf[512];
            while (fdgets(buf, 512, fd) > 0) {
                uintptr_t start = 0, end = 0;
                char flags[5], filename[128];
                if (sscanf(buf, s, &start, &end, flags, filename) != 4)
                    continue;
        
                if (strcmp(ANDROID_RUNTIME_LIBRARY, filename) == 0) {
                    addresses.push_back(std::pair<uintptr_t, uintptr_t>(start, end));
                    LOGI("%lx %lx %s %s", start, end, flags, filename);
                }
            }
            close(fd);

            JNINativeMethod *method[3];
            method[0] = search_method(endian, addresses, "nativeForkAndSpecialize", strlen("nativeForkAndSpecialize") + 1);
            method[1] = search_method(endian, addresses, "nativeForkSystemServer", strlen("nativeForkSystemServer") + 1);
            method[2] = search_method(endian, addresses, "nativeSpecializeAppProcess", strlen("nativeSpecializeAppProcess") + 1);
        
            if (!method[0] || !method[1] || !method[2]) {
                LOGE("JNI Native Method not found.");
                return 0;
            }

            //get jni methods pointer
            _nativeForkAndSpecialize = method[0]->fnPtr;
            gMethods[0].fnPtr = (void *) nativeForkAndSpecialize_p;
            gMethods[0].name = method[0]->name;
            gMethods[0].signature = method[0]->signature;

            _nativeForkSystemServer = method[1]->fnPtr;
            gMethods[1].fnPtr = (void *) nativeForkSystemServer;
            gMethods[1].name = method[1]->name;
            gMethods[1].signature = method[1]->signature;

            _nativeSpecializeAppProcess = method[2]->fnPtr;
            gMethods[2].fnPtr = (void *) nativeSpecializeAppProcess;
            gMethods[2].name = method[2]->name;
            gMethods[2].signature = method[2]->signature;

            //replace zygote methods
            init_called = 1;

            jint res = env->RegisterNatives(clazz, gMethods, 3);
            if (res != JNI_OK) {
                LOGE("RegisterNatives failed");
                return 0;
            } else {
                LOGI("replaced com.android.internal.os.Zygote and simbols");
            }
        }
    }

    return res;
}

__attribute__((constructor)) __attribute__((used)) void OnLoad() {
    LOGI("app-lib running!!! pid=%d", getpid());

    XHOOK_REGISTER(".*\\libandroid_runtime.so$", _ZN7android39register_com_android_internal_os_ZygoteEP7_JNIEnv);

    if (xhook_refresh(0) == 0) {
        xhook_clear();
        LOGI("hook installed");
    } else {
        LOGE("failed to refresh hook");
    }
}