#include "libartHook.h"
#include "../logUtils.h"

#ifdef __LP64__
    using Elf_Ehdr = Elf64_Ehdr;
    using Elf_Shdr = Elf64_Shdr;
    using Elf_Sym  = Elf64_Sym;
    #define SS_SCANF "%lx-%lx %*s %*s %*s %*s %s"
#else
    using Elf_Ehdr = Elf32_Ehdr;
    using Elf_Shdr = Elf32_Shdr;
    using Elf_Sym  = Elf32_Sym;
    #define SS_SCANF "%lx-%lx %*s %*s %*s %*s %s"
#endif

inline uint8_t get_sym_type(uint8_t st_info) {
    return st_info & 0x0F;
}

#define MAX_NAME_LEN 256
#define MEMORY_ONLY  "[memory]"

struct memoryMap {
    char name[MAX_NAME_LEN];
    unsigned long start, end;
};

struct symlist {
    Elf_Sym *sym;
    char *str;
    unsigned num;
};

struct symtab {
    struct symlist *st;
    struct symlist *dyn;
};

typedef struct symtab* symtab_t;

static int my_pread(int fd, void *buf, size_t count, off_t offset) {
    lseek(fd, offset, SEEK_SET);
    return read(fd, buf, count);
}

static struct symlist* get_syms(int fd, Elf_Shdr *symh, Elf_Shdr *strh) {
    if (symh->sh_size % sizeof(Elf_Sym)) return NULL;

    struct symlist *symlst = (struct symlist *) malloc(sizeof(struct symlist));
    if (!symlst) {
        LOGE("Out of memory");
        return NULL;
    }

    symlst->num = symh->sh_size / sizeof(Elf_Sym);
    symlst->sym = (Elf_Sym *) malloc(symh->sh_size);
    if (!symlst->sym) {
        LOGE("Out of memory");
        free(symlst);
        return NULL;
    }

    if (my_pread(fd, symlst->sym, symh->sh_size, symh->sh_offset) != (int)symh->sh_size) {
        free(symlst->sym);
        free(symlst);
        return NULL;
    }

    symlst->str = (char *) malloc(strh->sh_size);
    if (!symlst->str) {
        LOGE("Out of memory");
        free(symlst->sym);
        free(symlst);
        return NULL;
    }

    if (my_pread(fd, symlst->str, strh->sh_size, strh->sh_offset) != (int)strh->sh_size) {
        free(symlst->str);
        free(symlst->sym);
        free(symlst);
        return NULL;
    }

    return symlst;
}

static int lookup_sym_list(struct symlist *symlst, unsigned char type, char *name, unsigned long *val) {
    int len = strlen(name);
    for (unsigned i = 0; i < symlst->num; i++) {
        Elf_Sym *eSymPointer = &symlst->sym[i];
        if (!strncmp(symlst->str + eSymPointer->st_name, name, len) &&
            symlst->str[eSymPointer->st_name + len] == 0 &&
            get_sym_type(eSymPointer->st_info) == type) {
            *val = eSymPointer->st_value;
            return 0;
        }
    }
    return -1;
}

static int lookup_sym(symtab_t symtb, unsigned char type, char *name, unsigned long *val) {
    if (symtb->dyn && !lookup_sym_list(symtb->dyn, type, name, val)) return 0;
    if (symtb->st && !lookup_sym_list(symtb->st, type, name, val)) return 0;
    return -1;
}

static int do_load(int fd, symtab_t symtab) {
    Elf_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) return -1;
    if (strncmp(ELFMAG, (const char*)ehdr.e_ident, SELFMAG)) return -1;

    size_t size = ehdr.e_shentsize * ehdr.e_shnum;
    Elf_Shdr *shdr = (Elf_Shdr *) malloc(size);
    if (!shdr) {
        LOGE("Out of memory");
        return -1;
    }

    if (my_pread(fd, shdr, size, ehdr.e_shoff) != (int)size) {
        free(shdr);
        return -1;
    }

    char *shstrtab = (char *) malloc(shdr[ehdr.e_shstrndx].sh_size);
    if (!shstrtab) {
        LOGE("Out of memory");
        free(shdr);
        return -1;
    }

    if (my_pread(fd, shstrtab, shdr[ehdr.e_shstrndx].sh_size, shdr[ehdr.e_shstrndx].sh_offset) != (int)shdr[ehdr.e_shstrndx].sh_size) {
        free(shstrtab);
        free(shdr);
        return -1;
    }

    Elf_Shdr *symh = NULL, *strh = NULL, *dynsymh = NULL, *dynstrh = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB) symh = &shdr[i];
        else if (shdr[i].sh_type == SHT_DYNSYM) dynsymh = &shdr[i];
        else if (shdr[i].sh_type == SHT_STRTAB) {
            const char* secname = shstrtab + shdr[i].sh_name;
            if (!strncmp(secname, ".strtab", 7)) strh = &shdr[i];
            else if (!strncmp(secname, ".dynstr", 8)) dynstrh = &shdr[i];
        }
    }

    if (dynsymh && dynstrh) symtab->dyn = get_syms(fd, dynsymh, dynstrh);
    if (symh && strh) symtab->st = get_syms(fd, symh, strh);

    free(shstrtab);
    free(shdr);
    return 0;
}

