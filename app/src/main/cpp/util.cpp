//
// Created by Administrator on 2021/9/10.
//

#include "util.h"

uaddr getLibAddr(const char* name){
    u64 ret = 0;
    char buf[4096], *temp;
    int pid;
    FILE *fp;
    pid = getpid();
    sprintf(buf, "/proc/%d/maps", pid);
    fp = fopen(buf, "r");
    if(fp == NULL)
    {
        puts("open failed");
        goto _error;
    }
    while(fgets(buf, sizeof(buf), fp)){
        if(strstr(buf, name)){
            temp = strtok(buf, "-");
            ret = strtoul(temp, NULL, 16);
            break;
        }
    }
    _error:
    fclose(fp);
    return ret;
}

u32 elfhash(const char* input){
    const char* s = input;
    u32 h = 0, g = 0;
    while(*s) {
        h = (h << 4) + *s++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

u32 dl_gnu_hash (const char *s){
    u32 h = 5381;
    for (unsigned char c = *s; c != '\0'; c = *++s)
        h = ((h << 5) + h) + c; // h = h * 33 + c


    return h;
}