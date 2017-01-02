/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __LIB_H__
#define __LIB_H__

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <intuition/classes.h>

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

struct DockBotLibrary {
    struct Library          l_Lib;
    UWORD                   l_Pad;
    BPTR                    l_SegList;
    struct Library          *l_ExecBase;
    struct Library          *l_IntuitionBase;
    struct Library          *l_DOSBase;
    struct Library          *l_GfxBase;

    struct MemoryControl    l_MemControl;
    struct IClass *         l_GadgetClass;
    struct MinList          l_ClassLibs;
};

BOOL InitClassLibs(struct DockBotLibrary *lib);
VOID FreeClassLibs(struct DockBotLibrary *lib);
UWORD ClassLibCount(struct DockBotLibrary *lib);

BOOL InitMem(struct DockBotLibrary *lib);
VOID CleanUpMem(struct DockBotLibrary *lib);
VOID* AllocMemInternal(struct DockBotLibrary *lib, ULONG byteSize, ULONG attributes);
VOID FreeMemInternal(struct DockBotLibrary *lib, VOID *memoryBlock, ULONG byteSize);

BOOL InitGadgetClass(struct DockBotLibrary *lib);
VOID FreeGadgetClass(struct DockBotLibrary *lib);

#endif

