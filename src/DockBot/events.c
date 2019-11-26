/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <clib/exec_protos.h>

#define SIG(port) (port ? (1 << port->mp_SigBit) : 0)

#define DOCK_SIG(dw) SIG(dw->awPort)
#define ICON_SIG(dw) SIG(dw->aiPort)
#define WIN_SIG(dw) (dw->win ? (1 << dw->win->UserPort->mp_SigBit) : 0)
#define NOTIFY_SIG(dw) SIG(dw->notifyPort)
#define TIMER_SIG(dw) SIG(dw->timerPort)
#define GADGET_SIG(dw) SIG(dw->gadgetPort)
#define CX_SIG(dw) SIG(dw->cxPort)
#define SCREENNOTIFY_SIG(dw) SIG(dw->screenNotifyMsgPort)


VOID run_event_loop(struct DockWindow *dock)
{
    ULONG signals, winsig, iconsig, docksig, notifysig, 
          timersig, gadgetsig, cxsig, totsig, screensig;

    while( dock->runState != RS_STOPPED ) {

        switch( dock->runState ) {

            case RS_STARTING:
                DEBUG(printf("runState = RS_STARTING\n"));

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
                DEBUG(printf("runState = RS_LOADING\n"));
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

            case RS_ICONIFYING:
                DEBUG(printf("runState = RS_ICONIFYING\n"));
                disable_layout(dock);
                hide_dock_window(dock);
                if( ! show_app_icon(dock) ) {
                    return;
                }
                dock->runState = RS_RUNNING;
                break;

            case RS_UNICONIFYING:
                DEBUG(printf("runState = RS_UNICONIFYING\n"));
                if( ! show_dock_window(dock) ) {
                    return;
                }
                free_app_icon(dock);
                enable_layout(dock);
                dock->runState = RS_RUNNING;
                break;

            case RS_HIDING:
                DEBUG(printf("runState = RS_HIDING\n"));
                disable_layout(dock);
                hide_dock_window(dock);
                dock->runState = RS_RUNNING;
                break;

            case RS_SHOWING:
                DEBUG(printf("runState = RS_SHOWING\n"));
                if( ! show_dock_window(dock) ) {
                    return;
                }
                enable_layout(dock);
                dock->runState = RS_RUNNING;
                break;

            case RS_RUNNING:
            case RS_QUITTING:
                winsig      = WIN_SIG(dock);
                docksig     = DOCK_SIG(dock);
                iconsig     = ICON_SIG(dock);
                notifysig   = NOTIFY_SIG(dock);
                timersig    = TIMER_SIG(dock);
                gadgetsig   = GADGET_SIG(dock);
                cxsig       = CX_SIG(dock);
                screensig   = SCREENNOTIFY_SIG(dock);

                totsig = winsig | docksig | iconsig | notifysig | timersig | gadgetsig | cxsig | screensig | SIGBREAKF_CTRL_C;

                while( dock->runState == RS_RUNNING || 
                        dock->runState == RS_QUITTING ) {

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

                    if( signals & screensig ) {
                        handle_screennotify(dock);
                    }
                }
                break;
        }
    }

    DEBUG(printf("Exiting event loop.\n"));
}


