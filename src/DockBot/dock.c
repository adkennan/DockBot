/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <exec/memory.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/wb_protos.h>
#include <clib/gadtools_protos.h>
#include <devices/timer.h>

#include <stdio.h>

#include "dock.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "dock_handle.h"
#include "dock_button.h"

struct NewMenu mainMenu[] = {
    { NM_TITLE, "Project",     0, 0, 0, 0 },
    {  NM_ITEM, "About",       0, 0, 0, 0 },
    {  NM_ITEM, "Settings",    0, 0, 0, 0 },
    {  NM_ITEM, NM_BARLABEL,   0, 0, 0, 0 }, 
    {  NM_ITEM, "Quit",      "Q", 0, 0, 0 },

    { NM_END,   NULL,          0, 0, 0, 0 }
};

BOOL init_dock_window(struct DockWindow *dock)
{
	struct Screen *screen;
    APTR* vi;
	UWORD x,y;
    BOOL result;

	struct TagItem tags[] = {
		{ WA_Left, 0 },
		{ WA_Top, 0 },
		{ WA_Width, 1 },
		{ WA_Height, 1 },
		{ WA_Borderless, TRUE },
        { WA_SmartRefresh, TRUE },
		{ WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW | IDCMP_MENUPICK },
		{ TAG_DONE, NULL }
	};

    result = FALSE;
	if( screen = LockPubScreen(NULL) ) {
		
		x = get_window_left(screen, DA_CENTER, DP_RIGHT, DEFAULT_SIZE);
		y = get_window_top(screen, DA_CENTER, DP_RIGHT, DEFAULT_SIZE);

		tags[0].ti_Data = x;
		tags[1].ti_Data = y;

  		if( dock->win = OpenWindowTagList(NULL, tags) ) {
	
            if( vi = GetVisualInfo(dock->win->WScreen, TAG_END) ) {
        
                if( dock->menu = CreateMenus(mainMenu, TAG_END) ) {           

                    if( LayoutMenus(dock->menu, vi, TAG_END) ) {

                        if( SetMenuStrip(dock->win, dock->menu) ) {

                            if( dock->awPort = CreateMsgPort() ) {

                                if( dock->appWin = AddAppWindow(1, 0, dock->win, dock->awPort, NULL) ) {
                    
                                    result = TRUE;

                                }
                            }
                        }
                    }
                }
                FreeVisualInfo(vi);  
            }   
        }
        UnlockPubScreen(NULL, screen);
    }
    
    return result;     
}

struct DockWindow* create_dock_window(VOID)
{
	struct DockWindow *dock;
    const char *err;

    if( dock = (struct DockWindow *)DB_AllocMem(sizeof(struct DockWindow), MEMF_CLEAR) ) {
	
        dock->runState = RS_STARTING;
        dock->disableLayout = TRUE;
        dock->align = DA_CENTER;
        dock->pos = DP_RIGHT;

        if( init_dock_window(dock) ) {
        
            if( init_gadget_classes(dock) ) {
                        
                if( init_gadgets(dock) ) {    
                
                    if( init_config_notification(dock) ) {
                        
                        if( init_timer_notification(dock) ) {

                            dock->runState = RS_RUNNING;
                            set_timer(dock, TIMER_INTERVAL);

                        } else {
                            // Unable to initialize timer
                            err = "Timer";
                            goto error;
                        }        
                    } else {
                        // Unable to initialize config notifications.
                        err = "Notification";
                        goto error;
                    }
                } else {    
                    // Can't initialize gadgets
                    err = "Config";
                    goto error;
                }
            } else {
                // Can't create gadget classes
                err = "Classes";
                goto error;
            }
        } else {
		    // Can't open window
            err = "Window";
            goto error;
        }
	} else {
        // Can't allocate dock
        err = "Dock";
	}
	return dock;

error:
    if( dock ) {
        dock->runState = RS_STOPPED;

        close_dock_window(dock);
    }

    return NULL;
}


VOID close_dock_window(struct DockWindow* dock)
{
    struct Message *msg;

    remove_dock_gadgets(dock);
    close_class_libs(dock);

    if( dock->appWin ) {
        RemoveAppWindow(dock->appWin);
    }

    if( dock->win ) {

        if( dock->menu ) {

            ClearMenuStrip(dock->win);
            FreeMenus(dock->menu);
        }

    	CloseWindow(dock->win);
    }

    if( dock->awPort ) {

        while( msg = GetMsg(dock->awPort)) {
            ReplyMsg(msg);
        }
        DeleteMsgPort(dock->awPort);
    }

    if( dock->notifyPort ) {

        EndNotify(&dock->notifyReq);
        DeleteMsgPort(dock->notifyPort);
    }

    if( dock->buttonClass ) {

        if( ! free_dock_button_class(dock->buttonClass) ) {
            printf("Could not free button class\n");
        }
    }

    if( dock->handleClass ) {

        if( ! free_dock_handle_class(dock->handleClass) ) {
            printf("Could not free handle class\n");
        }
    }

    if( dock->gadgetClass ) {

        if( !free_dock_gadget_class(dock->gadgetClass) ) {
            printf("Could not free root class\n");
        }
    }
    
    if( dock->timerPort ) {

        DeletePort(dock->timerPort);

        if( dock->timerReq ) {

            CloseDevice((struct IORequest *)dock->timerReq);    
            DeleteExtIO((struct IORequest *)dock->timerReq);    
        }    
    }

    if( dock->gadgetPort ) {

        DeletePort(dock->gadgetPort);
    }

	DB_FreeMem(dock, sizeof(struct DockWindow));
}

