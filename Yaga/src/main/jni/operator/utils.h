#include <unistd.h>
#include <stddef.h> 
#include <string>
#include <jni.h>
#include <elf.h>
#include <android/log.h>
#include <vector>


ssize_t fdgets(char *buf, const size_t size, int fd);
void *memsearch(const uintptr_t addr_start, const uintptr_t addr_end, const void *s, size_t size);
JNINativeMethod *search_method(int endian, std::vector<std::pair<uintptr_t, uintptr_t>> addresses, const char *name, size_t len);