//
// Created by Administrator on 2021/9/7.
//
#ifndef CHECKNATIVEHOOK_GOT_H
#define CHECKNATIVEHOOK_GOT_H

#include <stdio.h>
#include "util.h"


using namespace std;
namespace GOTCheck{

    class GOT
    {
    public:
        GOT(u32 head,u32 end);
        ~GOT();
        bool checkHook();

    public:
        uaddr getgotitem(uaddr lib_addr, int idx) const;        // 返回运行时idx项got地址里的地址内容
        uaddr getsymaddr(uaddr libAddr, const char* symname);   // 返回运行时依赖库函数地址
    private:
        u32 head;                   // GOT表起始偏移（预设）
        u32 end;                    // GOT表结束偏移（预设）
    };
}
#endif //CHECKNATIVEHOOK_GOT_H
