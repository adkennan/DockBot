/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "prefs.h"

#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include <proto/triton.h>


VOID create_new_gadget(struct DockPrefs *prefs)
{
    struct Node *curr;
    ULONG gadCount = 0, index;
    struct DgNode *dg;

    DEBUG(printf("create_new_gadget\n"));

    index = TR_GetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, TRAT_Value);

    for( curr = prefs->classes.lh_Head; curr->ln_Succ; curr = curr->ln_Succ ) {
        if( gadCount == index ) {

            if( dg = add_dock_gadget(prefs, curr->ln_Name) ) {

                update_gadget_list(prefs);
    
                if( dock_gadget_can_edit(dg->dg) ) {

                    edit_gadget(prefs, dg);
    
                    prefs->editGadgetIsNew = TRUE;
                }
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

    DEBUG(printf("class_selected\n"));

    index = TR_GetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, TRAT_Value);

    for( curr = prefs->classes.lh_Head; curr->ln_Succ; curr = curr->ln_Succ ) {
        if( gadCount == index ) {

            if( g = DB_CreateDockGadget(curr->ln_Name) ) {

                dock_gadget_get_info(g, &name, &vers, &desc, &copy);

                DEBUG(printf(" -> %s %s\n", name, vers));

                len = strlen(name) + strlen(vers) + strlen(desc) + strlen(copy) + 8;
                if( text = DB_AllocMem(len, MEMF_CLEAR) ) {

                    sprintf(text, "%s %s\n\n%s\n\n%s\n", name, vers, desc, copy);    

                    TR_SetAttribute(prefs->newGadgetDialog, OBJ_NEW_STR_DESC, TRAT_Text, (ULONG)text);

                    DB_FreeMem(text, len);
                }
                DisposeObject(g);
            } else {

                DEBUG(printf(" -> Could not create gadget %s\n", curr->ln_Name));

            }

            break;
        }
        gadCount++;
    }
}

VOID open_new_gadget_dialog(struct DockPrefs *prefs)
{
    DEBUG(printf("open_new_gadget_dialog\n"));

    if( ! (prefs->newGadgetDialog = TR_OpenProjectTags(Application,         
        WindowID(2),
        WindowTitle(MSG_NGD_Title),
        WindowPosition(TRWP_CENTERDISPLAY),
        WindowFlags(TRWF_HELP),
        QuickHelpOn(TRUE),
        VertGroupEA,
            Space,
            HorizGroupA,
                Space,
                TextN(MSG_NGD_ChooseGadget),
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
                NamedFrameBox(MSG_NGD_About),
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
                Button(MSG_OK, OBJ_BTN_NEW_OK),
                Space,
                ButtonE(MSG_Cancel, OBJ_BTN_NEW_CAN),
                Space,
            EndGroup,
            Space,
        EndGroup) ) ) {
    
        return;
    }

    TR_LockProject(prefs->mainWindow);

    TR_SetAttribute(prefs->newGadgetDialog, OBJ_NEW_GADGET, 0, (ULONG)&prefs->classes);

    class_selected(prefs);
}


VOID add_dropped_icon(struct DockPrefs *prefs, struct AppMessage *msg)
{
    STRPTR path;
    BPTR lock;
    struct FileInfoBlock *fib;
    struct DgNode *dg;

    DEBUG(printf("add_dropped_icon\n"));

    if( path = (STRPTR)DB_AllocMem(MAX_PATH_LENGTH, MEMF_ANY) ) {

        path[0] = 0;
        NameFromLock(msg->am_ArgList->wa_Lock, path, MAX_PATH_LENGTH);
        AddPart(path, msg->am_ArgList->wa_Name, MAX_PATH_LENGTH);
        
        DEBUG(printf("  -> %s\n", path));

        if( lock = Lock(path, ACCESS_READ) ) {
            
            if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {

                if( Examine(lock, fib) ) {
    
                   if( fib->fib_DirEntryType < 0 ) {
    
                        // A file was dropped. Create a new DockButton.

                        if( dg = add_dock_gadget(prefs, DB_BUTTON_CLASS) ) {

                            dock_gadget_init_button(dg->dg, msg->am_ArgList->wa_Name, path);

                            update_gadget_list(prefs);

                            edit_gadget(prefs, dg);                                    

                            prefs->editGadgetIsNew = TRUE;
                        }

                    } else {
                        TR_EasyRequestTags(Application,
                            (STRPTR)MSG_ERR_CantAddDirectory,
                            (STRPTR)MSG_OK,
                            TREZ_LockProject, prefs->mainWindow,
                            TREZ_Title, MSG_NGD_Title,
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
