//
// Created by Administrator on 2021/9/7.
//
#include "test.h"
void* (*oldmalloc)(size_t __byte_count);
void *my_malloc(size_t size)
{
    LOGE("%zu bytes memory are allocated by libnative-lib.so\n", size);
    return oldmalloc(size);
}

void say_hello()
{
    char *buf = malloc(512);
    if(NULL != buf)
    {
        snprintf(buf, 1024, "%s", "hello\n");
        LOGD("say_hello %s", buf);
    }
}