static void print_symlist(struct symlist* symlst, const char* label) {
    if (!symlst) {
        LOGI("[SYM] No %s symbols found.", label);
        return;
    }

    LOGI("[SYM] --- %s SYMBOLS ---", label);
    for (unsigned i = 0; i < symlst->num; i++) {
        Elf_Sym* sym = &symlst->sym[i];
        const char* name = symlst->str + sym->st_name;
        if (name && name[0] != '\0') {
            LOGI("[SYM] %-40s | addr: 0x%lx | type: %u", name, (unsigned long)sym->st_value, get_sym_type(sym->st_info));
        }
    }
}

static void print_symtab(symtab_t symtb) {
    LOGI("[SYM] ===== PRINTING SYMBOL TABLE =====");
    print_symlist(symtb->st,  "STATIC");
    print_symlist(symtb->dyn, "DYNAMIC");
    LOGI("[SYM] ===== END SYMBOL TABLE =====");
}

static symtab_t load_symtab(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return NULL;

    symtab_t s = (symtab_t) malloc(sizeof(*s));
    if (!s) {
        LOGE("Out of memory");
        close(fd);
        return NULL;
    }

    memset(s, 0, sizeof(*s));

    if (do_load(fd, s) < 0) {
        LOGE("Failed to load symbol table from: %s", filename);
        free(s);
        close(fd);
        return NULL;
    }

    LOGI("Successfully loaded symbol table from: %s", filename);
    close(fd);
    return s;
}

static int find_libname(char *libn, char *name, int len, unsigned long *start, struct memoryMap *memMap, int memMapCount) {
    for (int i = 0; i < memMapCount; i++) {
        struct memoryMap *memory = &memMap[i];
        if (!strcmp(memory->name, MEMORY_ONLY)) continue;

        const char *p = strrchr(memory->name, '/');
        if (!p) continue;
        p++;

        size_t mlen = strlen(p);
        size_t libn_len = strlen(libn);

        if (mlen < libn_len) continue;
        if (strncmp(p + mlen - libn_len, libn, libn_len) != 0) continue;

        *start = memory->start;
        strncpy(name, memory->name, len);
        name[len - 1] = '\0';

        mprotect((void*)memory->start, memory->end - memory->start, PROT_READ | PROT_WRITE | PROT_EXEC);

        LOGI("Found library %s mapped at 0x%lx", name, *start);
        return 0;
    }
    return -1;
}

static int load_memmap(struct memoryMap *memMap, int *memMapCount) {
    char path[128];
    snprintf(path, sizeof(path), "/proc/self/maps");
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;

    char line[512], name[MAX_NAME_LEN];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        unsigned long start, end;
        if (sscanf(line, SS_SCANF, &start, &end, name) < 3) continue;

        struct memoryMap *memory = NULL;
        for (int i = 0; i < count; i++) {
            if (!strcmp(memMap[i].name, name)) {
                memory = &memMap[i];
                break;
            }
        }
        if (memory) {
            if (start < memory->start) memory->start = start;
            if (end > memory->end) memory->end = end;
        } else {
            memory = &memMap[count++];
            memory->start = start;
            memory->end = end;
            strncpy(memory->name, name, MAX_NAME_LEN);
        }
    }
    fclose(fp);
    *memMapCount = count;
    return 0;
}

int find_name(char *name, char *libn, unsigned long *addr) {
    struct memoryMap memMap[1000];
    int memMapCount;
    char path[PATH_MAX];
    unsigned long base;

    if (load_memmap(memMap, &memMapCount) < 0) {
        LOGE("cannot read memory map");
        return -1;
    }

    LOGI("Loaded memory map for current process");

    if (find_libname(libn, path, sizeof(path), &base, memMap, memMapCount) < 0) {
        LOGE("cannot find lib: %s", libn);
        return -1;
    }

    LOGI("Library %s found at base address: 0x%lx", path, base);

    symtab_t s = load_symtab(path);
    if (!s) {
        LOGE("cannot read symbol table");
        return -1;
    }

    //print_symtab(s);

    if (lookup_sym(s, STT_FUNC, name, addr) < 0) {
        LOGE("cannot find function: %s", name);
        return -1;
    }

    *addr += base;
    LOGI("Symbol %s resolved to address: 0x%lx", name, *addr);
    return 0;
}