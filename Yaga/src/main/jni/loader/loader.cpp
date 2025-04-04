#include <jni.h>
#include <string>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <android/log.h>
#include <dlfcn.h>

#include "nativeBridgeStructs.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Zygote Injection", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Zygote Injection", __VA_ARGS__)

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

    char buf[PATH_MAX]{0};
    int fd = open("/data/adb/yagaModule/native_bridge", O_RDONLY);
    if (fd == -1) {
        LOGE("access /data/adb/yagaModule/native_bridge failed!");
        return;
    }

    auto size = read(fd, buf, PATH_MAX);
    close(fd);

    if (size <= 0) {
        LOGE("can't read native_bridge");
        return;
    }

    buf[size] = 0;
    if (size > 1 && buf[size - 1] == '\n') buf[size - 1] = 0;
    LOGI("original native bridge: %s", buf);

    if (buf[0] == '0' && buf[1] == 0) {
        return;
    }

    auto native_bridge = buf + size + 1;
    strcpy(native_bridge, LIB_PATH);
    strncat(native_bridge, buf, size);

    if (access(native_bridge, F_OK) != 0) {
        LOGE("access %s", native_bridge);
        return;
    }

    original_bridge = dlopen(native_bridge, RTLD_NOW);
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
}