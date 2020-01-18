/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <dos/dos.h>
#include <dos/rdargs.h>
#include <clib/dos_protos.h>

#ifdef DEBUG_BUILD

BOOL __DebugEnabled = FALSE;

#define TEMPLATE "DEBUG/S"
#define OPT_DEBUG 0

VOID parse_args(VOID)
{
    struct RDArgs *rd;
    LONG values[] = {
        (LONG)FALSE
    };

    if( rd = ReadArgs(TEMPLATE, values, NULL ) ) {

        __DebugEnabled = values[OPT_DEBUG];

        FreeArgs(rd);
    }        
}

#endif
