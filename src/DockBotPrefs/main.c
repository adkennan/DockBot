
#include "prefs.h"

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/dos_protos.h>

#include <libraries/triton.h>
#include <proto/triton.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <stdio.h>

struct Library *DockBotBase;
struct Library *UtilityBase;

STRPTR positions[] = { "Left", "Right", "Top", "Bottom", NULL };
STRPTR alignments[] = { "Top/Left", "Center", "Bottom/Right", NULL };

enum {
    OBJ_MENU_ABOUT  = 1001,
    OBJ_MENU_QUIT   = 1002,

    OBJ_POSITION    = 1003,
    OBJ_ALIGNMENT   = 1004,
    
    OBJ_GADGETS     = 1005,

    OBJ_BTN_NEW     = 1006,
    OBJ_BTN_DELETE  = 1007,
    OBJ_BTN_EDIT    = 1008,
    OBJ_BTN_UP      = 1009,
    OBJ_BTN_DOWN    = 1010,

    OBJ_BTN_SAVE    = 1011,
    OBJ_BTN_USE     = 1012,
    OBJ_BTN_TEST    = 1013,
    OBJ_BTN_CANCEL  = 1014,

    OBJ_BTN_GAD_OK  = 1015,
    OBJ_BTN_GAD_CAN = 1016,

    OBJ_NEW_GADGET  = 1017,
    OBJ_BTN_NEW_OK  = 1018,
    OBJ_BTN_NEW_CAN = 1019,
    OBJ_NEW_STR_NAME= 1020,
    OBJ_NEW_STR_DESC= 1021
};

ProjectDefinition(newGadgetWindowTags)
{
    WindowID(2),
    WindowTitle("New Dock Gadget"),
    WindowPosition(TRWP_CENTERDISPLAY),
    WindowFlags(TRWF_HELP),
    QuickHelpOn(TRUE),
    VertGroupEA,
        Space,
        HorizGroupA,
            Space,
            TextN("Choose Gadget"),
            Space,    
        EndGroup,
        Space,
        HorizGroupA,
            Space,
            ListSSN(NULL, OBJ_NEW_GADGET, 0, 0),
            Space,
        EndGroup,
        HorizGroupEA,
            Space,
            NamedFrameBox("About"),
                VertGroupA,
                    Space,
                    HorizGroupA,
                        Space,
                        TROB_Text, 0L, TRAT_ID, OBJ_NEW_STR_DESC,
                            TRAT_Text, (ULONG)"\n\n\n\n\n", 
                            TRAT_MinWidth, 30,
                            TRAT_Flags, TRTX_MULTILINE | TRTX_NOUNDERSCORE,
                        Space,
                    EndGroup,
                    Space,
                EndGroup,
            Space,
        EndGroup,
        Space,
        HorizGroupEC,
            Space,
            Button("_Ok", OBJ_BTN_NEW_OK),
            Space,
            ButtonE("Cancel", OBJ_BTN_NEW_CAN),
            Space,
        EndGroup,
        Space,
    EndGroup
};

ProjectDefinition(mainWindowTags)
{
    WindowID(1),
    WindowTitle("DockBot Preferences"),
    WindowPosition(TRWP_CENTERDISPLAY),
    WindowFlags(TRWF_HELP|TRWF_APPWINDOW),
    QuickHelpOn(TRUE),
    BeginMenu("Project"),
        MenuItem("?_About...", OBJ_MENU_ABOUT),
        ItemBarlabel,
        MenuItem("Q_Quit", OBJ_MENU_QUIT),
    HorizGroupA,
        Space,
        VertGroupA,
            Space,
            NamedFrameBox("Dock Settings"),           
                ColumnArray,
                    Space,
                    BeginColumn,
                        Space,
                        TextN("Position"),
                        Space,
                        TextN("Alignment"),
                        Space,
                    EndColumn,
                    Space,
                    BeginColumn,
                        Space,
                        CycleGadget(positions, 0, OBJ_POSITION),
                        Space,
                        CycleGadget(alignments, 0, OBJ_ALIGNMENT),
                        Space,
                    EndColumn,
                    Space,
                EndArray,
            Space,
            NamedFrameBox("Gadgets"),
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
                        Button("_New...", OBJ_BTN_NEW),
                        Space,
                        Button("_Edit...", OBJ_BTN_EDIT),
                        Space,
                        Button("Delete", OBJ_BTN_DELETE),
                        Space,
                        Button("_Up", OBJ_BTN_UP),
                        Space,
                        Button("_Down", OBJ_BTN_DOWN),
                        Space,
                    EndGroup,
                    Space,
                EndGroup,
            Space,
            HorizGroupEC,    
                Button("_Save", OBJ_BTN_SAVE),
                Space,
                Button("_Use", OBJ_BTN_USE),
                Space,
                Button("_Test", OBJ_BTN_TEST),
                Space,
                ButtonE("Cancel", OBJ_BTN_CANCEL),
            EndGroup,
            Space,
        EndGroup,
        Space,
    EndGroup
    
};

