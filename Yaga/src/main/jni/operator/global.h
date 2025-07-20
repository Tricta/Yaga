#ifndef OPERATOR_GLOBAL_H
#define OPERATOR_GLOBAL_H

#include <jni.h>
#include "../logUtils.h"

extern JavaVM* g_vm;
extern const char* g_appDataDirChars;
extern JNIEnv* GetEnv();

#endif