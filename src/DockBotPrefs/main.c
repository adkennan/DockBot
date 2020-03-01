/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "prefs.h"

#include <dos/dos.h>
#include <dos/rdargs.h>

#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include <proto/triton.h>

const char __ver[40] = "$VER:" APP_NAME " " VERSION_STR;

struct Library *DockBotBase;
struct Library *UtilityBase;

STRPTR positions[] = { 0/*"Left"*/, 0/*"Right"*/, 0/*"Top"*/, 0/*"Bottom"*/, NULL };
STRPTR alignments[] = { 0/*"Top/Left"*/, 0/*"Center"*/, 0/*"Bottom/Right"*/, NULL };

#define DEFAULT_PATH "SYS:"

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

#define MAX_PATH_LEN 1024

VOID update_background_path(struct DockPrefs *prefs, struct TR_Project *window, STRPTR newPath)
{
    FREE_STRING(prefs->cfg.bgBrushPath);

    DEBUG(printf(__FUNC__ ": bgBrushPath = %s\n", newPath));

    if( prefs->cfg.bgBrushPath = DB_AllocMem(strlen(newPath) + 1, MEMF_ANY) ) {
                  
        strcpy(prefs->cfg.bgBrushPath, newPath);

        TR_SetAttribute(window, OBJ_BG_BRUSH, 0L, (ULONG)prefs->cfg.bgBrushPath);
    }
}

VOID select_background(struct DockPrefs *prefs, struct TR_Project *window)
{
    STRPTR newPath;

    if( newPath = DB_SelectFile(MSG_FR_ChooseBackground
                                , MSG_OK
                                , MSG_Cancel
                                , prefs->cfg.bgBrushPath) ) {

        update_background_path(prefs, window, newPath);

        FREE_STRING(newPath);
    }  
}

TR_Project *open_main_window(VOID) {

    DEBUG(printf("open_main_window\n"));

    positions[0] = (STRPTR)MSG_POS_Left;
    positions[1] = (STRPTR)MSG_POS_Right;
    positions[2] = (STRPTR)MSG_POS_Top;
    positions[3] = (STRPTR)MSG_POS_Bottom;

    alignments[0] = (STRPTR)MSG_ALIGN_TopLeft;
    alignments[1] = (STRPTR)MSG_ALIGN_Center;
    alignments[2] = (STRPTR)MSG_ALIGN_BottomRight;

    return TR_OpenProjectTags(Application, 

    WindowID(1),
    WindowTitle(MSG_MW_Title),
    WindowPosition(TRWP_CENTERDISPLAY),
    WindowFlags(TRWF_HELP|TRWF_APPWINDOW),
    QuickHelpOn(TRUE),
    BeginMenu(MSG_MENU_Project),
        MenuItem(MSG_MENU_About, OBJ_MENU_ABOUT),
        ItemBarlabel,
        MenuItem(MSG_MENU_Quit, OBJ_MENU_QUIT),
    HorizGroupA,
        Space,
        VertGroupA,
            Space,
            NamedFrameBox(MSG_MW_DockSettings),
                LineArray,
                    Space,
                    BeginLine,
                        Space,
                        TextN(MSG_MW_Position),
                        Space,
                        CycleGadget(positions, 0, OBJ_POSITION),
                        Space,
                    EndLine,
                    Space,
                    BeginLine,
                        Space,
                        TextN(MSG_MW_Alignment),
                        Space,
                        CycleGadget(alignments, 0, OBJ_ALIGNMENT),
                        Space,
                    EndLine,
                    Space,
                    BeginLine,
                        Space,
                        TextN(MSG_MW_ShowLabels),
                        Space,
                        HorizGroupSAC,
                            Space,
                            CheckBox(OBJ_SHOW_LABELS),
                            Space,
                        EndGroup,
                        Space,
                    EndLine,
                    Space,
                    BeginLine,
                        Space,
                        TextN(MSG_MW_ShowBorders),
                        Space,
                        HorizGroupSAC,
                            Space,
                            CheckBox(OBJ_SHOW_BORDERS),
                            Space,
                        EndGroup,
                        Space,
                    EndLine,
                    Space,
                    BeginLine,
                        Space,
                        TextN(MSG_MW_Background),
                        Space,
                        HorizGroup,
                            StringGadget(NULL, OBJ_BG_BRUSH),                
                            GetFileButton(OBJ_BTN_BG_BRUSH),
                        EndGroup,
                        Space,
                    EndLine,
                    Space,
                EndArray,
            Space,
            NamedFrameBox(MSG_MW_Gadgets),
                HorizGroupA,
                    Space,
                    VertGroupA,
                        Space,
                        ListSSN(NULL, OBJ_GADGETS, 0, 0),
                        Space,
                    EndGroup,
                    Space,
                    VertGroupA,
                        Space,
                        Button(MSG_MW_NewGadget, OBJ_BTN_NEW),
                        Space,
                        Button(MSG_MW_EditGadget, OBJ_BTN_EDIT),
                        Space,
                        Button(MSG_MW_DeleteGadget, OBJ_BTN_DELETE),
                        Space,
                        Button(MSG_MW_GadgetUp, OBJ_BTN_UP),
                        Space,
                        Button(MSG_MW_GadgetDown, OBJ_BTN_DOWN),
                        Space,
                    EndGroup,
                    Space,
                EndGroup,
            Space,
            HorizGroupEC,    
                Button(MSG_MW_ConfigSave, OBJ_BTN_SAVE),
                Space,
                Button(MSG_MW_ConfigUse, OBJ_BTN_USE),
                Space,
                Button(MSG_MW_ConfigTest, OBJ_BTN_TEST),
                Space,
                ButtonE(MSG_Cancel, OBJ_BTN_CANCEL),
            EndGroup,
            Space,
        EndGroup,
        Space,
    EndGroup);    
}