struct TR_Project *mainWindow;


struct TagItem *make_tag_list(ULONG data, ...)
{
    struct TagItem *tags = (struct TagItem *)&data;

    return CloneTagItems(tags);
}


VOID gadget_selected(struct DockPrefs *prefs, ULONG index)
{
    struct DgNode *dg = NULL, *curr;
    ULONG gadCount = 0;

    FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {
        if( gadCount == index ) {
            dg = curr;
        }
        gadCount++;
    }
         
    if( gadCount == 0 ) 
    {
        TR_SetAttribute(mainWindow, OBJ_BTN_UP, TRAT_Disabled, TRUE);
        TR_SetAttribute(mainWindow, OBJ_BTN_DOWN, TRAT_Disabled, TRUE);
        TR_SetAttribute(mainWindow, OBJ_BTN_EDIT, TRAT_Disabled, TRUE);
        TR_SetAttribute(mainWindow, OBJ_BTN_DELETE, TRAT_Disabled, TRUE);
    } else {
        TR_SetAttribute(mainWindow, OBJ_BTN_UP, TRAT_Disabled, index == 0);
        TR_SetAttribute(mainWindow, OBJ_BTN_DOWN, TRAT_Disabled, index == gadCount - 1);
        TR_SetAttribute(mainWindow, OBJ_BTN_EDIT, TRAT_Disabled, !dock_gadget_can_edit(dg->dg));
        TR_SetAttribute(mainWindow, OBJ_BTN_DELETE, TRAT_Disabled, FALSE);
    }
}

VOID free_gadget_list(struct DockPrefs *prefs)
{
    struct Node *ln;

    while( ! IsListEmpty(&prefs->gadLabels) ) {
        if( ln = RemTail(&prefs->gadLabels) ) {
            DB_FreeMem(ln, sizeof(struct Node) + strlen(ln->ln_Name) + 1);
        }        
    }
}

VOID update_gadget_list(struct DockPrefs *prefs)
{
    struct DgNode *curr;
    struct Node *ln;
    STRPTR label;
    UWORD len;

    free_gadget_list(prefs);

    FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {
        dock_gadget_get_label(curr->dg, &label);
        if( ! label ) {
            label = curr->n.ln_Name;
        }
        len = strlen(label) + 1;

        if( ln = (struct Node *)DB_AllocMem(sizeof(struct Node) + len + 1, MEMF_ANY) ) {
            ln->ln_Name = (STRPTR)(ln + 1);
            CopyMem(label, (VOID *)(ln + 1), len);
            AddTail(&prefs->gadLabels, ln);
        }
    }

    TR_SetAttribute(mainWindow, OBJ_GADGETS, 0L, (ULONG)&(prefs->gadLabels));
}

struct DgNode *get_selected_gadget(struct DockPrefs *prefs)
{
    struct DgNode *curr;
    ULONG gadCount = 0, index;
    
    index = TR_GetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value);

    FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {

        if( gadCount == index ) {
            return curr;
        }
        gadCount++;
    }

    return NULL;
}

