#ifndef MMAN_H
#define MMAN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <switch.h>
#include <stdlib.h>

//#include "3ds_utils.h"

#define PROT_READ 0b001
#define PROT_WRITE 0b010
#define PROT_EXEC 0b100
#define MAP_PRIVATE 2
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20

#define MAP_FAILED ((void *)-1)

    static void *dynarec_cache = NULL;
    static void *dynarec_cache_mapping = NULL;

    static inline void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
    {
        (void)fd;
        (void)offset;

        //void* addr_out;
        Result rc = svcMapPhysicalMemory(addr, len);
        if (R_FAILED(rc))
        {
            printf("mmap failed\n");
            return malloc(len);
        }

        return addr;

        //   if((prot == (PROT_READ | PROT_WRITE | PROT_EXEC)) &&
        //      (flags == (MAP_PRIVATE | MAP_ANONYMOUS)))
        //   {
        //      if(true)// __ctr_svchax)
        //     {
        //         /* this hack works only for pcsx_rearmed */
        //         uint32_t currentHandle;
        //
        //         if(!dynarec_cache)
        //            dynarec_cache = memalign(0x1000, len);
        //
        //         //svcDuplicateHandle(&currentHandle, 0xFFFF8001);
        //         //svcControlProcessMemory(currentHandle, addr, dynarec_cache,
        //         //                        len, MEMOP_MAP, prot);
        //         svcCloseHandle(currentHandle);
        //         dynarec_cache_mapping = addr;
        //         return addr;
        //      }
        //      else
        //      {
        //         printf("tried to mmap RWX pages without svcControlProcessMemory access !\n");
        //         return MAP_FAILED;
        //      }
        //
        //   }

        //   addr_out = memalign(0x1000, len);
        //   if(!addr_out)
        //      return MAP_FAILED;
        //
        //   return addr_out;
    }

    static inline int mprotect(void *addr, size_t len, int prot)
    {
        return 0;
        //if(true) // __ctr_svchax)
        //{
        //   uint32_t currentHandle;
        //   //svcDuplicateHandle(&currentHandle, 0xFFFF8001);
        //   //svcControlProcessMemory(currentHandle, addr, NULL,
        //   //                        len, MEMOP_PROT, prot);
        //   svcCloseHandle(currentHandle);
        //   return 0;
        //}

        //printf("mprotect called without svcControlProcessMemory access !\n");
        //return -1;
    }

    static inline int munmap(void *addr, size_t len)
    {
        Result rc = svcUnmapPhysicalMemory(addr, len);
        if (R_FAILED(rc))
        {
            printf("munmap failed\n");
            free(addr);
        }
        return 0;
        //   if((addr == dynarec_cache_mapping) && true)//__ctr_svchax)
        //   {
        //      uint32_t currentHandle;
        //      //svcDuplicateHandle(&currentHandle, 0xFFFF8001);
        //      //svcControlProcessMemory(currentHandle,
        //      //                        dynarec_cache, dynarec_cache_mapping,
        //      //                        len, MEMOP_UNMAP, 0b111);
        //      svcCloseHandle(currentHandle);
        //      dynarec_cache_mapping = NULL;
        //
        //   }
        //   else
        free(addr);

        return 0;
    }

#ifdef __cplusplus
};
#endif

#endif // MMAN_H
