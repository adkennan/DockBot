
#include <exec/types.h>
#include <intuition/classes.h>
#include <libraries/mui.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>

#include <stdio.h>

#include "pref_editor.h"

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

static const char *_positions[] = { "Left", "Right", "Top", "Bottom", NULL };
static const char *_hAligns[] = { "Left", "Center", "Right", NULL };
static const char *_vAligns[] = { "Top", "Middle", "Bottom", NULL };
static const char *_startTypes[] = { "Workbench", "Shell", NULL };

struct Library *MUIMasterBase;
struct Library *DockBotBase;

VOID dump_config(struct GadgetList *config)
{
    struct DockGadgetInfo *gi;

    for( gi = (struct DockGadgetInfo *)config->gadgets.mlh_Head;
                gi->n.mln_Succ;
                gi = (struct DockGadgetInfo *)gi->n.mln_Succ) {
        printf("%s : %s : %s : %s : %s : %s\n",
            gi->gadgetName, gi->name, gi->version,
            gi->description, gi->copyright);
    }
}

LONG __asm __saveds display_gadget(register __a2 char **array, register __a1 struct DockGadgetInfo *dgi)
{
    *array = dgi->gadgetName;

    return 0;
}

int main(int argc, char *argv[])
{
    Object *app;
    Object *win;
    BOOL running = TRUE;
    ULONG signal;
    struct GadgetList *config;
    struct DockGadgetInfo *dgi;
    APTR gadgetList;

    struct Hook dispHook = { { NULL, NULL }, (VOID *)display_gadget, NULL, NULL };

    if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {
    
        if( ! (config = load_config() ) ) {

            return;
        }

    if( MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN) ) {

        gadgetList = ListviewObject,
                                MUIA_Listview_Input, TRUE,      
                                MUIA_Listview_List, ListObject,
                                    InputListFrame,
                                    MUIA_List_DisplayHook, &dispHook,
                                End,
                            End;

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
                            Child, gadgetList,
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

            if( config ) {
                for( dgi = (struct DockGadgetInfo *)config->gadgets.mlh_Head;
                            dgi->n.mln_Succ;
                            dgi = (struct DockGadgetInfo *)dgi->n.mln_Succ) {
                    DoMethod(gadgetList, MUIM_List_InsertSingle, dgi, MUIV_List_Insert_Bottom);
                }
            }

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
        if( config ) {
            free_config(config);
        }
        CloseLibrary(DockBotBase);
    }
}