VOID move_gadget_up(struct DockPrefs *prefs)
{
    struct DgNode* dg;
    struct Node *newPred;
    ULONG index;

    if( (dg = get_selected_gadget(prefs)) && dg->n.ln_Pred ) {

        newPred = dg->n.ln_Pred->ln_Pred;
        Remove((struct Node *)dg);
        Insert(&(prefs->cfg.gadgets), (struct Node *)dg, newPred);

        update_gadget_list(prefs);

        index = TR_GetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value) - 1;
        TR_SetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value, index);
        gadget_selected(prefs, index);
    }
}

VOID move_gadget_down(struct DockPrefs *prefs)
{
    struct DgNode* dg;
    struct Node *newPred;
    ULONG index;

    if( (dg = get_selected_gadget(prefs)) && dg->n.ln_Succ ) {

        newPred = dg->n.ln_Succ;
        Remove((struct Node *)dg);
        Insert(&(prefs->cfg.gadgets), (struct Node *)dg, newPred);

        update_gadget_list(prefs);

        index = TR_GetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value) + 1;
        TR_SetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value, index);
        gadget_selected(prefs, index);

    }
}

VOID delete_gadget(struct DockPrefs *prefs)
{
    struct DgNode *dg;
    ULONG index;

    if( dg = get_selected_gadget(prefs) ) {

        if( TR_EasyRequestTags(Application, 
            "Are you sure you want to remove this gadget?", "_Ok|Cancel", 
            TREZ_LockProject, mainWindow,
            TREZ_Title, "Delete Gadget",
            TREZ_Activate, TRUE,
            TAG_END) == 1 ) {

            remove_dock_gadget(dg);

            update_gadget_list(prefs);

            index = TR_GetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value);
            gadget_selected(prefs, index);
        }
    }
}

ULONG count_tags(struct TagItem *list)
{
    ULONG c = 0;
    struct TagItem *tstate, *tag;

    if( list == NULL ) {
        return 0;
    }

    tstate = list;
    while( tag = NextTagItem(&tstate) ) {
        c++;
    }
    return c;
}

struct TagItem *copy_tags(struct TagItem *dest, struct TagItem *src)
{
    struct TagItem *tstate, *tag;

    tstate = src;
    while( tag = NextTagItem(&tstate) ) {
        dest->ti_Tag = tag->ti_Tag;
        dest->ti_Data = tag->ti_Data;
        dest++;
    }
    return dest;
}

struct TagItem *merge_tag_lists(struct TagItem *head, struct TagItem *middle, struct TagItem *tail)
{
    struct TagItem *newTags, *dest;
    ULONG tagCount = count_tags(head) + count_tags(middle) + count_tags(tail);

    if( newTags = AllocateTagItems(tagCount) ) {

        dest = newTags;
        dest = copy_tags(dest, head);
        if( middle != NULL ) {
            dest = copy_tags(dest, middle);
        }
        dest = copy_tags(dest, tail);

        dest->ti_Tag = TAG_END;
    }

    return newTags;
}

VOID edit_gadget(struct DockPrefs *prefs, struct DgNode *dg)
{    
    struct TagItem *gadTags, *headTags, *windowTags, *tailTags;

    prefs->editGadgetIsNew = FALSE;

    if( dg ) {

        prefs->editGadget = dg;

        if( gadTags = dock_gadget_get_editor(dg->dg) ) {

            if( headTags = make_tag_list(
                    WindowID(2),
                    WindowTitle(dg->n.ln_Name),
                    WindowPosition(TRWP_CENTERDISPLAY),
                    WindowFlags(TRWF_HELP),
                    QuickHelpOn(TRUE),
                    HorizGroupA,
                        VertGroupA,
                            Space,
                            TAG_END
                ) ) {

                if( tailTags = make_tag_list(
                            Space,
                            HorizGroupEC,
                                Space,
                                ButtonR("Ok", OBJ_BTN_GAD_OK),
                                Space,
                                ButtonE("Cancel", OBJ_BTN_GAD_CAN),
                                Space,
                            EndGroup, 
                            Space,                       
                        EndGroup,
                    EndGroup,
                    TAG_END
                ) ) {

                    if( windowTags = merge_tag_lists(headTags, gadTags, tailTags) ) {

                        TR_LockProject(mainWindow);

                        if( !(prefs->editDialog = TR_OpenProject(Application, windowTags) ) ) {
    
                            printf("uh oh\n");
                            TR_UnlockProject(mainWindow);
                        }

                        FreeTagItems(windowTags);
                    }
                    FreeTagItems(tailTags);
                }
                FreeTagItems(headTags);
            }

            FreeTagItems(gadTags);
        }
    }
}

