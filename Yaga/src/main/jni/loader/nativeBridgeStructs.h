#ifndef NATIVE_LIB_NATIVEBRIDGESTRUCTS_H
#define NATIVE_LIB_NATIVEBRIDGESTRUCTS_H

struct NativeBridgeCallbacks {
    [[maybe_unused]] uint32_t version;
    [[maybe_unused]] void *initialize;
    [[maybe_unused]] void *loadLibrary;
    [[maybe_unused]] void *getTrampoline;
    [[maybe_unused]] void *isSupported;
    [[maybe_unused]] void *getAppEnv;
    [[maybe_unused]] void *isCompatibleWith;
    [[maybe_unused]] void *getSignalHandler;
    [[maybe_unused]] void *unloadLibrary;
    [[maybe_unused]] void *getError;
    [[maybe_unused]] void *isPathSupported;
    [[maybe_unused]] void *initAnonymousNamespace;
    [[maybe_unused]] void *createNamespace;
    [[maybe_unused]] void *linkNamespaces;
    [[maybe_unused]] void *loadLibraryExt;
    [[maybe_unused]] void *getVendorNamespace;
    [[maybe_unused]] void *getExportedNamespace;
    [[maybe_unused]] void *preZygoteFork;
};

#endif //NATIVE_LIB_NATIVEBRIDGESTRUCTS_H