VOID run_event_loop(struct DockPrefs *prefs) 
{
    BOOL done = FALSE, closeMsgProj = FALSE;
    struct TR_Message *msg;
    struct TR_Project *msgProj;
    ULONG msgClass, msgID, msgData;
    UWORD openWindowCount = 0;

    DEBUG(printf("run_event_loop\n"));

    while( ! done || openWindowCount > 0 ) {
        TR_Wait(Application, NULL);

        while( msg = TR_GetMsg(Application) ) {
            msgProj = msg->trm_Project;
            msgClass = msg->trm_Class;
            msgID = msg->trm_ID;
            msgData = msg->trm_Data;

            if( msgProj == prefs->mainWindow ) {
                switch( msgClass ) {
                    case TRMS_CLOSEWINDOW:
                        done = TRUE;
                        break;

                    case TRMS_ICONDROPPED:
                        add_dropped_icon(prefs, (struct AppMessage *)msg->trm_Data);
                        break;

                    case TRMS_ACTION:
                        switch( msgID ) {
                            case OBJ_MENU_QUIT:
                                done = TRUE;
                                break;

                            case OBJ_BTN_CANCEL:
                                revert_config();
                                done = TRUE;
                                break;

                            case OBJ_BTN_UP:
                                move_gadget_up(prefs);
                                break;

                            case OBJ_BTN_DOWN:
                                move_gadget_down(prefs);
                                break;

                            case OBJ_BTN_DELETE:
                                delete_gadget(prefs);
                                break;

                            case OBJ_BTN_NEW:
                                open_new_gadget_dialog(prefs);
                                break;


                            case OBJ_BTN_EDIT:
                                edit_gadget(prefs, get_selected_gadget(prefs));
                                break;

                            case OBJ_BTN_TEST:
                                save_config(prefs, FALSE);
                                break;

                            case OBJ_BTN_USE:
                                save_config(prefs, FALSE);
                                done = TRUE;
                                break;

                            case OBJ_BTN_SAVE:
                                save_config(prefs, TRUE);
                                save_config(prefs, FALSE);
                                done = TRUE;
                                break;
                               
                            case OBJ_BTN_BG_BRUSH:
                                select_background(prefs, msgProj);
                                break;
                        }
                        break;

                    case TRMS_NEWVALUE:
                        switch( msgID ) {
                            case OBJ_GADGETS:
                                gadget_selected(prefs, msgData);
                                break;

                            case OBJ_POSITION:
                                prefs->cfg.pos = msgData;
                                break;

                            case OBJ_ALIGNMENT:
                                prefs->cfg.align = msgData;
                                break;

                            case OBJ_SHOW_LABELS:
                                prefs->cfg.showGadgetLabels = (BOOL)msgData;
                                break;

                            case OBJ_SHOW_BORDERS:
                                prefs->cfg.showGadgetBorders = (BOOL)msgData;
                                break;

                            case OBJ_BG_BRUSH:
                                update_background_path(prefs, msgProj, (STRPTR)msgData);
                                break;
                        }
                        break;
                }
            } else if( msgProj == prefs->newGadgetDialog ) {

                switch( msgClass ) {

                    case TRMS_CLOSEWINDOW:
                        closeMsgProj = TRUE;
                        prefs->newGadgetDialog = NULL;
                        break;

                    case TRMS_NEWVALUE:
                        switch( msgID ) {
                            case OBJ_NEW_GADGET:
                                class_selected(prefs);
                                break;
                        }
                        break;
                    
                    case TRMS_ACTION:
                        switch( msgID ) {

                            case OBJ_BTN_NEW_OK:
                                closeMsgProj = TRUE;
                                create_new_gadget(prefs);
                                prefs->newGadgetDialog = NULL;
                                break;

                            case OBJ_BTN_NEW_CAN:
                                closeMsgProj = TRUE;
                                prefs->newGadgetDialog = NULL;
                                break;

                        }
                        break;
                }
            
            } else if( prefs->editGadget ) {
                
                switch( msgClass ) {
                    case TRMS_CLOSEWINDOW:
                        closeMsgProj = TRUE;
                        prefs->editGadget = NULL;
                        break;
                        
                    case TRMS_ACTION:
                        switch( msgID ) {
                           case OBJ_BTN_GAD_OK:
                               dock_gadget_editor_update(prefs->editGadget->dg, msgProj);
                               closeMsgProj = TRUE;
                               prefs->editGadget = NULL;
                               update_gadget_list(prefs);
                               break;

                           case OBJ_BTN_GAD_CAN:
                               if( prefs->editGadgetIsNew ) {
                                  remove_dock_gadget(prefs->editGadget);
                                  update_gadget_list(prefs);
                               }
                               closeMsgProj = TRUE;
                               prefs->editGadget = NULL;
                               break;

                           default:
                              dock_gadget_editor_event(prefs->editGadget->dg, msgProj, msg);
                              break;
                        }
                        break;

                    default:
                        dock_gadget_editor_event(prefs->editGadget->dg, msgProj, msg);
                        break;
    
                }
            }

            TR_ReplyMsg(msg);

            if( closeMsgProj ) {
                TR_CloseProject(msgProj);
                closeMsgProj = FALSE;
                TR_UnlockProject(prefs->mainWindow);
            }
        }
    }

    DEBUG(printf("run_event_loop: Done.\n"));
}


