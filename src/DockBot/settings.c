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

    DEBUG(printf("load_config\n"));

    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        if( DB_ReadConfig(&dock->cfg, s) ) {

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
            return FALSE;
        }
    }

    return TRUE; 
}


VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    while( msg = GetMsg(dock->notifyPort) ) {

        if( msg == (struct Message *)&dock->notifyReq ) {
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

    execute_external(dock, (STRPTR)&path, NULL, NULL, TRUE);
}
