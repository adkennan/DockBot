/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/libraries.h>

#include <clib/exec_protos.h>

const char __ver[40] = "$VER:DockBot 1.0 (10.04.2016)";

struct Library *LayersBase;
struct Library *GfxBase;
struct Library *IntuitionBase;
struct Library *WorkbenchBase;
struct Library *DockBotBase;
struct Library *IconBase;
struct Library *GadToolsBase;

struct DockWindow;

struct DockWindow *create_dock_window(VOID);
VOID enable_layout(struct DockWindow *);
VOID run_event_loop(struct DockWindow *);
VOID close_dock_window(struct DockWindow *);

int main(int argc, char** argv)
{
    struct DockWindow *dock;
    
    if( GfxBase = OpenLibrary("graphics.library", 37) ) {

        if( LayersBase = OpenLibrary("layers.library", 37) ) {

            if( IntuitionBase = OpenLibrary("intuition.library", 37) ) {

                if( GadToolsBase = OpenLibrary("gadtools.library", 37) ) {
        
                    if( IconBase = OpenLibrary("icon.library", 46) ) {

                        if( WorkbenchBase = OpenLibrary("workbench.library", 37) ) {
    
                            if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {

                                if( dock = create_dock_window() ) {
                    
                                    enable_layout(dock);

                                    run_event_loop(dock);                     
        
                                    close_dock_window(dock);   
                                }
                                CloseLibrary(DockBotBase);
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
}