VOID add_dropped_icon(struct DockPrefs *prefs, struct AppMessage *msg)
{
    STRPTR path;
    STRPTR gadName;
    BPTR lock;
    struct FileInfoBlock *fib;
    Object *btn;
    UWORD len;
    struct DgNode *dg;

    if( path = (STRPTR)DB_AllocMem(MAX_PATH_LENGTH, MEMF_ANY) ) {

        path[0] = 0;
        NameFromLock(msg->am_ArgList->wa_Lock, path, MAX_PATH_LENGTH);
        AddPart(path, msg->am_ArgList->wa_Name, MAX_PATH_LENGTH);
        
        if( lock = Lock(path, ACCESS_READ) ) {
            
            if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {

                if( Examine(lock, fib) ) {
    
                   if( fib->fib_DirEntryType < 0 ) {
    
                        // A file was dropped. Create a new DockButton.

                        len = strlen(DB_BUTTON_CLASS) + 1;

                        if( btn = DB_CreateDockGadget(DB_BUTTON_CLASS) ) {                 

                            dock_gadget_init_button(btn, msg->am_ArgList->wa_Name, path);

                            if( gadName = (STRPTR)DB_AllocMem(len, MEMF_ANY) ) {                

                                CopyMem(DB_BUTTON_CLASS, gadName, len);

                                if( dg = add_dock_gadget(prefs, btn, gadName) ) {

                                    update_gadget_list(prefs);

                                    edit_gadget(prefs, dg);                                    

                                    prefs->editGadgetIsNew = TRUE;

                                } else {
                                    DB_FreeMem(gadName, len);
                                    DisposeObject(btn);
                                }                
                            } else {
                                DisposeObject(btn);
                            }
                        }
                        
                    } else {
                        TR_EasyRequestTags(Application,
                            "Sorry, directories cannot be added to the dock.", "_Ok",
                            TREZ_LockProject, mainWindow,
                            TREZ_Title, "Add Gadget",
                            TREZ_Activate, TRUE, TAG_END);
                    }
                }

                FreeDosObject(DOS_FIB, fib);
            }
            UnLock(lock);
        }

        DB_FreeMem(path, MAX_PATH_LENGTH);
    }
}

VOID create_new_gadget(struct DockPrefs *prefs)
{
    struct Node *curr;
    ULONG gadCount = 0, index;
    Object *g;
    struct DgNode *dg;
    STRPTR gadName;
    UWORD len;

    index = TR_GetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, TRAT_Value);

    for( curr = prefs->classes.lh_Head; curr->ln_Succ; curr = curr->ln_Succ ) {
        if( gadCount == index ) {

            len = strlen(curr->ln_Name) + 1;
            if( gadName = DB_AllocMem(len, MEMF_CLEAR) ) {
                CopyMem(curr->ln_Name, gadName, len);

                if( g = DB_CreateDockGadget(gadName) ) {
                    if( dg = add_dock_gadget(prefs, g, gadName) ) {

                        update_gadget_list(prefs);
    
                        if( dock_gadget_can_edit(g) ) {

                            edit_gadget(prefs, dg);
    
                            prefs->editGadgetIsNew = TRUE;
                        }

                        break;
                    }
                    DisposeObject(g);
                }
                DB_FreeMem(gadName, len);
            }

            break;
        }
        gadCount++;
    }
}

