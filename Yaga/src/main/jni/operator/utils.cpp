#include "utils.h"

ssize_t fdgets(char *buf, const size_t size, int fd) {
    ssize_t len = 0;
    buf[0] = '\0';
    while (len < size - 1) {
        ssize_t ret = read(fd, buf + len, 1);
        if (ret < 0)
            return -1;
        if (ret == 0)
            break;
        if (buf[len] == '\0' || buf[len++] == '\n') {
            break;
        }
    }
    buf[len] = '\0';
    buf[size - 1] = '\0';
    return len;
}

void *memsearch(const uintptr_t addr_start, const uintptr_t addr_end, const void *s, size_t size) {
    uintptr_t _addr_start = addr_start;
    while (1) {
        if (_addr_start + size >= addr_end)
            return NULL;

        if (memcmp((const void *) _addr_start, s, size) == 0)
            return (void *) _addr_start;

        _addr_start += 1;
    }
}