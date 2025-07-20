#include <elf.h>
#include <sys/mman.h>
#include <android/log.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

int find_name(char *name, char *libn, unsigned long *addr) ;