VOID class_selected(struct DockPrefs *prefs)
{
    STRPTR name;
    STRPTR desc;
    STRPTR copy;
    STRPTR vers;
    STRPTR text;
    Object *g;
    struct Node *curr;
    ULONG gadCount = 0, index, len;

    index = TR_GetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, TRAT_Value);

    for( curr = prefs->classes.lh_Head; curr->ln_Succ; curr = curr->ln_Succ ) {
        if( gadCount == index ) {

            if( g = DB_CreateDockGadget(curr->ln_Name) ) {

                dock_gadget_get_info(g, &name, &vers, &desc, &copy);

                len = strlen(name) + strlen(vers) + strlen(desc) + strlen(copy) + 8;
                if( text = DB_AllocMem(len, MEMF_CLEAR) ) {

                    sprintf(text, "%s %s\n\n%s\n\n%s\n", name, vers, desc, copy);    

                    TR_SetAttribute(prefs->newGadgetDialog, OBJ_NEW_STR_DESC, TRAT_Text, (ULONG)text);

                    DB_FreeMem(text, len);
                }
                DisposeObject(g);
            }

            break;
        }
        gadCount++;
    }
}

VOID open_new_gadget_dialog(struct DockPrefs *prefs)
{
    if( ! (prefs->newGadgetDialog = TR_OpenProject(Application, newGadgetWindowTags) ) ) {
    
        return;
    }

    TR_LockProject(mainWindow);

    TR_SetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, 0, (ULONG)&prefs->classes);

    class_selected(prefs);
}

VOID run_event_loop(struct DockPrefs *prefs) 
{
    BOOL done = FALSE, closeMsgProj = FALSE;
    struct TR_Message *msg;
    struct TR_Project *msgProj;
    ULONG msgClass, msgID, msgData;
    UWORD openWindowCount = 0;

    while( ! done || openWindowCount > 0 ) {
        TR_Wait(Application, NULL);

        while( msg = TR_GetMsg(Application) ) {
            msgProj = msg->trm_Project;
            msgClass = msg->trm_Class;
            msgID = msg->trm_ID;
            msgData = msg->trm_Data;

            if( msgProj == mainWindow ) {
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
                            case OBJ_BTN_CANCEL:
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
                TR_UnlockProject(mainWindow);
            }
        }
    }
}

int main(char **argv, int argc)
{
    struct DockPrefs prefs;

    if( TR_OpenTriton(TRITON11VERSION,
        TRCA_Name,      "DockBotPrefs",
        TRCA_LongName,  "DockBot Preferences",
        TRCA_Info,      "Preferences editor for DockBot",
        TAG_END) ) {

        if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {

            if( UtilityBase = OpenLibrary("utility.library", 37) ) {

                NewList(&prefs.cfg.gadgets);
                NewList(&prefs.gadLabels);
                NewList(&prefs.classes);

                if( mainWindow = TR_OpenProject(Application, mainWindowTags) ) {

                    if( load_config(&prefs) ) {

                        if( DB_ListClasses(&prefs.classes) ) {

                            TR_SetAttribute(mainWindow, OBJ_POSITION, TRAT_Value, (ULONG)prefs.cfg.pos);
                            TR_SetAttribute(mainWindow, OBJ_ALIGNMENT, TRAT_Value, (ULONG)prefs.cfg.align);
        
                            update_gadget_list(&prefs);

                            gadget_selected(&prefs, 0);

                            run_event_loop(&prefs);
                               
                            remove_dock_gadgets(&prefs);
                            free_gadget_list(&prefs);

                            free_plugins(&prefs);
                        } else {
                            printf("Couldn't load class list.\n");
                        }

                    } else {
                        printf("Couldn't load config.\n");
                    }
                    TR_CloseProject(mainWindow);            
                } else { 
                    printf("Couldn't open window.\n");
                }
                CloseLibrary(UtilityBase);
            }
            CloseLibrary(DockBotBase);
        } else {
            printf("Couldn't open dockbot.library.\n");
        }
        TR_CloseTriton();
    } else {
        printf("Couldn't open triton.\n");
    }
}

