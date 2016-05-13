/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#include <string.h>

struct MemoryControl
{
    APTR fastPool;
    APTR chipPool;
    ULONG fastAllocated;
    ULONG chipAllocated;
    ULONG fastAllocCount;
    ULONG chipAllocCount;
};

struct MemoryControl *mc = NULL;

VOID InitMem(VOID) 
{
    if( mc = (struct MemoryControl *)AllocMem(sizeof(struct MemoryControl), MEMF_CLEAR) ) {

        if( AvailMem(MEMF_FAST) > 0 ) {
            mc->fastPool = CreatePool(MEMF_FAST, 4096, 2048);
        }
        mc->chipPool = CreatePool(MEMF_CHIP, 4096, 2048);
    }
}

VOID CleanUpMem(VOID)
{
    if( mc ) {
        if( mc->fastPool ) {
            DeletePool(mc->fastPool);
        }
        if( mc->chipPool ) {
            DeletePool(mc->chipPool);
        }

        FreeMem(mc, sizeof(struct MemoryControl));
    }
}


VOID* AllocMemInternal(ULONG byteSize, ULONG attributes)
{
    VOID *result;
    ULONG i;
  
    if( (attributes & MEMF_CHIP) || ! mc->fastPool ) {
       
        if( result = AllocPooled(mc->chipPool, byteSize) ) {
            mc->chipAllocated += byteSize;
            mc->chipAllocCount++;
        }   
        
    } else {
        if( result = AllocPooled(mc->fastPool, byteSize) ) {
            mc->fastAllocated += byteSize;
            mc->fastAllocCount++;
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

VOID FreeMemInternal(VOID *memoryBlock, ULONG byteSize)
{
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
    return mc;
}

VOID* __asm __saveds DB_AllocMem(
    register __d0 ULONG byteSize,
    register __d1 ULONG attributes)
{
    return AllocMemInternal(byteSize, attributes);
}

VOID __asm __saveds DB_FreeMem(
    register __a0 VOID *memoryBlock,
    register __d0 ULONG byteSize)
{
    FreeMemInternal(memoryBlock, byteSize);
}

