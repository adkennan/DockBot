/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <stdio.h>

#include "debug.h"

#include "dock.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"


struct MemoryControl
{
    APTR fastPool;
    APTR chipPool;
    ULONG fastAllocated;
    ULONG chipAllocated;
    ULONG fastAllocCount;
    ULONG chipAllocCount;
    ULONG fastAllocMax;
    ULONG chipAllocMax;
};

WORD memLogCounter = 0;

VOID log_memory(VOID)
{
    struct MemoryControl *mc = DB_GetMemInfo();
    if( ! mc ) {
        printf("No memory info available\n");
    }
    printf("C: a: %ld, c: %ld, m: %ld   F: a: %ld, c: %ld, m: %ld\n", 
        mc->chipAllocated, mc->chipAllocCount, mc->chipAllocMax,
        mc->fastAllocated, mc->fastAllocCount, mc->fastAllocMax);
}

VOID log_memory_timed(VOID)
{
    memLogCounter--;
    if( memLogCounter <= 0 ) {
        log_memory();
        memLogCounter = 10;
    }
}
