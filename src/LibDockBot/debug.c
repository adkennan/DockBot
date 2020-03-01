/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2020 Andrew Kennan
**
************************************/

#include "lib.h"

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>


extern struct DockBotLibrary *DockBotBaseFull;

#ifdef DEBUG_BUILD

BOOL __DebugEnabled = TRUE;

#endif

VOID __asm __saveds DB_RegisterDebugStream(
	register __a0 BPTR fh)
{
    DockBotBaseFull->l_DebugOutput = fh;

    if( fh ) {
        VFPrintf(fh, "Library Logging Enabled\n", NULL);
    }
}


VOID DebugLogInternal(STRPTR fmt, LONG *argv)
{
    if( ! DockBotBaseFull->l_DebugOutput ) {
        return;
    }

    VFPrintf(DockBotBaseFull->l_DebugOutput, fmt, argv);
}

VOID __asm __saveds DB_DebugLog(
	register __a0 STRPTR fmt,
	register __a1 LONG *argv)
{
    DebugLogInternal(fmt, argv);
}

VOID DebugLog(STRPTR fmt, ...)
{
    DebugLogInternal(fmt, (LONG *)(&fmt + 1));
}




