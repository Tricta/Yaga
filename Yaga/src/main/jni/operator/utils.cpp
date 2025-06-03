#include "utils.h"
#include "../logUtils.h"

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