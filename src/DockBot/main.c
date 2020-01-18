/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <exec/memory.h>
#include <exec/libraries.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include <string.h>

const char __ver[40] = "$VER:" APP_NAME " " VERSION_STR;

struct Library *DosBase;
struct Library *LayersBase;
struct Library *GfxBase;
struct Library *IntuitionBase;
struct Library *WorkbenchBase;
struct Library *DockBotBase;
struct Library *GadToolsBase;
struct Library *CxBase;
struct Library *ScreenNotifyBase;
struct Library *LocaleBase;

BOOL already_running(VOID)
{
    if( FindPort(APP_NAME) ) {
        return TRUE;
    }
    return FALSE;
}

struct Library *open_lib(STRPTR name, UWORD version, BOOL optional) 
{
    struct Library *lib;
    STRPTR msg;
    UWORD l;

    DEBUG(printf("Opening %s V %d...", name, version));

    if( !(lib = OpenLibrary(name, version) ) ) {

        DEBUG(printf("Failed\n"));

        if( optional ) {
            return NULL;
        }

        l = strlen(name) + strlen(MSG_ERR_OpenLibrary);
        if( msg = DB_AllocMem(l, MEMF_ANY) ) {
            sprintf(msg, MSG_ERR_OpenLibrary, version, name);
            DB_ShowError(msg);
            DB_FreeMem(msg, l);
        }
    } else {
        DEBUG(printf("OK\n"));
    }

    return lib;
}

VOID close_lib(STRPTR name, struct Library *lib) 
{
    if( lib ) {
        DEBUG(printf("Closing %s\n", name));
        CloseLibrary(lib);
    }
}

int main(int argc, char** argv)
{
    struct DockWindow *dock;
 
    if( already_running() ) {
        return 0;
    }

#ifdef DEBUG_BUILD

    parse_args();

#endif

    if( DockBotBase = OpenLibrary("PROGDIR:dockbot.library", 1) ) {

        DEBUG(DB_RegisterDebugStream(Output()));

        if( DosBase = open_lib("dos.library", 39, FALSE) ) {

            if( GfxBase = open_lib("graphics.library", 39, FALSE) ) {

                if( LayersBase = open_lib("layers.library", 39, FALSE) ) {

                    if( IntuitionBase = open_lib("intuition.library", 39, FALSE) ) {
    
                        if( GadToolsBase = open_lib("gadtools.library", 39, FALSE) ) {
        
                            if( WorkbenchBase = open_lib("workbench.library", 39, FALSE) ) {

                                if( CxBase = open_lib("commodities.library", 39, FALSE) ) {

                                    if( LocaleBase = open_lib("locale.library", 39, FALSE) ) {
    
                                        ScreenNotifyBase = open_lib("screennotify.library", 1, TRUE);

                                        open_catalog();

                                        if( dock = create_dock() ) {
                                                                   
                                            run_event_loop(dock);                     

                                            free_dock(dock);   
                                        }
    
                                        close_catalog();
                                            
                                        close_lib("screennotify.library", ScreenNotifyBase);
                                        close_lib("locale.library", LocaleBase);
                                    }
                                    close_lib("commodities.library", CxBase);
                                }
                                close_lib("workbench.library", WorkbenchBase);
                            }
                            close_lib("gadtools.library", GadToolsBase);
                        }        
                        close_lib("intuition.library", IntuitionBase);
                    }
                    close_lib("layers.library", LayersBase);
                }
                close_lib("graphics.library", GfxBase);
            }
            close_lib("dos.library", DosBase);
        }  
        close_lib("dockbot.library", DockBotBase);
    }
}
