/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "platform_api_vmcore.h"

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#define FIXED_BUFFER_SIZE (1 << 9)

static char *g_pos = NULL; /* 用于保存扫描位置的静态变量 */

int
bh_platform_init()
{
    return 0;
}

void
bh_platform_destroy()
{
}

void *
os_malloc(unsigned size)
{
    return TEE_Malloc(size, TEE_USER_MEM_HINT_NO_FILL_ZERO);
}

void *
os_realloc(void *ptr, unsigned size)
{
    return TEE_Realloc(ptr, size);
}

void
os_free(void *ptr)
{
    TEE_Free(ptr);
}

int
os_printf(const char *message, ...)
{
    /*
     * Defined static to reduce the pressure on the stack allocation.
     * The TA may panic otherwise. Note this implementation may cause
     * concurrent accesses issues when the application is multithreaded,
     * which is not the case in the TA.
     */
    static char msg[FIXED_BUFFER_SIZE] = { '\0' };
    va_list ap;
    va_start(ap, message);
    vsnprintf(msg, FIXED_BUFFER_SIZE, message, ap);
    va_end(ap);

    IMSG(msg);

    return 0;
}

int
os_vprintf(const char *format, va_list arg)
{
    /*
     * Defined static to reduce the pressure on the stack allocation.
     * The TA may panic otherwise. Note this implementation may cause
     * concurrent accesses issues when the application is multithreaded,
     * which is not the case in the TA.
     */
    static char msg[FIXED_BUFFER_SIZE] = { '\0' };
    vsnprintf(msg, FIXED_BUFFER_SIZE, format, arg);

    EMSG(msg);

    return 0;
}

char *
strcpy(char *dest, const char *src)
{
    const unsigned char *s = src;
    unsigned char *d = dest;

    while ((*d++ = *s++))
        ;
    return dest;
}

void *
os_mmap(void *hint, size_t size, int prot, int flags, os_file_handle file)
{
    uint64 aligned_size, page_size;
    void *ret = NULL;

    page_size = PAGE_SIZE;
    aligned_size = (size + page_size - 1) & ~(page_size - 1);

    if (aligned_size >= UINT32_MAX)
        return NULL;

    ret = tee_map_zi(aligned_size, 0);
    if (ret == NULL) {
        os_printf("os_mmap(size=%lu, aligned size=%lu, prot=0x%x) failed.",
                  size, aligned_size, prot);
        return NULL;
    }
    else {
        DMSG("os_mmap(addr=%p, size=%lu, aligned size=%lu, prot=0x%x) memory "
             "allocated.",
             ret, size, aligned_size, prot);
    }

    if (os_mprotect(ret, aligned_size, prot) != 0) {
        os_printf("os_mmap(size=%lu, prot=0x%x) failed to set protect.", size,
                  prot);
        tee_unmap(ret, aligned_size);
        return NULL;
    }
    else {
        DMSG("os_mmap(addr=%p, size=%lu, aligned size=%lu, prot=0x%x) "
             "protection set.",
             ret, size, aligned_size, prot);
    }

    return ret;
}

void
os_munmap(void *addr, size_t size)
{
    uint64 aligned_size, page_size;
    TEE_Result res;

    page_size = PAGE_SIZE;
    aligned_size = (size + page_size - 1) & ~(page_size - 1);

    res = tee_unmap(addr, aligned_size);
    if (res != TEE_SUCCESS) {
        os_printf("os_munmap(addr=%p, size=%lu, aligned size=%lu) error while "
                  "unmapping the memory: %u.",
                  addr, size, aligned_size, res);
    }
    else {
        DMSG("os_munmap(addr=%p, size=%lu, aligned size=%lu) OK.", addr, size,
             aligned_size);
    }
}

int
os_mprotect(void *addr, size_t size, int prot)
{
    int mprot = 0;
    TEE_Result res;

    if (prot & MMAP_PROT_READ)
        mprot |= TEE_MATTR_UR | TEE_MATTR_PR;
    if (prot & MMAP_PROT_WRITE)
        mprot |= TEE_MATTR_UW | TEE_MATTR_PW;
    if (prot & MMAP_PROT_EXEC)
        mprot |= TEE_MATTR_UX | TEE_MATTR_PX;

    res = tee_mprotect(addr, size, mprot);
    if (res != TEE_SUCCESS) {
        os_printf("os_mprotect(addr=%p, size=%lu, prot=%u) failed: %u.", addr,
                  size, prot, res);
    }
    else {
        DMSG("os_mprotect(addr=%p, size=%lu, prot=%u) OK.", addr, size, prot);
    }

    return (res == TEE_SUCCESS ? 0 : -1);
}

void
os_dcache_flush(void)
{
}

float
strtof(const char *str, char **endptr)
{
    EMSG("strtof is not supported.");
    return 0;
}

long int
strtol(const char *str, char **endptr, int base)
{
    return strtoul(str, endptr, base);
}

double
strtod(const char *str, char **endptr)
{
    EMSG("strtod is not supported.");
    return 0;
}

unsigned long long int
strtoull(const char *str, char **endptr, int base)
{
    EMSG("strtoull is not supported.");
    return 0;
}

char *
strtok(char *str, const char *delim)
{
    char *token_start;

    /* 如果输入为NULL且没有保存的位置，返回NULL */
    if (!str && !g_pos)
        return NULL;

    /* 如果输入为NULL，使用保存的位置，否则使用输入字符串 */
    token_start = str ? str : g_pos;

    /* 跳过开头的分隔符 */
    while (*token_start && strchr(delim, *token_start))
        token_start++;

    /* 如果到达字符串末尾，返回NULL */
    if (!*token_start) {
        g_pos = NULL;
        return NULL;
    }

    /* 找到token的结束位置 */
    str = token_start;
    while (*str && !strchr(delim, *str))
        str++;

    if (*str) {
        *str = '\0';     /* 在token结束位置放置字符串结束符 */
        g_pos = str + 1; /* 保存下一次扫描的起始位置 */
    }
    else {
        g_pos = NULL; /* 已经到达字符串末尾 */
    }

    return token_start;
}

void
os_icache_flush(void *start, size_t len)
{
}

int
os_dumps_proc_mem_info(char *out, unsigned int size)
{
    return -1;
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n-- > 0) {
        unsigned char c1 = *s1;
        unsigned char c2 = *s2;

        /* 将大写字母转换为小写字母 */
        if (c1 >= 'A' && c1 <= 'Z')
            c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z')
            c2 += 'a' - 'A';

        if (c1 != c2)
            return c1 - c2;
        if (c1 == '\0')
            break;
        s1++;
        s2++;
    }
    return 0;
}

