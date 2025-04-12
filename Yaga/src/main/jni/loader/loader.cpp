#include <jni.h>
#include <string>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <android/log.h>
#include <dlfcn.h>

#include "nativeBridgeStructs.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Yaga", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Yaga", __VA_ARGS__)

#ifdef __LP64__
constexpr const char* zygoteName = "zygote64";
#else
constexpr const char* zygoteName = "zygote";
#endif

#ifdef __LP64__
#define LIB_PATH "/system/lib64/"
#else
#define LIB_PATH "/system/lib/"
#endif

static void *original_bridge = nullptr;

extern "C"  __attribute__((visibility("default"))) uint8_t NativeBridgeItf[sizeof(NativeBridgeCallbacks) * 2]{0};

bool is_zygote() {
    if (getuid() != 0) return false; // Zygote always running in root user

    // Read current process name and check it is Zygote
    int fd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        LOGI("Failed to open /proc/self/cmdline: %s(%d)", strerror(errno), errno);
        return false;
    }

    char buf[NAME_MAX + 1];
    ssize_t size = read(fd, buf, NAME_MAX);
    if (size <= 0) {
        LOGI("Failed to read /proc/self/cmdline: %s(%d)", strerror(errno), errno);
        close(fd);
        return false;
    }

    close(fd);

    return strcmp(buf, zygoteName) == 0;
}

__attribute__((destructor)) void destructor() {
    if (original_bridge) dlclose(original_bridge);
}

__attribute__((constructor)) __attribute__((used)) void OnLoad() {
    LOGI("Code loaded in %s process, pid=%d uid=%d", is_zygote() ? "zygote" : "non-zygote", getpid(), getuid());
    dlopen(LIB_PATH "libyagaoperator.so", 0);

    #if defined(__i386__) || defined(__x86_64__)
    char native_bridge[PATH_MAX] = "libndk_translation.so";
    size_t size = strlen(native_bridge);

    LOGI("original native bridge: %s", native_bridge);
    if (native_bridge[0] == '0' && native_bridge[1] == '\0') {
        return;
    }

    char native_bridge_full_path[PATH_MAX];
    snprintf(native_bridge_full_path, sizeof(native_bridge_full_path), "%s%s", LIB_PATH, native_bridge);

    if (access(native_bridge_full_path, F_OK) != 0) {
        LOGE("access %s", native_bridge_full_path);
        return;
    }
    
    original_bridge = dlopen(native_bridge_full_path, RTLD_NOW);
    if (original_bridge == nullptr) {
        LOGE("dlopen failed: %s", dlerror());
        return;
    }
    
    auto original_NativeBridgeItf = dlsym(original_bridge, "NativeBridgeItf");
    if (original_NativeBridgeItf == nullptr) {
        LOGE("dlsym failed: %s", dlerror());
        return;
    }
    
    auto callbacks_size = sizeof(NativeBridgeCallbacks);
    memcpy(NativeBridgeItf, original_NativeBridgeItf, callbacks_size);
    #endif
}