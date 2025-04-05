#include <unistd.h>
#include <stddef.h> 
#include <string.h>

ssize_t fdgets(char *buf, const size_t size, int fd);
void *memsearch(const uintptr_t addr_start, const uintptr_t addr_end, const void *s, size_t size);