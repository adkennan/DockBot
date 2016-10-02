/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/io.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/gadtools.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/wb_protos.h>
#include <clib/icon_protos.h>
#include <clib/commodities_protos.h>
#include <devices/timer.h>

#include <stdio.h>

#include "dock.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "gadget_msg.h"

#include "debug.h"

#define DOCK_SIG(dw) (dw->awPort ? (1 << dw->awPort->mp_SigBit) : 0)
#define ICON_SIG(dw) (dw->aiPort ? (1 << dw->aiPort->mp_SigBit) : 0)
#define WIN_SIG(dw) (dw->win ? (1 << dw->win->UserPort->mp_SigBit) : 0)
#define NOTIFY_SIG(dw) (dw->notifyPort ? (1 << dw->notifyPort->mp_SigBit) : 0)
#define TIMER_SIG(dw) (dw->timerPort ? (1 << dw->timerPort->mp_SigBit) : 0)
#define GADGET_SIG(dw) (dw->gadgetPort ? (1 << dw->gadgetPort->mp_SigBit) : 0)
#define CX_SIG(dw) (dw->cxPort ? (1 << dw->cxPort->mp_SigBit) : 0)

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

BOOL show_app_icon(struct DockWindow *dock)
{
    if( dock->iconObj = GetDiskObjectNew(MIN_ICON) ) {
        dock->iconObj->do_Type = NULL;

        if( dock->aiPort = CreateMsgPort() ) {

            if( dock->appIcon = AddAppIcon(
                0L, 0L, "DockBot", dock->aiPort, NULL, dock->iconObj, NULL) ) {

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

VOID handle_drop_event(struct DockWindow* dock)
{
    struct AppMessage *msg;
    struct WBArg *arg;
    UWORD i, len;
    STRPTR tmpBuf = NULL;
    STRPTR* buffers;
    Object *gadget;

    while( msg = (struct AppMessage *)GetMsg(dock->awPort)) {
        switch( msg->am_Type ) {
          case AMTYPE_APPWINDOW:
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
            break;

          case AMTYPE_APPICON:
            dock->runState = RS_SHOWING;
            break;
        }

        ReplyMsg((struct Message*)msg);
    }

    if( tmpBuf != NULL ) {
        DB_FreeMem(tmpBuf, 2048);
    }
}


VOID handle_icon_event(struct DockWindow* dock)
{
    struct AppMessage *msg;

    while( msg = (struct AppMessage *)GetMsg(dock->aiPort)) {
        switch( msg->am_Type ) {

          case AMTYPE_APPICON:
            dock->runState = RS_SHOWING;
            break;
        }

        ReplyMsg((struct Message*)msg);
    }
}

VOID handle_window_event(struct DockWindow *dock)
{
    struct IntuiMessage *msg;
    Object *gadget;
    MenuIndex menuItem;
    UWORD mouseX, mouseY, msgClass, msgCode, menuNum;
    struct MenuItem *item;

    while( msg = (struct IntuiMessage *)GetMsg(dock->win->UserPort) ) {
    
        msgClass = msg->Class;
        msgCode = msg->Code;
        mouseX = msg->MouseX;
        mouseY = msg->MouseY;

        ReplyMsg((struct Message *)msg);
        
        switch( msgClass )
        {
            case IDCMP_MOUSEBUTTONS: 
                if( msgCode == SELECTUP ) {
                    if( gadget = get_gadget_at(dock, mouseX, mouseY) ) {
      
                        dock_gadget_click(gadget, mouseX, mouseY);
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
                menuNum = msgCode;
                while( menuNum != MENUNULL ) {
                    item = ItemAddress(dock->menu, menuNum);
                    menuItem = (MenuIndex)GTMENUITEM_USERDATA(item);
                    switch( menuItem ){
                       case MI_QUIT:
                            dock->runState = RS_QUITTING;
                            break;

                       case MI_ABOUT:
                            show_about(dock);
                            break;

                       case MI_HIDE:
                            dock->runState = RS_HIDING;
                            break;

                       default:
                            break;
                    }
                    menuNum = item->NextSelect;
                }
                break;
        }
        
    }
}

VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    while( msg = GetMsg(dock->notifyPort) ) {
        ReplyMsg(msg);
    }

    dock->runState = RS_LOADING;
}

VOID handle_timer_message(struct DockWindow *dock)
{
    struct DgNode *curr;
    struct Message *msg;

    while( msg = GetMsg(dock->timerPort) ) {
    }

    if( dock->runState == RS_RUNNING ) {
        for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                    curr->n.mln_Succ; 
                    curr = (struct DgNode *)curr->n.mln_Succ ) {

            dock_gadget_tick(curr->dg);
        }
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

VOID handle_cx_message(struct DockWindow *dock)
{
    CxMsg *msg;
    ULONG msgType, msgId, ix;
    struct DgNode *curr;

    while( msg = (CxMsg*)GetMsg(dock->cxPort) ) {

        msgType = CxMsgType(msg);
        msgId = CxMsgID(msg);

        ReplyMsg((struct Message *)msg);

        switch(msgType) {
            
            case CXM_COMMAND:
                switch( msgId ) {
                    case CXCMD_DISABLE:
                        if( dock->cxBroker ) {
                            ActivateCxObj(dock->cxBroker, 0L);
                        }
                        break;
            
                    case CXCMD_ENABLE:
                        if( dock->cxBroker ) {
                            ActivateCxObj(dock->cxBroker, 1L);    
                        }
                        break;

                    case CXCMD_KILL:
                        dock->runState = RS_QUITTING;
                        break;

                    case CXCMD_APPEAR:
                        if( ! dock->win ) {
                            dock->runState = RS_SHOWING;
                        }
                        break;

                    case CXCMD_DISAPPEAR:
                        if( dock->win ) {
                            dock->runState = RS_HIDING;
                        }
                        break;
                }
                break;

            case CXM_IEVENT:
                ix = 0;
                for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                     curr->n.mln_Succ; 
                     curr = (struct DgNode *)curr->n.mln_Succ ) {
                    if( ix == msgId ) {
                        dock_gadget_hotkey(curr->dg);
                        break;
                    }
                    ix++;
                }
                break;
        }
    }
}

VOID run_event_loop(struct DockWindow *dock)
{
    ULONG signals, winsig, iconsig, docksig, notifysig, timersig, gadgetsig, cxsig, totsig;

    while( dock->runState != RS_STOPPED ) {

        switch( dock->runState ) {

            case RS_STARTING:
                if( ! load_config(dock) ) {
                    return;
                }
                if( ! init_cx_broker(dock) ) {
                    return;
                }
                if( ! show_dock_window(dock) ) {
                    return;
                }
                enable_layout(dock);
                set_timer(dock, TIMER_INTERVAL);
                dock->runState = RS_RUNNING;
                break;

            case RS_LOADING:
                disable_layout(dock);
                hide_dock_window(dock);
                remove_dock_gadgets(dock);
                if( ! create_dock_handle(dock) ) {
                    return;
                }
                if( ! load_config(dock) ) {
                    return;
                }
                if( ! init_cx_broker(dock) ) {
                    return;
                }
                if( ! show_dock_window(dock) ) {
                    return;
                }
                enable_layout(dock);
                dock->runState = RS_RUNNING;
                break;

            case RS_HIDING:
                disable_layout(dock);
                hide_dock_window(dock);
                if( ! show_app_icon(dock) ) {
                    return;
                }
                dock->runState = RS_RUNNING;
                break;

            case RS_SHOWING:
                if( ! show_dock_window(dock) ) {
                    return;
                }
                free_app_icon(dock);
                enable_layout(dock);
                dock->runState = RS_RUNNING;
                break;

            case RS_RUNNING:
            case RS_QUITTING:

                winsig = WIN_SIG(dock);
                docksig = DOCK_SIG(dock);
                iconsig = ICON_SIG(dock);
                notifysig = NOTIFY_SIG(dock);
                timersig = TIMER_SIG(dock);
                gadgetsig = GADGET_SIG(dock);
                cxsig = CX_SIG(dock);

                totsig = winsig | docksig | iconsig | notifysig | timersig | gadgetsig | cxsig | SIGBREAKF_CTRL_C;

                while( dock->runState == RS_RUNNING || dock->runState == RS_QUITTING ) {

                    signals = Wait( totsig );

                    if( signals & SIGBREAKF_CTRL_C ) {
                        SetSignal(0, SIGBREAKF_CTRL_C);
                        dock->runState = RS_QUITTING;
                    }

                    if( signals & timersig ) {
                        handle_timer_message(dock);
                    }

                    if( signals & winsig ) {
                        handle_window_event(dock);          
                    }

                    if( signals & docksig ) {
                        handle_drop_event(dock);
                    }

                    if( signals & iconsig ) {
                        handle_icon_event(dock);
                    }

                    if( signals & notifysig ) {
                        handle_notify_message(dock);
                    }

                    if( signals & gadgetsig ) {
                        handle_gadget_message(dock);
                    }

                    if( signals & cxsig ) {
                        handle_cx_message(dock);
                    }
                }
                break;
        }
    }
}