VOID show_open_lib_err(STRPTR name, UWORD version) {
    STRPTR msg;
    UWORD l;
  
    l = strlen(name) + strlen(MSG_ERR_OpenLibrary);
    if( msg = DB_AllocMem(l, MEMF_ANY) ) {
        sprintf(msg, MSG_ERR_OpenLibrary, version, name);
        DEBUG(printf(msg));
        DB_ShowError(msg);
        DB_FreeMem(msg, l);
    }
}

struct Library *open_lib(STRPTR name, UWORD version) {

    struct Library *lib;

    DEBUG(printf("open_lib: %s V %d\n", name, version));

    if( !(lib = OpenLibrary(name, version) ) ) {
        show_open_lib_err(name, version);
    }

    return lib;
}

VOID close_lib(STRPTR name, struct Library *lib) {

    if( lib ) {
        DEBUG(printf("close_lib: %s\n", name));
        CloseLibrary(lib);
    }
}  

int main(char **argv, int argc)
{
    struct DockPrefs prefs;

#ifdef DEBUG_BUILD

    parse_args();

#endif

    if( DockBotBase = OpenLibrary("PROGDIR:dockbot.library", 1) ) {

        DEBUG(DB_RegisterDebugStream(Output()));

        if( TR_OpenTriton(TRITON11VERSION,
            TRCA_Name,      "DockBotPrefs",
            TRCA_LongName,  MSG_APP_Title,
            TRCA_Info,      MSG_APP_Info,
            TAG_END) ) {

            if( UtilityBase = open_lib("utility.library", 37) ) {

                prefs.cfg.align = DA_CENTER;
                prefs.cfg.pos = DP_RIGHT;
                prefs.cfg.showGadgetLabels = TRUE;
                prefs.cfg.showGadgetBorders = TRUE;
                prefs.cfg.bgBrushPath = NULL;
    
                NewList(&prefs.cfg.gadgets);
                NewList(&prefs.gadLabels);
                NewList(&prefs.classes);

                if( prefs.mainWindow = open_main_window() ) {
    
                    if( load_config(&prefs) ) {
    
                        if( DB_ListClasses(&prefs.classes) ) {

                            TR_SetAttribute(prefs.mainWindow, OBJ_POSITION, TRAT_Value, (ULONG)prefs.cfg.pos);
                            TR_SetAttribute(prefs.mainWindow, OBJ_ALIGNMENT, TRAT_Value, (ULONG)prefs.cfg.align);
                            TR_SetAttribute(prefs.mainWindow, OBJ_SHOW_LABELS, TRAT_Value, (ULONG)prefs.cfg.showGadgetLabels);
                            TR_SetAttribute(prefs.mainWindow, OBJ_SHOW_BORDERS, TRAT_Value, (ULONG)prefs.cfg.showGadgetBorders);
                            TR_SetAttribute(prefs.mainWindow, OBJ_BG_BRUSH, 0L, (ULONG)prefs.cfg.bgBrushPath);
        
                            update_gadget_list(&prefs);

                            gadget_selected(&prefs, 0);

                            run_event_loop(&prefs);
                               
                            free_config(&prefs);

                        } else {
                            DB_ShowError((STRPTR)MSG_ERR_ClassList);
                        }

                    } else {
                        DB_ShowError((STRPTR)MSG_ERR_LoadConfig);
                        }
                    TR_CloseProject(prefs.mainWindow);            
                } else { 
                    DB_ShowError((STRPTR)MSG_ERR_OpenWindow);
                }

                close_lib("utility.library", UtilityBase);
            }
            TR_CloseTriton();
        } else {
            show_open_lib_err("triton.library", TRITON11VERSION);
        }
            
        close_lib("PROGDIR:dockbot.library", DockBotBase);
    }
}

