/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <exec/memory.h>
#include <exec/libraries.h>

#include <clib/exec_protos.h>

#include <stdio.h>
#include <string.h>

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "debug.h"

const char __ver[40] = "$VER:DockBot 1.0 (03.06.2019)";

struct Library *DosBase;
struct Library *LayersBase;
struct Library *GfxBase;
struct Library *IntuitionBase;
struct Library *WorkbenchBase;
struct Library *DockBotBase;
struct Library *GadToolsBase;
struct Library *CxBase;

struct DockWindow;

struct DockWindow *create_dock(VOID);
VOID run_event_loop(struct DockWindow *);
VOID free_dock(struct DockWindow *);

BOOL already_running(VOID)
{
    if( FindPort("DockBot") ) {
        return TRUE;
    }
    return FALSE;
}

const char __libErr[33] = "Unable to open version %d of %s.";

struct Library *OpenLib(STRPTR name, UWORD version) {

    struct Library *lib;
    STRPTR msg;
    UWORD l;

    if( !(lib = OpenLibrary(name, version) ) ) {
        l = strlen(name) + strlen(__libErr);
        if( msg = DB_AllocMem(l, MEMF_ANY) ) {
            sprintf(msg, __libErr, version, name);
            DB_ShowError(msg);
            DB_FreeMem(msg, l);
        }
    }

    return lib;
}

int main(int argc, char** argv)
{
    struct DockWindow *dock;
 
    if( already_running() ) {
        return 0;
    }

    if( DockBotBase = OpenLibrary("PROGDIR:dockbot.library", 1) ) {
   
        if( DosBase = OpenLib("dos.library", 39) ) {

            if( GfxBase = OpenLib("graphics.library", 39) ) {

                if( LayersBase = OpenLib("layers.library", 39) ) {

                    if( IntuitionBase = OpenLib("intuition.library", 39) ) {
    
                        if( GadToolsBase = OpenLib("gadtools.library", 39) ) {
        
                            if( WorkbenchBase = OpenLib("workbench.library", 39) ) {

                                if( CxBase = OpenLib("commodities.library", 39) ) {
    
                                    if( dock = create_dock() ) {
                
                                        run_event_loop(dock);                     
        
                                        free_dock(dock);   
                  
                                        LOG_MEMORY
                                    }
                                    CloseLibrary(CxBase);
                                }
                                CloseLibrary(WorkbenchBase);
                            }
                            CloseLibrary(GadToolsBase);
                        }        
                        CloseLibrary(IntuitionBase);
                    }
                    CloseLibrary(LayersBase);
                }
                CloseLibrary(GfxBase);
            }
            CloseLibrary(DosBase);
        }  
        CloseLibrary(DockBotBase);
    } else {
        printf("ERROR: DockBot.library not found.");
    }
}
