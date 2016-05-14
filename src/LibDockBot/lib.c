/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/types.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>

#include "dock_gadget.h"

int __saveds __UserLibInit(void);
void __saveds __UserLibCleanup(void);

struct Library *SysBase = NULL;
struct Library *DOSBase = NULL;
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
Class *gadgetClass = NULL;

VOID InitMem(VOID);

VOID CleanUpMem(VOID); 

int __saveds __UserLibInit(void)
{
    SysBase = (*((void **)4));

    if(IntuitionBase = OpenLibrary("intuition.library", 39) ) {
        if( GfxBase = OpenLibrary("graphics.library", 39) ) {
            if( DOSBase = OpenLibrary("dos.library", 39) ) {

                InitMem();

                if( gadgetClass = init_dock_gadget_class() ) {
                    return 0;
                }

                CleanUpMem();

                CloseLibrary(DOSBase);
            }

            CloseLibrary(GfxBase);
        }
        CloseLibrary(IntuitionBase);
    }    

    return 1;

}

void __saveds __UserLibCleanup(void)
{
    free_dock_gadget_class(gadgetClass);

    CleanUpMem();

    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
    CloseLibrary(DOSBase);
}


