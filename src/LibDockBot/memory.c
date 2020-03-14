/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#include <string.h>

#include "lib.h"

#ifdef DEBUG_BUILD

#define MEM_PREFIX 0xF0E1D2C3
#define MEM_SUFFIX 0xA9B8C7D6
#define MEM_FREED  0xDEADBEEF

VOID *AllocMemInternalReal(struct DockBotLibrary *lib, ULONG byteSize, ULONG attributes);
VOID FreeMemInternalReal(struct DockBotLibrary *lib, VOID *memoryBlock, ULONG byteSize);

#endif

extern struct DockBotLibrary *DockBotBase;

BOOL InitMem(struct DockBotLibrary *lib) 
{
    struct MemoryControl *mc = &lib->l_MemControl;

    if( mc->chipPool = CreatePool(MEMF_CHIP, 4096, 2048) ) {

        if( AvailMem(MEMF_FAST) > 0 ) {
            if( ! (mc->fastPool = CreatePool(MEMF_FAST, 4096, 2048))) {
                return FALSE;
            }
        }
        return TRUE;
    }   
    return FALSE;
}

VOID CleanUpMem(struct DockBotLibrary *lib)
{
    struct MemoryControl *mc = &lib->l_MemControl;

    if( mc->fastPool ) {
        DeletePool(mc->fastPool);
    }

    if( mc->chipPool ) {
        DeletePool(mc->chipPool);
    }

#ifdef DEBUG_BUILD

    if( mc->chipAllocated > 0 ) {
        DebugLog("MEMORY ERROR: Chip Memory Not Freed: %ld, Allocs: %ld, Frees: %ld\n", mc->chipAllocated, mc->chipAllocCount, mc->chipFreeCount);
    }

    if( mc->fastAllocated > 0 ) {
        DebugLog("MEMORY ERROR: Fast Memory Not Freed: %ld, Allocs: %ld, Frees: %ld\n", mc->fastAllocated, mc->fastAllocCount, mc->fastFreeCount);
    }

#endif
}


VOID* AllocMemInternal(struct DockBotLibrary *lib, ULONG byteSize, ULONG attributes)
{

#ifdef DEBUG_BUILD

    ULONG actualSize = byteSize + 12;
    VOID *m;
    ULONG *p, *s;

    if( actualSize & 1 ) {
        actualSize++;
    }

    if( m = AllocMemInternalReal(lib, actualSize, attributes) ) {
    
        p = (ULONG *)m;
        *p = MEM_PREFIX;
        p++;
        *p = byteSize;
        p++;

        s = (ULONG *)(((UBYTE *)m) + actualSize - 4);
        *s = MEM_SUFFIX;

        return (VOID *)p;    
    }
    
    return NULL;
}

VOID *AllocMemInternalReal(struct DockBotLibrary *lib, ULONG byteSize, ULONG attributes)
{
#endif

    VOID *result;
    struct MemoryControl *mc = &lib->l_MemControl;
  
    if( (attributes & MEMF_CHIP) || ! mc->fastPool ) {
       
        if( result = AllocPooled(mc->chipPool, byteSize) ) {
            mc->chipAllocated += byteSize;
            mc->chipAllocCount++;

            if( mc->chipAllocated > mc->chipAllocMax ) {
                mc->chipAllocMax = mc->chipAllocated;
            }
        }   
        
    } else {

        if( result = AllocPooled(mc->fastPool, byteSize) ) {
            mc->fastAllocated += byteSize;
            mc->fastAllocCount++;
            
            if( mc->fastAllocated > mc->fastAllocMax ) {
                mc->fastAllocMax = mc->fastAllocated;
            }
        }
    }

    if( result && (attributes & MEMF_CLEAR ) ) {
        memset(result, 0, byteSize);
    }

    return result;    
}

VOID FreeMemInternal(struct DockBotLibrary *lib, VOID *memoryBlock, ULONG byteSize)
{
#ifdef DEBUG_BUILD

    UBYTE *m;
    ULONG prefix, size, suffix, actualSize = byteSize + 12;
    ULONG *p;
    BOOL showErr = FALSE;

    if( actualSize & 1 ) {
        actualSize++;
    }

    m = ((UBYTE *)memoryBlock) - 8;

    p = (ULONG *)m;
    prefix = *p;
    *p = MEM_FREED;
    if( prefix != MEM_PREFIX ) {
        showErr = TRUE;
    }
    *p++;
    size = *p;
    if( size != byteSize ) {
        showErr = TRUE;        
    }

    p = (ULONG *)(m + actualSize - 4);
    suffix = *p;
    *p = MEM_FREED;
    if( suffix != MEM_SUFFIX ) {
        showErr = TRUE;
    }

    if( showErr ) {

        DebugLog("MEMORY ERROR! block = %08lx, PREFIX %08lx %08lx, SIZE %ld %ld SUFFIX %08lx, %08lx\n", 
            memoryBlock, MEM_PREFIX, prefix, byteSize, size, MEM_SUFFIX, suffix);
    } else {
        FreeMemInternalReal(lib, m, actualSize);
    }
}

VOID FreeMemInternalReal(struct DockBotLibrary *lib, VOID *memoryBlock, ULONG byteSize)
{

#endif

    struct MemoryControl *mc = &lib->l_MemControl;

    if( TypeOfMem(memoryBlock) & MEMF_CHIP ) {

        FreePooled(mc->chipPool, memoryBlock, byteSize);
        mc->chipFreeCount++;
        mc->chipAllocated -= byteSize;
        
    } else {

        FreePooled(mc->fastPool, memoryBlock, byteSize);
        mc->fastFreeCount++;
        mc->fastAllocated -= byteSize;
    }
}

VOID* __asm __saveds DB_GetMemInfo(VOID)
{
    return (VOID*)&DockBotBase->l_MemControl;
}

VOID* __asm __saveds DB_AllocMem(
    register __d0 ULONG byteSize,
    register __d1 ULONG attributes)
{
    return AllocMemInternal(DockBotBase, byteSize, attributes);
}

VOID __asm __saveds DB_FreeMem(
    register __a0 VOID *memoryBlock,
    register __d0 ULONG byteSize)
{
    FreeMemInternal(DockBotBase, memoryBlock, byteSize);
}

