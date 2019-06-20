/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "prefs.h"

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include <dos/exall.h>

#include <stdio.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <proto/triton.h>

#define DIR_BUF_SIZE 512


struct DgNode * add_dock_gadget(struct DockPrefs *prefs, Object *obj, STRPTR name)
{
    struct DgNode *dg;

    if( dg = (struct DgNode *)DB_AllocMem(sizeof(struct DgNode), MEMF_CLEAR) ) {
        dg->n.ln_Name = name;
        dg->dg = obj;

        AddTail((struct List *)&prefs->cfg.gadgets, (struct Node *)dg);

        dock_gadget_init_edit(dg->dg, Application);

        return dg;
    }
   
    return NULL;
}

VOID remove_dock_gadget(struct DgNode *dg)
{
    Remove((struct Node *)dg);

    FREE_STRING(dg->n.ln_Name);

    DisposeObject(dg->dg);
            
    DB_FreeMem(dg, sizeof(struct DgNode));
}

VOID remove_dock_gadgets(struct DockPrefs *prefs)
{
    struct DgNode *dg;

    while( ! IsListEmpty((struct List *)&prefs->cfg.gadgets) ) {
        if( dg = (struct DgNode *)RemTail((struct List *)&prefs->cfg.gadgets) ) {

            remove_dock_gadget(dg);
 
        }
    }    
}

VOID free_plugins(struct DockPrefs *prefs)
{
    struct Node *node;
    UWORD len;

    while( ! IsListEmpty(&prefs->classes) ) {
        if( node = RemTail(&prefs->classes) ) {

            len = sizeof(struct Node) + strlen(node->ln_Name) + 1;

            DB_FreeMem(node, len);
        }
    }
}

BOOL load_config(struct DockPrefs *prefs)
{
    struct DockSettings *s;
    BOOL r = FALSE;
    struct DgNode *curr;

    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        if( DB_ReadConfig(&prefs->cfg, s) ) {

            r = TRUE;

            FOR_EACH_GADGET(&prefs->cfg.gadgets, curr) {
                dock_gadget_init_edit(curr->dg, Application);
            }
        }

        DB_CloseSettings(s);
    }
    return r;    
}

BOOL save_config(struct DockPrefs *prefs, BOOL permanent)
{
    struct DockSettings *s;
    BOOL r = FALSE;

    if( s = DB_OpenSettingsWrite(permanent ? CONFIG_FILE_PERM : CONFIG_FILE) ) {

        if( DB_WriteConfig(&prefs->cfg, s) ) {

            r = TRUE;
        }

        DB_CloseSettings(s);
    }

    return r;
}

