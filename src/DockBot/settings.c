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

BOOL load_config(struct DockWindow *dock)
{
    struct DockSettings *s;
    struct DgNode *curr;
    BOOL r = FALSE;

    DEBUG(printf(__FUNC__ ": %s\n", CONFIG_FILE));

    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        DEBUG(printf(__FUNC__ ":  settings opened\n"));

        if( DB_ReadConfig(&dock->cfg, s) ) {

            DEBUG(printf(__FUNC__ ":  config read\n"));

            if( dock->cfg.bgBrushPath ) {

                DEBUG(printf(__FUNC__ ":  load background %s...", dock->cfg.bgBrushPath));
                if( dock->bgBrush = DB_LoadBrush(dock->cfg.bgBrushPath, FALSE) ) {
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


BOOL init_config_notification(struct DockWindow *dock)
{
    DEBUG(printf("init_config_notification\n"));

    if( dock->notifyPort = CreateMsgPort() ) {
        dock->notifyReq.nr_Name = CONFIG_FILE;
        dock->notifyReq.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
        dock->notifyReq.nr_stuff.nr_Msg.nr_Port = dock->notifyPort;
    
        if( ! StartNotify(&dock->notifyReq) ) {
            DEBUG(printf("init_config_notification: FS does not support notification.\n"));
            return FALSE;
        }

        dock->notifyEnabled = TRUE;

        return TRUE;
    }

    DEBUG(printf(" Failed\n"));
    return FALSE; 
}

VOID free_config_notification(struct DockWindow *dock)
{
    DEBUG(printf("free_config_notification\n"));

    if( dock->notifyPort ) {

        if( dock->notifyEnabled ) {
            EndNotify(&dock->notifyReq);
        }
        delete_port(dock->notifyPort);
    }
}

VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    DEBUG(printf("handle_notify_message\n"));

    while( msg = GetMsg(dock->notifyPort) ) {

        dock->runState = RS_LOADING;

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
