/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#define ICON_PATH_HI "PROGDIR:icons_hi"
#define ICON_PATH_MED "PROGDIR:icons_med"

VOID free_config(struct DockWindow *dock)
{
    if( dock->bgBrush ) {
        DB_FreeBrush(dock->bgBrush);
        dock->bgBrush = NULL;
    }

    FREE_STRING(dock->cfg.bgBrushPath);
}

BOOL load_config(struct DockWindow *dock)
{
    struct DockSettings *s;
    struct DgNode *curr;
    BOOL r = FALSE;

    DEBUG(printf(__FUNC__ ": %s\n", CONFIG_FILE));

    free_config(dock);

    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        DEBUG(printf(__FUNC__ ":  settings opened\n"));

        if( DB_ReadConfig(&dock->cfg, s) ) {

            DEBUG(printf(__FUNC__ ":  config read\n"));

            if( dock->cfg.bgBrushPath ) {

                DEBUG(printf(__FUNC__ ":  load background %s...", dock->cfg.bgBrushPath));
                if( dock->bgBrush = DB_LoadBrush(dock->cfg.bgBrushPath, BF_NONE) ) {
                    DEBUG(printf("OK\n"));
                } else {
                    DEBUG(printf("FAILED\n"));
                }
            }

            FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

                dock_gadget_added(curr->dg, dock->gadgetPort);
            }

            layout_gadgets(dock);

            r = TRUE;

        }

        DB_CloseSettings(s);
    }
    return r;    
}

VOID write_config(struct DockWindow *dock, STRPTR path)
{
    struct DockSettings *s;

    if( s = DB_OpenSettingsWrite(path) ) {

        DB_WriteConfig(&dock->cfg, s);

        DB_CloseSettings(s);
    }
}

VOID save_config(struct DockWindow *dock)
{
    write_config(dock, CONFIG_FILE_PERM);
    write_config(dock, CONFIG_FILE);
}

VOID disable_notification(struct DockWindow *dock)
{
    if( ! dock->notifyEnabled ) {
        return;
    }

    dock->notifyEnabled = FALSE;       

    if( dock->notifySupported ) {
        EndNotify(&dock->notifyReq);
    }
}   

VOID enable_notification(struct DockWindow *dock)
{
    if( dock->notifyEnabled ) {
        return;
    }

    if( StartNotify(&dock->notifyReq) ) {
        dock->notifyEnabled = TRUE;
        dock->notifySupported = TRUE;
    
    } else {
    
        DEBUG(printf("init_config_notification: FS does not support notification.\n"));
    }
}   


BOOL init_config_notification(struct DockWindow *dock)
{
    DEBUG(printf("init_config_notification\n"));

    if( dock->notifyPort = CreateMsgPort() ) {
        dock->notifyReq.nr_Name = CONFIG_FILE;
        dock->notifyReq.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
        dock->notifyReq.nr_stuff.nr_Msg.nr_Port = dock->notifyPort;
    
        enable_notification(dock);

        return TRUE;
    }

    DEBUG(printf(" Failed\n"));
    return FALSE; 
}

VOID free_config_notification(struct DockWindow *dock)
{
    DEBUG(printf("free_config_notification\n"));

    if( dock->notifyPort ) {

        disable_notification(dock);

        delete_port(dock->notifyPort);
    }
}

VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    DEBUG(printf("handle_notify_message\n"));

    while( msg = GetMsg(dock->notifyPort) ) {

        if( dock->notifyEnabled ) {
            dock->runState = RS_LOADING;
        }

        ReplyMsg(msg);
    }
}

VOID open_settings(struct DockWindow *dock)
{
    UWORD l;
    UBYTE path[256];
    STRPTR pos = (STRPTR)&path;

    COPY_STRING((STRPTR)dock->progPath, pos);
    pos--;
    COPY_STRING("/DockBotPrefs", pos);
    pos--;
    *pos = '\0';

    DEBUG(printf("open_settings: %s\n", path));

    execute_external(dock, (STRPTR)&path, NULL, NULL, TRUE);
}

