//
// Created by Administrator on 2021/9/7.
//

#include "got.h"

namespace GOTCheck{


    GOT::GOT(u32 head,u32 end): head(head),end(end) {

    }

    GOT::~GOT() {

    }

    bool GOT::checkHook() {

        return false;
    }

    uaddr GOT::getgotitem(uaddr lib_addr,int idx) const {
        uaddr got_addr = lib_addr + head;
        uaddr cur_addr = 0;
#ifdef __arm__
        cur_addr = got_addr + 4 * idx;
#elif __aarch64__
        cur_addr = got_addr + 8 * idx;
#endif
        return cur_addr;
    }

    uaddr GOT::getsymaddr(uaddr libAddr, const char* symname) {
        // 查找符号地址
        uaddr item_addr = 0;
        u32 item_hash = 0;
        ElfW(Ehdr) *ehdr;
        ElfW(Phdr) *phdr;
        // check
        if(0 == libAddr){
            LOGD("getsymaddr(),fail.\n");
            return 0;
        }

        ehdr = (ElfW(Ehdr) *)libAddr;  //ELF文件头
        phdr = (ElfW(Phdr)*)(libAddr + ehdr->e_phoff); //ELF程序头表

        ElfW(Dyn) *p_dynamic = NULL;
        // 查找PT_DYNAMIC
        for (u32 k = 0; k < ehdr->e_phnum; ++k) {
            if(PT_DYNAMIC==phdr->p_type) {
                p_dynamic = (ElfW(Dyn)*)(phdr->p_vaddr + libAddr);
                break;
            }
            ++phdr;
        }//for
        if( p_dynamic == NULL ){
            LOGD("getItemAddr() NULL==p_dynamic  ,fail.\n");
            return false;
        }

        ElfW(Dyn)* d = NULL;
        ElfW(Sym)* p_symtab = NULL;

        uaddr gnu_hash = 0;
        size_t gnu_nbucket_ = NULL;
        u32* gnu_bucket_ = NULL;
        u32 gnu_maskwords_ = NULL;
        u32* gnu_chain_ = NULL;
        ElfW(Addr)* gnu_bloom_filter_ = NULL;

        char* strtab = NULL;
        u64 strz = 0;


        u32 hash = 0;
        u32* p_bucket = NULL;
        u32* p_chain = NULL;
        u32 nbucket = 0;

        //  linker.cpp -> prelink_image()
        for(d = p_dynamic; DT_NULL != d->d_tag; ++d) {
            if(d->d_tag == DT_SYMTAB) {
                p_symtab = (ElfW(Sym)*)(d->d_un.d_ptr + libAddr);
            }

            if(d->d_tag == DT_HASH) {
                hash = (u32)(d->d_un.d_ptr + libAddr);
                nbucket = *((u32*)hash);
                p_bucket = (u32*)(hash+8);
                p_chain = (u32*)(hash + 4 * (2 + nbucket));
            }

            if (d->d_tag == DT_GNU_HASH) {
                gnu_hash = (d->d_un.d_ptr + libAddr);
                gnu_nbucket_ = reinterpret_cast<u32 *>(gnu_hash)[0];
                // skip symndx
                gnu_maskwords_ = reinterpret_cast<u32 *>(gnu_hash)[2];
                gnu_bloom_filter_ = reinterpret_cast<ElfW(Addr) *>(gnu_hash + 16);
                gnu_bucket_ = reinterpret_cast<u32 *>(gnu_bloom_filter_ + gnu_maskwords_);

                // amend chain for symndx = header[1]
                gnu_chain_ = gnu_bucket_ + gnu_nbucket_ - reinterpret_cast<u32 *>(gnu_hash)[1];
            }
            if(d->d_tag == DT_STRTAB) {
                strtab = (char*)(d->d_un.d_ptr + libAddr);
            }
            if(d->d_tag == DT_STRSZ) {
                strz = (u64)(d->d_un.d_val);
            }
        }//for
        if( NULL==p_symtab || NULL==strtab || 0==strz ){
            LOGD("getItemAddr,fail.\n");
            return false;
        }

        //gnu hash 段 arm64下so没有hash段，使用gnu hash
        if (gnu_hash != 0){
            item_hash = dl_gnu_hash(symname);
            u32 n = gnu_bucket_[item_hash % gnu_nbucket_];
            do {
                ElfW(Sym)* s = p_symtab + n;

                if(dl_gnu_hash(strtab + s->st_name) == item_hash){
                    item_addr = libAddr + s->st_value;
                    break;
                }
            } while ((gnu_chain_[n++] & 1) == 0);
        } else { //hash段，armv7下有so没有gnu hash段，使用hash
            item_hash = elfhash(symname);
            int mod = (item_hash % nbucket);
            for(int i = p_bucket[mod]; i != 0; i = p_chain[i]) {
                if(elfhash(strtab + (p_symtab + i)->st_name) == item_hash) {
                    item_addr = libAddr + (p_symtab + i)->st_value;
                    break;
                }
            }//for
        }

        // 清理内存
        ehdr = NULL;
        phdr = NULL;
        p_dynamic = NULL;
        d = NULL;
        p_symtab = NULL;
        strtab = NULL;
        strz = 0;
        return item_addr;
    }
}