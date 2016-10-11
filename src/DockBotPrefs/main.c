
#include "prefs.h"

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>

#include <libraries/triton.h>
#include <proto/triton.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <stdio.h>

struct Library *DockBotBase;

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
    OBJ_BTN_GAD_CAN = 1016
};

ProjectDefinition(mainWindowTags)
{
    WindowID(1),
    WindowTitle("DockBot Preferences"),
    WindowPosition(TRWP_CENTERDISPLAY),
    WindowFlags(TRWF_HELP),
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

VOID gadget_selected(struct DockPrefs *prefs, ULONG index)
{
    struct DgNode *dg = NULL, *curr;
    ULONG gadCount = 0;
    
    for( curr = (struct DgNode *)prefs->gadgets.lh_Head;
                curr->n.ln_Succ;
                curr = (struct DgNode *)curr->n.ln_Succ ) {

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

VOID update_gadget_list(struct DockPrefs *prefs)
{
    TR_SetAttribute(mainWindow, OBJ_GADGETS, 0L, (ULONG)&(prefs->gadgets));
}

struct DgNode *get_selected_gadget(struct DockPrefs *prefs)
{
    struct DgNode *curr;
    ULONG gadCount = 0, index;
    
    index = TR_GetAttribute(mainWindow, OBJ_GADGETS, TRAT_Value);

    for( curr = (struct DgNode *)prefs->gadgets.lh_Head;
                curr->n.ln_Succ;
                curr = (struct DgNode *)curr->n.ln_Succ ) {

        if( gadCount == index ) {
            return curr;
        }
        gadCount++;
    }

    return NULL;
}

struct DgNode *get_gadget_from_window(struct DockPrefs *prefs, struct TR_Project *window)
{
    struct DgNode *curr;
    for( curr = (struct DgNode *)prefs->gadgets.lh_Head;
                curr->n.ln_Succ;
                curr = (struct DgNode *)curr->n.ln_Succ ) {
        
        if( curr->editor == window ) {
            return curr;
        }
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
        Insert(&(prefs->gadgets), (struct Node *)dg, newPred);

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
        Insert(&(prefs->gadgets), (struct Node *)dg, newPred);

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

            Remove((struct Node *)dg);

            DisposeObject(dg->dg);
            
            if( dg->editor ) {
                TR_CloseProject(dg->editor);                
            }

            DB_FreeMem(dg, sizeof(struct DgNode));

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
        dest = copy_tags(dest, middle);
        dest = copy_tags(dest, tail);

        dest->ti_Tag = TAG_END;
    }

    return newTags;
}

VOID edit_gadget(struct DockPrefs *prefs)
{
    struct DgNode *dg;
    struct TagItem *gadTags, *headTags, *windowTags, *tailTags;

    if( dg = get_selected_gadget(prefs) ) {

        if( gadTags = dock_gadget_get_editor(dg->dg) ) {

            if( headTags = make_tag_list(
                    WindowID(2),
                    WindowTitle(dock_gadget_get_name(dg->dg)),
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

                        if( !(dg->editor = TR_OpenProject(Application, windowTags) ) ) {
    
                            printf("uh oh\n");
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

VOID run_event_loop(struct DockPrefs *prefs) 
{
    BOOL done = FALSE, closeMsgProj = FALSE;
    struct TR_Message *msg;
    struct DgNode *dg;
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

                            case OBJ_BTN_EDIT:
                                edit_gadget(prefs);
                                break;
                        }
                        break;

                    case TRMS_NEWVALUE:
                        switch( msgID ) {
                            case OBJ_GADGETS:
                                gadget_selected(prefs, msgData);
                                break;

                            case OBJ_POSITION:
                                prefs->pos = msgData;
                                break;

                            case OBJ_ALIGNMENT:
                                prefs->align = msgData;
                                break;
                        }
                        break;
                }
            } else {
                if( dg = get_gadget_from_window(prefs, msgProj) ) {
                
                    switch( msgClass ) {
                        case TRMS_CLOSEWINDOW:
                            closeMsgProj = TRUE;
                            dg->editor = NULL;
                            break;
                        
                        case TRMS_ACTION:
                            switch( msgID ) {
                                case OBJ_BTN_GAD_OK:
                                    dock_gadget_update(dg->dg, msgProj);
                                    closeMsgProj = TRUE;
                                    dg->editor = NULL;
                                    break;

                                case OBJ_BTN_GAD_CAN:
                                    closeMsgProj = TRUE;
                                    dg->editor = NULL;
                                    break;

                                default:
                                    dock_gadget_handle_event(dg->dg, msg);
                                    break;
                            }
                            break;

                        default:
                            dock_gadget_handle_event(dg->dg, msg);
                            break;
      
                    }
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

            NewList(&prefs.gadgets);
            NewList((struct List *)&prefs.libs);

            if( prefs.baseClass = init_prefs_base_class() ) {

                if( prefs.buttonClass = init_dock_button_class() ) {

                    if( init_icon_class() ) {
  
                        if( mainWindow = TR_OpenProject(Application, mainWindowTags) ) {

                            if( load_config(&prefs) ) {

                                TR_SetAttribute(mainWindow, OBJ_POSITION, TRAT_Value, (ULONG)prefs.pos);
                                TR_SetAttribute(mainWindow, OBJ_ALIGNMENT, TRAT_Value, (ULONG)prefs.align);
    
                                update_gadget_list(&prefs);

                                gadget_selected(&prefs, 0);

                                run_event_loop(&prefs);
                               
                                remove_dock_gadgets(&prefs);
                            }
                            TR_CloseProject(mainWindow);            
                        }
                    }
                    free_dock_button_class(prefs.buttonClass);
                }
                free_prefs_base_class(prefs.baseClass);
            }
            CloseLibrary(DockBotBase);
        }


        TR_CloseTriton();
    }
}