VOID load_icon_brushes(struct DockWindow *dock)
{
    struct Screen *screen;
    struct DisplayInfo dispInfo;
    ULONG dispMode;
    UBYTE xAspect = 1, yAspect = 1;
    STRPTR path;
    UWORD w, h;

    DEBUG(printf(__FUNC__ ": Loading edit icons\n"));
   
    if( dock->iconBrush != NULL ) {
        DEBUG(printf(__FUNC__ ": Already loaded.\n"));
        return;
    }

    if( screen = LockPubScreen(NULL) ) {

        dispMode = GetVPModeID(&screen->ViewPort);
        if( GetDisplayInfoData(NULL, (UBYTE *)&dispInfo, sizeof(struct DisplayInfo), DTAG_DISP, dispMode) ) {

            xAspect = dispInfo.Resolution.x;
            yAspect = dispInfo.Resolution.y;
        }

        UnlockPubScreen(NULL, screen);
    }

    if( xAspect / yAspect < 2 ) {
        path = ICON_PATH_HI;        
    } else {
        path = ICON_PATH_MED;
    }    

    DEBUG(printf(__FUNC__ ": Loading %s\n", path));
    dock->iconBrush = DB_LoadBrush(path, BF_CREATE_MASK);

    DB_GetBrushSize(dock->iconBrush, &w, &h);

    dock->iconW = w / ICON_COUNT;
    dock->iconH = h;
}

VOID free_icon_brushes(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

    if( dock->iconBrush ) {
        DB_FreeBrush(dock->iconBrush);
    }
}

VOID handle_change_config(struct DockWindow *dock)
{
    struct Node *prev;

    DEBUG(printf(__FUNC__ ": editCount = %d\n", dock->editCount));

    if( dock->editCount == 0 ) {

        switch( dock->editOp ) {

            case EO_MOVE_UP:
                DEBUG(printf(__FUNC__ ": Move Up %s\n", dock->editNode->n.ln_Name));
                prev = dock->editNode->n.ln_Pred->ln_Pred;
                if( prev ) {
                    Remove((struct Node *)dock->editNode);
                    Insert(&dock->cfg.gadgets, (struct Node *)dock->editNode, prev);
                }
                break;
            
            case EO_MOVE_DOWN:
                DEBUG(printf(__FUNC__ ": Move Down %s\n", dock->editNode->n.ln_Name));

                prev = dock->editNode->n.ln_Succ;
                if( prev ) {
                    Remove((struct Node *)dock->editNode);
                    Insert(&dock->cfg.gadgets, (struct Node *)dock->editNode, prev);
                }
                break;

            case EO_DELETE:
                DEBUG(printf(__FUNC__ ": Delete %lx - %s\n", dock->editNode, dock->editNode->n.ln_Name));

                Remove((struct Node *)dock->editNode);

                DEBUG(printf(__FUNC__ ": Removed\n"));                
                DB_FreeGadget(dock->editNode);
                DEBUG(printf(__FUNC__ ": Deleted\n"));                
            
                break;                
        }

        dock->editNode = NULL;
        dock->hoverGad = NULL;
        dock->runState = RS_EDITING;

        hide_gadget_label(dock);
        layout_gadgets(dock);
        update_hover_gadget(dock);
        draw_gadgets(dock);

    } else {

        dock->editCount--;

        draw_gadget(dock, dock->editNode->dg);
    }
}

VOID add_dropped_icon(struct DockWindow *dock, BPTR dir, STRPTR name)
{
    STRPTR path;
    struct FileInfoBlock *fib;
    struct DgNode *dg;
    BPTR lock;

    DEBUG(printf(__FUNC__ "\n"));

    if( path = (STRPTR)DB_AllocMem(MAX_PATH_LENGTH, MEMF_ANY) ) {

        path[0] = 0;
        NameFromLock(dir, path, MAX_PATH_LENGTH);
        AddPart(path, name, MAX_PATH_LENGTH);
        
        DEBUG(printf(__FUNC__ ":  -> %s\n", path));

        if( lock = Lock(path, ACCESS_READ) ) {
            
            if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {

                if( Examine(lock, fib) ) {
    
                   if( fib->fib_DirEntryType < 0 ) {
    
                        // A file was dropped. Create a new DockButton.

                        if( dg = DB_AllocGadget(DB_BUTTON_CLASS) ) {
                
                            dock_gadget_init_button(dg->dg, name, path);

                            AddTail(&dock->cfg.gadgets, (struct Node *)dg);

                            layout_gadgets(dock);

                            draw_gadgets(dock);
                        }
                        
                    } else {
                        DB_ShowError((STRPTR)MSG_ERR_CantAddDirectory);
                    }
                }

                FreeDosObject(DOS_FIB, fib);
            }
            UnLock(lock);
        }

        DB_FreeMem(path, MAX_PATH_LENGTH);
    }
}
