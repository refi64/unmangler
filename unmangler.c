#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifndef __PCONFIGURE_DEPS
// make libc++ happy
typedef int namespace;
const namespace __cxxabiv1 = 0;

#include <cxxabi.h>

#ifdef _LIBCPPABI_VERSION
// copied from libc++'s cxxabi.h
extern char* __cxa_demangle(const char* mangled_name, char* output_buffer,
                            size_t* length, int* status);
#endif
#endif

#define ERR(...) fprintf(stderr, __VA_ARGS__);

#define M(n) !strncmp(name, #n, i=sizeof(#n )-1) ||
#define KW M(int) M(char) M(std) M(unsigned) M(long) M(short) 0

int ind=0;

void ln() {
    int i=0;
    putchar('\n');
    for (i=0; i<ind; ++i) fputs("  ", stdout);
}

int main(int argc, char** argv) {
    int status, i;
    char* name, *start, *tmp;
    if (argc != 2) {
        ERR("usage: %s <name>\n", argv[0]);
        return 1;
    }
    start = name = __cxa_demangle(argv[1], NULL, 0, &status);
    if (!status) {
        while (*name) {
            if (!strncmp(name, "(char)", 6)) {
                name += 6;
                assert(*name);
                printf("\033[36m'%c'\033[0m", (char)strtol(name, &tmp, 10));
                name = tmp;
            } else if (isdigit(*name) || *name == '-' && isdigit(*(name+1))) {
                printf("\033[34m%ld\033[0m", strtol(name, &tmp, 10));
                name = tmp;
                while (*name == 'u' || *name == 'l') {
                    printf("\033[34m%c\033[0m", *name);
                    ++name;
                }
            } else if (KW) {
                printf("\033[35m%.*s\033[0m", i, name);
                name += i;
            } else if (*name == '*') {
                printf("\033[31m%c\033[0m", *name);
                ++name;
            } else if (*name == ':' && *(name+1) == ':') {
                printf("\033[33m%.2s\033[0m", name);
                name += 2;
            } else if (*name == ',') {
                putchar(',');
                if (*++name == ' ') {
                    ++name;
                    putchar(' ');
                }
            } else if (*name == '<') {
                putchar('<');
                ++name;
                ++ind;
                ln();
            } else if (*name == '>') {
                putchar('>');
                if (*(name+1) == ' ') {
                    putchar(' ');
                    ++name;
                }
                if (*++name == '>') while (*name == '>') {
                    putchar('>');
                    if (*++name == ' ') {
                        putchar(' ');
                        ++name;
                    }
                    --ind;
                } else --ind;
                if (*name == ',') {
                    putchar(',');
                    if (*++name == ' ') ++name;
                }
                ln();
            } else putchar(*name++);
        }
        putchar('\n');
    }
    else {
        switch(status) {
        case -1: ERR("memory allocation error\n"); break;
        case -2: ERR("invalid name given\n"); break;
        case -3: ERR("internal error: __cxa_demangle: invalid argument\n"); break;
        default: ERR("unknown error occured\n"); break;
        }
        return 1;
    }
    free(start);
    return 0;
}
