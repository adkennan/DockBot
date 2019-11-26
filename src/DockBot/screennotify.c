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

#include <clib/screennotify_protos.h>
#include <libraries/screennotify.h>
#include <pragmas/screennotify_pragmas.h>

#include "dock_gadget.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <stdio.h>

extern struct Library *ScreenNotifyBase;

BOOL init_screennotify(struct DockWindow *dock)
{
    DEBUG(printf("init_screennotify\n"));

    if( ScreenNotifyBase == NULL ) {
        DEBUG(printf("  screennotify.library not available\n"));
        dock->screenNotifyHandle = NULL;
        return TRUE;
    }

    if( dock->screenNotifyMsgPort = CreateMsgPort() ) {

        if( dock->screenNotifyHandle = AddWorkbenchClient(dock->screenNotifyMsgPort, 0) ) {
        
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL free_screennotify(struct DockWindow *dock)
{
    DEBUG(printf("free_screen_notify\n"));

    if( ScreenNotifyBase == NULL ) {
        DEBUG(printf("  screennotify.library not available\n"));
        return TRUE;
    }

    if( dock->screenNotifyHandle ) {

        while( !RemWorkbenchClient(dock->screenNotifyHandle) ) {

           Delay(10);
        }

        delete_port(dock->screenNotifyMsgPort);

        return TRUE;        
    }

    return FALSE;
}

VOID handle_screennotify(struct DockWindow *dock)
{
    struct ScreenNotifyMessage *snm;
    
    while( snm = (struct ScreenNotifyMessage *) GetMsg(dock->screenNotifyMsgPort) ) {

        if( snm->snm_Type == SCREENNOTIFY_TYPE_WORKBENCH ) {

            if( snm->snm_Value == (APTR)TRUE ) {
                dock->runState = RS_SHOWING;
            }
            
            if( snm->snm_Value == (APTR)FALSE ) {
                dock->runState = RS_HIDING;
            }

            ReplyMsg((struct Message *) snm);
        }
    }
}

