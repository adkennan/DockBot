/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/libraries.h>

#include <clib/exec_protos.h>

#include "debug.h"

const char __ver[40] = "$VER:DockBot 1.0 (10.04.2016)";

struct Library *DosBase;
struct Library *LayersBase;
struct Library *GfxBase;
struct Library *IntuitionBase;
struct Library *WorkbenchBase;
struct Library *DockBotBase;
struct Library *IconBase;
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

int main(int argc, char** argv)
{
    struct DockWindow *dock;
 
    if( already_running() ) {
        return 0;
    }
   
    if( DosBase = OpenLibrary("dos.library", 37) ) {
        if( GfxBase = OpenLibrary("graphics.library", 37) ) {

            if( LayersBase = OpenLibrary("layers.library", 37) ) {

                if( IntuitionBase = OpenLibrary("intuition.library", 37) ) {
    
                    if( GadToolsBase = OpenLibrary("gadtools.library", 37) ) {
        
                        if( IconBase = OpenLibrary("icon.library", 46) ) {

                            if( WorkbenchBase = OpenLibrary("workbench.library", 37) ) {

                                if( CxBase = OpenLibrary("commodities.library", 37) ) {
    
                                    if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {
    
                                        if( dock = create_dock() ) {
                    
                                            run_event_loop(dock);                     
        
                                            free_dock(dock);   
                    
                                            LOG_MEMORY
                                        }
                                        CloseLibrary(DockBotBase);
                                    }
                                    CloseLibrary(CxBase);
                                }
                                CloseLibrary(WorkbenchBase);
                            }
                            CloseLibrary(IconBase);
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
}
