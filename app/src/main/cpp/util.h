//
// Created by Administrator on 2021/9/10.
//

#ifndef CHECKNATIVEHOOK_UTIL_H
#define CHECKNATIVEHOOK_UTIL_H

#include <elf.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include "log.h"

typedef uint32_t       u32;
typedef uint64_t       u64;

#ifdef __arm__
typedef u32 uaddr;
#define ElfW(type) Elf32_ ## type
#elif __aarch64__
typedef u64 uaddr;
#define ElfW(type) Elf64_ ## type
#endif


uaddr getLibAddr(const char* name);
u32 elfhash(const char* input);
u32 dl_gnu_hash (const char *s);

#endif //CHECKNATIVEHOOK_UTIL_H
