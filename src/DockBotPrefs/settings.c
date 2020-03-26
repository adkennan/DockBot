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

    DEBUG(printf("add_dock_gadget: %s\n", name));

    if( dg = DB_AllocGadget(name) ) {

        dg->dg = obj;

        AddTail((struct List *)&prefs->cfg.gadgets, (struct Node *)dg);

        dock_gadget_init_edit(dg->dg, Application);

        return dg;
    }
   
    return NULL;
}

VOID remove_dock_gadget(struct DgNode *dg)
{
    DEBUG(printf("remove_dock_gadget: %s\n", dg->n.ln_Name));

    Remove((struct Node *)dg);

    DB_FreeGadget(dg);
}

VOID remove_dock_gadgets(struct DockPrefs *prefs)
{
    DEBUG(printf("remove_dock_gadgets\n"));

    DB_DisposeConfig(&prefs->cfg);
}

VOID free_plugins(struct DockPrefs *prefs)
{
    struct Node *node;
    UWORD len;

    DEBUG(printf("free_plugins\n"));

    while( ! IsListEmpty(&prefs->classes) ) {
        if( node = RemTail(&prefs->classes) ) {

            len = sizeof(struct Node) + strlen(node->ln_Name) + 1;

            DB_FreeMem(node, len);
        }
    }
}

VOID init_config(struct DockPrefs *prefs)
{
    DEBUG(printf(__FUNC__ "\n"));

    prefs->cfg.align = DA_CENTER;
    prefs->cfg.pos = DP_RIGHT;
    prefs->cfg.showGadgetLabels = TRUE;
    prefs->cfg.showGadgetBorders = TRUE;
    prefs->cfg.bgBrushPath = NULL;
    
    NewList(&prefs->cfg.gadgets);
    NewList(&prefs->gadLabels);
    NewList(&prefs->classes);
}

VOID free_config(struct DockPrefs *prefs) 
{
    DEBUG(printf(__FUNC__ "\n"));
       
    remove_dock_gadgets(prefs);

    free_gadget_list(prefs);

    free_plugins(prefs);

    DEBUG(printf(__FUNC__ ": Freeing %s\n", prefs->cfg.bgBrushPath));
    FREE_STRING(prefs->cfg.bgBrushPath);
}

BOOL load_config(struct DockPrefs *prefs)
{
    struct DockSettings *s;
    BOOL r = FALSE;
    struct DgNode *curr;

    DEBUG(printf("load_config\n"));

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

    DEBUG(printf("save_config: %s\n", permanent ? CONFIG_FILE_PERM : CONFIG_FILE));

    if( s = DB_OpenSettingsWrite(permanent ? CONFIG_FILE_PERM : CONFIG_FILE) ) {

        if( DB_WriteConfig(&prefs->cfg, s) ) {

            r = TRUE;
        }

        DB_CloseSettings(s);
    }

    return r;
}

VOID revert_config(VOID) {

    BPTR fh1;
    BPTR fh2;
    struct FileInfoBlock *fib;
    BYTE *buf;

    DEBUG(printf("revert_config\n"));

    if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {
           
        if( fh1 = Open(CONFIG_FILE_PERM, MODE_OLDFILE) ) {

            if( ExamineFH(fh1, fib) ) {

                if( buf = DB_AllocMem(fib->fib_Size, MEMF_ANY) ) {
                
                    Read(fh1, buf, fib->fib_Size);

                    if( fh2 = Open(CONFIG_FILE, MODE_READWRITE) ) {

                        SetFileSize(fh2, 0, OFFSET_BEGINNING);

                        Write(fh2, buf, fib->fib_Size);

                        Close(fh2);
                    }

                    DB_FreeMem(buf, fib->fib_Size);

                }

            }

            Close(fh1);

        }

        FreeDosObject(DOS_FIB, fib);
    }
}

