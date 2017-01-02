/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include "dock.h"

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include "dock_gadget.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <stdio.h>

BOOL load_config(struct DockWindow *dock)
{
    struct DockSettings *s;
    struct DgNode *curr;
    BOOL r = FALSE;

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
    if( dock->notifyPort = CreateMsgPort() ) {
        dock->notifyReq.nr_Name = CONFIG_FILE;
        dock->notifyReq.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
        dock->notifyReq.nr_stuff.nr_Msg.nr_Port = dock->notifyPort;
    
        if( StartNotify(&dock->notifyReq) ) {
            return TRUE;
        }
    }
    return FALSE; 
}
