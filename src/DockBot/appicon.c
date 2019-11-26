/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <workbench/workbench.h>
#include <clib/wb_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>

BOOL show_app_icon(struct DockWindow *dock)
{
    DEBUG(printf("show_app_icon\n"));

    if( dock->iconObj = GetDiskObjectNew(MIN_ICON) ) {
        dock->iconObj->do_Type = NULL;

        if( dock->aiPort = CreateMsgPort() ) {

            if( dock->appIcon = AddAppIcon(
                0L, 0L, APP_NAME, dock->aiPort, NULL, dock->iconObj, NULL) ) {

                return TRUE;                
            }
            delete_port(dock->aiPort);
        }

        FreeDiskObject(dock->iconObj);
    }
    dock->appIcon = NULL;
    dock->iconObj = NULL;
    dock->aiPort = NULL;
    return FALSE;
}

VOID free_app_icon(struct DockWindow *dock)
{
    DEBUG(printf("free_app_icon\n"));

    if( dock->appIcon ) {
        RemoveAppIcon(dock->appIcon);
        dock->appIcon = NULL;
    }

    delete_port(dock->aiPort);
    dock->aiPort = NULL;

    if( dock->iconObj ) {
        FreeDiskObject(dock->iconObj);
        dock->iconObj = NULL;
    }
}

VOID handle_icon_event(struct DockWindow* dock)
{
    struct AppMessage *msg;

    while( msg = (struct AppMessage *)GetMsg(dock->aiPort)) {
        switch( msg->am_Type ) {

          case AMTYPE_APPICON:
            dock->runState = RS_UNICONIFYING;
            break;
        }

        ReplyMsg((struct Message*)msg);
    }
}

