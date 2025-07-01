#include <android/dlext.h>
#include <cstring>
#include <dlfcn.h>
#include <stdio.h>
#include <jni.h>
#include "dobby/dobby.h"

extern void *(*orig_android_dlopen_ext)(const char *_Nullable __filename, int __flags, const android_dlextinfo *_Nullable __info, const void* caller_addr);
typedef int64_t (*NativeFunc)(char*);

void *hooked_android_dlopen_ext(const char *_Nullable __filename, int __flags, const android_dlextinfo *_Nullable __info, const void* caller_addr);