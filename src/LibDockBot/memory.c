/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#include <string.h>

#include "lib.h"

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
}


VOID* AllocMemInternal(struct DockBotLibrary *lib, ULONG byteSize, ULONG attributes)
{
    VOID *result;
    ULONG i;
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
        // TODO: Make this faster
        for( i = 0;i < byteSize; i++ ) {
            *(((UBYTE*)result) + i) = 0;
        }
    }

    return result;    
}

VOID FreeMemInternal(struct DockBotLibrary *lib, VOID *memoryBlock, ULONG byteSize)
{
    struct MemoryControl *mc = &lib->l_MemControl;

    if( TypeOfMem(memoryBlock) & MEMF_CHIP ) {

        FreePooled(mc->chipPool, memoryBlock, byteSize);
        mc->chipAllocated -= byteSize;
        
    } else {
        
        FreePooled(mc->fastPool, memoryBlock, byteSize);
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

