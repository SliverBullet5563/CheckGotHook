#include <jni.h>
#include <string>
#include <elf.h>
#include "test.h"
#include "got.h"
#include "xhook.h"
#include<sstream>

using namespace GOTCheck;

void* (*oldmalloc)(size_t __byte_count);
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_checknativehook_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */,jboolean startGotHook) {
    std::string hello = "Hello from C++";

    //开启GOT Hook
    if (startGotHook){
        xhook_register(".*/libnative-lib\\.so$", "malloc", (void *)(my_malloc), (void**)(&oldmalloc));
        xhook_refresh(1);
    }

    uaddr lib_Addr = getLibAddr("libnative-lib.so");
    LOGD("libnative_addr = 0x%lx", lib_Addr);
    //计算libc中malloc内存的地址
    uaddr libc_Addr = getLibAddr("libc.so");
    LOGD("libc_addr = 0x%lx", libc_Addr);


    //预设got起始地址，导入表函数位置
    u32 head = 0;
    u32 end = 0;
    int idx = 0;
#ifdef __arm__
    head = 0x76F94;
    idx = 194;
#elif __aarch64__
    head = 0xC8010; //got起始地址
    idx = 82;   //malloc项
#endif

    GOT* got = new GOT(head, end);
    // 返回运行时got地址里的地址内容
    uaddr item_addr = got->getgotitem(lib_Addr, idx);
    LOGD("item_addr = 0x%lx",item_addr);
    LOGD("item_addr pointer content = 0x%lx",*(uaddr *)item_addr);
    //返回运行时依赖库函数地址
    uaddr malloc_addr = got->getsymaddr(libc_Addr,"malloc");

    LOGD("libc malloc_addr = 0x%lx",malloc_addr);

    //比较got表里的地址，和实际导入函数的地址是否相同
    if (*(uaddr *)item_addr != malloc_addr){
        LOGE("检测到libc.so malloc 被GOT hook！被hook的地址 0x%lx",*(uaddr *)item_addr);
        hello.append(" 检测到GOT hook, hook地址为 ");
        stringstream ss1;
        ss1 << *(uaddr *)item_addr;
        hello.append(ss1.str());
    } else{
        hello.append(" 对malloc 未检测到GOT hook");
    }


    say_hello();
    delete got;
    return env->NewStringUTF(hello.c_str());
}