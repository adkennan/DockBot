/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "prefs.h"

#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>

#include <proto/triton.h>

VOID gadget_selected(struct DockPrefs *prefs, ULONG index)
{
    struct DgNode *dg = NULL, *curr;
    ULONG gadCount = 0;

    DEBUG(printf("gadget_selected: %d\n", index));

    FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {
        if( gadCount == index ) {
            dg = curr;
        }
        gadCount++;
    }
         
    if( gadCount == 0 ) 
    {
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_UP, TRAT_Disabled, TRUE);
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_DOWN, TRAT_Disabled, TRUE);
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_EDIT, TRAT_Disabled, TRUE);
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_DELETE, TRAT_Disabled, TRUE);
    } else {
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_UP, TRAT_Disabled, index == 0);
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_DOWN, TRAT_Disabled, index == gadCount - 1);
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_EDIT, TRAT_Disabled, !dock_gadget_can_edit(dg->dg));
        TR_SetAttribute(prefs->mainWindow, OBJ_BTN_DELETE, TRAT_Disabled, FALSE);
    }
}

VOID free_gadget_list(struct DockPrefs *prefs)
{
    struct Node *ln;

    DEBUG(printf("free_gadget_list\n"));

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

    DEBUG(printf("update_gadget_list\n"));

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

    TR_SetAttribute(prefs->mainWindow, OBJ_GADGETS, 0L, (ULONG)&(prefs->gadLabels));
}

struct DgNode *get_selected_gadget(struct DockPrefs *prefs)
{
    struct DgNode *curr;
    ULONG gadCount = 0, index;
    
    DEBUG(printf("get_selected_gadget\n"));

    index = TR_GetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value);

    FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {

        if( gadCount == index ) {

            DEBUG(printf("  -> %s\n", curr->n.ln_Name));

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

    DEBUG(printf("move_gadget_up\n"));

    if( (dg = get_selected_gadget(prefs)) && dg->n.ln_Pred ) {

        newPred = dg->n.ln_Pred->ln_Pred;
        Remove((struct Node *)dg);
        Insert(&(prefs->cfg.gadgets), (struct Node *)dg, newPred);

        update_gadget_list(prefs);

        index = TR_GetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value) - 1;
        TR_SetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value, index);
        gadget_selected(prefs, index);
    }
}

VOID move_gadget_down(struct DockPrefs *prefs)
{
    struct DgNode* dg;
    struct Node *newPred;
    ULONG index;

    DEBUG(printf("move_gadget_down\n"));

    if( (dg = get_selected_gadget(prefs)) && dg->n.ln_Succ ) {

        newPred = dg->n.ln_Succ;
        Remove((struct Node *)dg);
        Insert(&(prefs->cfg.gadgets), (struct Node *)dg, newPred);

        update_gadget_list(prefs);

        index = TR_GetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value) + 1;
        TR_SetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value, index);
        gadget_selected(prefs, index);

    }
}

VOID delete_gadget(struct DockPrefs *prefs)
{
    struct DgNode *dg;
    ULONG index;

    DEBUG(printf("delete_gadget\n"));

    if( dg = get_selected_gadget(prefs) ) {

        if( TR_EasyRequestTags(Application, 
            (STRPTR)MSG_DEL_Text, 
            (STRPTR)MSG_DEL_Buttons, 
            TREZ_LockProject, prefs->mainWindow,
            TREZ_Title, MSG_DEL_Title,
            TREZ_Activate, TRUE,
            TAG_END) == 1 ) {

            remove_dock_gadget(dg);

            update_gadget_list(prefs);

            index = TR_GetAttribute(prefs->mainWindow, OBJ_GADGETS, TRAT_Value);
            gadget_selected(prefs, index);
        }
    }
}


VOID edit_gadget(struct DockPrefs *prefs, struct DgNode *dg)
{    
    struct TagItem *gadTags, *headTags, *windowTags, *tailTags;

    DEBUG(printf("edit_gadget: %s\n", dg->n.ln_Name));

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
                                ButtonR(MSG_OK, OBJ_BTN_GAD_OK),
                                Space,
                                ButtonE(MSG_Cancel, OBJ_BTN_GAD_CAN),
                                Space,
                            EndGroup, 
                            Space,                       
                        EndGroup,
                    EndGroup,
                    TAG_END
                ) ) {

                    if( windowTags = merge_tag_lists(headTags, gadTags, tailTags) ) {

                        TR_LockProject(prefs->mainWindow);

                        if( !(prefs->editDialog = TR_OpenProject(Application, windowTags) ) ) {
    
                            DB_ShowError((STRPTR)MSG_ERR_EditorFailure);
                            TR_UnlockProject(prefs->mainWindow);
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
