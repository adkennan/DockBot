
#include <exec/types.h>
#include <intuition/classes.h>
#include <libraries/mui.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>

#include <stdio.h>

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

static const char *_positions[] = { "Left", "Right", "Top", "Bottom", NULL };
static const char *_hAligns[] = { "Left", "Center", "Right", NULL };
static const char *_vAligns[] = { "Top", "Middle", "Bottom", NULL };
static const char *_startTypes[] = { "Workbench", "Shell", NULL };

struct Library *MUIMasterBase;

int main(int argc, char *argv[])
{
    Object *app;
    Object *win;
    BOOL running = TRUE;
    ULONG signal;

    if( MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN) ) {

        app = ApplicationObject,
            MUIA_Application_Title,         "DockBotPrefs",
            MUIA_Application_Version,       "$VER: DockBotPrefs 1.0 (18.04.16)",
            MUIA_Application_Copyright,     "©2016, Andrew Kennan",
            MUIA_Application_Author,        "Andrew Kennan",
            MUIA_Application_Description,   "Preferences editor for DockBot.",
            MUIA_Application_Base,          "DOCKBOTPREFS",
            SubWindow, win = WindowObject,
                MUIA_Window_ID,             MAKE_ID('M', 'A', 'I', 'N'),
                MUIA_Window_Title,          "DockBot Preferences",
                WindowContents, VGroup, 
                    Child, HGroup, GroupFrameT("Dock Settings"),
                        Child, HGroup,
                            Child, VGroup,
                                Child, Label2("Position"),
                                Child, Label2("Alignment"),
                            End,
                            Child, VGroup,
                                Child, Cycle(_positions),
                                Child, Cycle(_hAligns),
                            End,
                        End,
                    End,
                    Child, HGroup, GroupFrameT("Gadgets"),
                        Child, HGroup, 
                            Child, ListviewObject,
                                MUIA_Listview_Input, TRUE,      
                                MUIA_Listview_List, ListObject,
                                InputListFrame,
                                End,
                            End,
                            Child, VGroup,
                                Child, HGroup,
                                    Child, Label2("dockbutton"),
                                End,
                                Child, HGroup,
                                    Child, VGroup,
                                        Child, Label2("Name"),
                                        Child, Label2("Path"),
                                        Child, Label2("Icon"),
                                        Child, Label2("Start"),  
                                    End,
                                    Child, VGroup,
                                        Child, String("", 40),
                                        Child, String("", 40),
                                        Child, String("", 40),
                                        Child, Cycle(_startTypes),
                                    End,
                                End,
                            End,
                        End,
                    End,
                    Child, HGroup, MUIA_Group_SameSize, TRUE,
                        Child, SimpleButton("_Save" ),
                        Child, SimpleButton("_Use"),
                        Child, SimpleButton("_Test"),
                        Child, SimpleButton("_Cancel"),
                        End,
                    End,
                End,
            End;

        if( ! app ) {
            printf("Failed to create Application.");
        } else {

            DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 
                2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

            set(win, MUIA_Window_Open, TRUE);

            while( running ) {
                switch( DoMethod(app, MUIM_Application_Input, &signal) ) {
                    case MUIV_Application_ReturnID_Quit:
                        running = FALSE;
                        break;
                }
            }

            if( running && signal ) {
                Wait(signal);
            }

            MUI_DisposeObject(app);
        }
        CloseLibrary(MUIMasterBase);
    }
}
