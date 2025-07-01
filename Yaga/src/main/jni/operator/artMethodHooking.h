#include <jni.h>
#include <string>
#include "artMethod.h"

extern int (*orig_doCall[4])(artMethod*, void*, void*, void*, int, void*);
extern std::string(*orig_PrettyMethod)(void*, bool);

int hooked_doCall_0(artMethod* a, void* b, void* c, void* d, int e, void* f);
int hooked_doCall_1(artMethod* a, void* b, void* c, void* d, int e, void* f);
int hooked_doCall_2(artMethod* a, void* b, void* c, void* d, int e, void* f);
int hooked_doCall_3(artMethod* a, void* b, void* c, void* d, int e, void* f);

std::string PrettyMethod(void* arg1, bool arg2);