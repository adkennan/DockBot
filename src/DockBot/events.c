/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/io.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <devices/timer.h>

#include <stdio.h>

#include "dock.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "gadget_msg.h"

#include "debug.h"

#define DOCK_SIG(dw) (1 << dw->awPort->mp_SigBit)
#define WIN_SIG(dw) (1 << dw->win->UserPort->mp_SigBit)
#define NOTIFY_SIG(dw) (1 << dw->notifyPort->mp_SigBit)
#define TIMER_SIG(dw) (1 << dw->timerPort->mp_SigBit)
#define GADGET_SIG(dw) (1 << dw->gadgetPort->mp_SigBit)

BOOL init_timer_notification(struct DockWindow *dock)
{
    if( dock->timerPort = CreateMsgPort() ) {

        if( dock->timerReq = (struct timerequest *)CreateExtIO(dock->timerPort, sizeof(struct timerequest) ) ) {
            
            if( OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)dock->timerReq, 0L) == 0 ) {

                return TRUE;
            }
        }
    }
    return FALSE;
}

VOID set_timer(struct DockWindow *dock, ULONG milliseconds) 
{
    dock->timerReq->tr_node.io_Command = TR_ADDREQUEST;
    dock->timerReq->tr_time.tv_secs = milliseconds / 1000;
    dock->timerReq->tr_time.tv_micro = (milliseconds % 1000) * 1000;

    SendIO((struct IORequest *)dock->timerReq);
}


VOID handle_drop_event(struct DockWindow* dock)
{
    struct AppMessage *msg;
    struct WBArg *arg;
    UWORD i, len;
    STRPTR tmpBuf = NULL;
    STRPTR* buffers;
    Object *gadget;

    while( msg = (struct AppMessage *)GetMsg(dock->awPort)) {
        if( msg->am_NumArgs > 0 && 
            (gadget = get_gadget_at(dock, msg->am_MouseX, msg->am_MouseY)) ) {

            arg = msg->am_ArgList;

            if( tmpBuf == NULL ) {
                tmpBuf = DB_AllocMem(2048, MEMF_CLEAR);
            }

            buffers = DB_AllocMem(msg->am_NumArgs * sizeof(STRPTR), MEMF_ANY);
            for( i = 0; i < msg->am_NumArgs; i++ ) {

                NameFromLock(arg->wa_Lock, tmpBuf, 2048);
                AddPart(tmpBuf, arg->wa_Name, 2048);
                len = strlen(tmpBuf);

                buffers[i] = DB_AllocMem(len + 1, MEMF_ANY);
                CopyMem(tmpBuf, buffers[i], len + 1);

                arg++;
            }

            dock_gadget_drop(gadget, buffers, msg->am_NumArgs);

            for( i = 0; i < msg->am_NumArgs; i++ ) {
                DB_FreeMem(buffers[i], strlen(buffers[i]) + 1);
            }
            DB_FreeMem(buffers, msg->am_NumArgs * sizeof(STRPTR));
        }

        ReplyMsg((struct Message*)msg);
    }

    if( tmpBuf != NULL ) {
        DB_FreeMem(tmpBuf, 2048);
    }
}


VOID handle_window_event(struct DockWindow *dock)
{
    struct IntuiMessage *msg;
    Object *gadget;
    UWORD menuItem;
    UWORD menuNum;
    struct MenuItem *item;

    while( msg = (struct IntuiMessage *)GetMsg(dock->win->UserPort) ) {
    
        if( dock->runState == RS_RUNNING ) {

            switch( msg->Class )
            {
                case IDCMP_MOUSEBUTTONS: 
                    if( msg->Code == SELECTUP ) {

                        if( gadget = get_gadget_at(dock, msg->MouseX, msg->MouseY) ) {
        
                            dock_gadget_click(gadget, msg->MouseX, msg->MouseY);
                        }
                    }
                    break;
    
                case IDCMP_CHANGEWINDOW:
                case IDCMP_REFRESHWINDOW:
                    BeginRefresh(dock->win);
                    draw_gadgets(dock);
                    EndRefresh(dock->win, TRUE);
                    break;
        
                case IDCMP_MENUPICK:
                    menuNum = msg->Code;
                    while( menuNum != MENUNULL ) {
                        item = ItemAddress(dock->menu, menuNum);
                        menuItem = ITEMNUM(menuNum);
                        if( menuItem == 3 ) {
                            dock->runState = RS_QUITTING;
                        }

                        menuNum = item->NextSelect;
                    }
                    break;
            }
        }
        
        ReplyMsg((struct Message *)msg);

    }
}

VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    while( msg = GetMsg(dock->notifyPort) ) {
        ReplyMsg(msg);
    }

    if( dock->runState == RS_RUNNING ) {

        remove_dock_gadgets(dock);

        create_dock_handle(dock);

        load_config(dock);

        enable_layout(dock);
    }
}

VOID handle_timer_message(struct DockWindow *dock)
{
    struct DgNode *curr;
    struct Message *msg;

    while( msg = GetMsg(dock->timerPort) ) {
    }

    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                curr->n.mln_Succ; 
                curr = (struct DgNode *)curr->n.mln_Succ ) {

        dock_gadget_tick(curr->dg);
    }

    switch( dock->runState ) {

        case RS_QUITTING:
            dock->runState = RS_STOPPED;
            break;

        case RS_RUNNING:
            set_timer(dock, TIMER_INTERVAL);
            break;

        default:
            break;
    }

    LOG_MEMORY_TIMED
}

VOID handle_gadget_message(struct DockWindow *dock)
{
    struct DgNode *curr;
    struct GadgetMessage *msg;
    BOOL exists = FALSE;
    
    while( msg = (struct GadgetMessage *)GetMsg(dock->gadgetPort) ) {
        if( dock->runState == RS_RUNNING ) {

            for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                 curr->n.mln_Succ; 
                 curr = (struct DgNode *)curr->n.mln_Succ ) {
                if( msg->sender == curr->dg ) {
                    exists = TRUE;
                    break;
                }
            }

            if( exists ) {

                switch( msg->messageType ) {
                    case GM_DRAW:
                        draw_gadget(dock, msg->sender);
                        break;

                    case GM_QUIT:
                        dock->runState = RS_QUITTING;
                        break;    
                }
            }
        }

        DB_FreeMem(msg, sizeof(struct GadgetMessage));
    }
}

VOID run_event_loop(struct DockWindow *dock)
{
    ULONG signals, winsig, docksig, notifysig, timersig, gadgetsig;
    
    winsig = WIN_SIG(dock);
    docksig = DOCK_SIG(dock);
    notifysig = NOTIFY_SIG(dock);
    timersig = TIMER_SIG(dock);
    gadgetsig = GADGET_SIG(dock);

    while( dock->runState != RS_STOPPED ) {

        signals = Wait( winsig | docksig | notifysig | timersig | gadgetsig | SIGBREAKF_CTRL_C );

        if( signals & winsig ) {
            
            handle_window_event(dock);          
        }

        if( signals & docksig ) {

            handle_drop_event(dock);
        }

        if( signals & notifysig ) {

            handle_notify_message(dock);
        }

        if( signals & timersig ) {
        
            handle_timer_message(dock);
        }

        if( signals & gadgetsig ) {

            handle_gadget_message(dock);

        }

        if( signals & SIGBREAKF_CTRL_C ) {
            SetSignal(0, SIGBREAKF_CTRL_C);
            dock->runState = RS_QUITTING;
        }
    }
}


