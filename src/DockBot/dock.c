/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  � 2016 Andrew Kennan
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
#include <clib/commodities_protos.h>
#include <devices/timer.h>

#include <stdio.h>

#include "dock.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "dock_handle.h"

struct NewMenu mainMenu[] = {
    { NM_TITLE, "Project",     0, 0, 0, 0 },
    {  NM_ITEM, "Settings...","S",0, 0, (APTR)MI_SETTINGS },
    {  NM_ITEM, NM_BARLABEL,   0, 0, 0, 0 }, 
    {  NM_ITEM, "About...",   "?",0, 0, (APTR)MI_ABOUT },
    {  NM_ITEM, "Help",        0, 0, 0, (APTR)MI_HELP },
    {  NM_ITEM, NM_BARLABEL,   0, 0, 0, 0 }, 
    {  NM_ITEM, "Iconify",    "I",0, 0, (APTR)MI_HIDE },
    {  NM_ITEM, NM_BARLABEL,   0, 0, 0, 0 }, 
    {  NM_ITEM, "Quit",       "Q",0, 0, (APTR)MI_QUIT },

    { NM_END,   NULL,          0, 0, 0, 0 }
};


VOID show_about(struct DockWindow *dock)
{
    STRPTR msg;
    STRPTR p;
    STRPTR name;
    STRPTR version;
    STRPTR desc;
    STRPTR copy;
    ULONG msgLen, l;
    struct DgNode *curr;
    ClassID lastClass = 0, currClass;
    struct EasyStruct es = {
            sizeof(struct EasyStruct),
            0,
            "About " APP_NAME,
            APP_NAME " Version " APP_VERSION "\n\n"
            APP_DESCRIPTION "\n\n" APP_COPYRIGHT "\n\n%s",
            "OK"
    };

 
    msgLen = 0;
    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

        if( ! dock_gadget_builtin(curr->dg) ) {

            currClass = OCLASS(curr->dg)->cl_ID;
            if( currClass == lastClass ) {
                continue;
            }
            lastClass = currClass;

            dock_gadget_get_info(curr->dg, &name, &version, &desc, &copy);

            msgLen += strlen(name) + strlen(version) + strlen(desc) + strlen(copy) + 8;
        }
    } 

    if( msg = (STRPTR)DB_AllocMem(msgLen, MEMF_CLEAR) ) {

        p = msg;
        lastClass = 0;
    
        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            if( ! dock_gadget_builtin(curr->dg) ) {

                currClass = OCLASS(curr->dg)->cl_ID;
                if( currClass == lastClass ) {
                    continue;
                }
                lastClass = currClass;


                dock_gadget_get_info(curr->dg, &name, &version, &desc, &copy);

                l = strlen(name);
                CopyMem(name, p, l);
                p += l;
                *(p++) = ' ';
                l = strlen(version);
                CopyMem(version, p, l);
                p += l;
                *(p++) = '\n';
                l = strlen(desc);
                CopyMem(desc, p, l);
                p += l;
                *(p++) = '\n';
                l = strlen(copy);
                CopyMem(copy, p, l);
                p += l;
                *(p++) = '\n';
                *(p++) = '\n';
            }
        } 
        *p = '\0';
    
        EasyRequest(NULL, &es, NULL, msg);


        DB_FreeMem(msg, msgLen);
    }
}

VOID delete_port(struct MsgPort *port) {
    struct Message *msg;

    if( port ) {
        while( msg = GetMsg(port) ) {
            ReplyMsg(msg);
        }

        if( port->mp_Node.ln_Name ) {
            DeletePort(port);
        } else {
            DeleteMsgPort(port);
        }
    }
}


BOOL create_cx_broker(struct DockWindow *dock)
{
    struct NewBroker nb = {
        NB_VERSION,
        APP_NAME,
        APP_NAME,
        APP_DESCRIPTION,
        NBU_UNIQUE, COF_SHOW_HIDE, 0, 0, 0
    };

    if( dock->cxPort = CreateMsgPort() ) {

        nb.nb_Port = dock->cxPort;

        if( dock->cxBroker = CxBroker(&nb, NULL) ) {

            return TRUE;   
        }

    }

    return FALSE;
}

VOID free_cx_broker(struct DockWindow *dock) 
{
    if( dock->cxBroker ) {

        DeleteCxObjAll(dock->cxBroker);
        dock->cxBroker = NULL;
    }

    if( dock->cxPort ) {
        delete_port(dock->cxPort);
        dock->cxPort = NULL;
    }
}

BOOL init_cx_broker(struct DockWindow *dock)
{
    struct DgNode *curr;
    STRPTR hotKey;
    CxObj *filter, *sender, *translate;
    UWORD msgId;
    
    if( dock->cxBroker ) {
        free_cx_broker(dock);
    } 
    
    if( create_cx_broker(dock) ) {

        msgId = 0;

        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            dock_gadget_get_hotkey(curr->dg, &hotKey);
            if( hotKey ) {
                if( filter = CxFilter(hotKey) ) {
                                    
                    AttachCxObj(dock->cxBroker, filter);

                    if( sender = CxSender(dock->cxPort, msgId) ) {

                        AttachCxObj(filter, sender);

                        if( translate = CxTranslate(NULL) ) {

                            AttachCxObj(filter, translate);
    
                            if( CxObjError(filter) ) {
                                return FALSE;
                            }
                        } else {
                            return FALSE;
                        }
                    } else {
                        return FALSE;
                    }

                } else {
                    return FALSE;
                }
            }
            msgId++;
        }

        ActivateCxObj(dock->cxBroker, 1L);

        return TRUE;
    }

    return FALSE;
}

BOOL show_dock_window(struct DockWindow *dock)
{
	struct Screen *screen;
    APTR* vi;
    BOOL result = FALSE;
    struct DrawInfo *di;

	struct TagItem tags[] = {
		{ WA_Left, 0 },
		{ WA_Top, 0 },
		{ WA_Width, 1 },
		{ WA_Height, 1 },
		{ WA_Borderless, TRUE },
        { WA_SmartRefresh, TRUE },
        { WA_NewLookMenus, TRUE },
		{ WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW | IDCMP_MENUPICK },
		{ TAG_DONE, NULL }
	};


	if( screen = LockPubScreen(NULL) ) {

        if( di = GetScreenDrawInfo(screen) ) {

      		if( dock->win = OpenWindowTagList(NULL, tags) ) {
	
                if( vi = GetVisualInfo(dock->win->WScreen, TAG_END) ) {
        
                    if( dock->menu = CreateMenus(mainMenu, GTMN_FrontPen, di->dri_Pens[TEXTPEN], TAG_END) ) {           

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
            FreeScreenDrawInfo(screen, di);   
        }
        UnlockPubScreen(NULL, screen);
    }
    
    return result;     
}

VOID hide_dock_window(struct DockWindow *dock)
{
    if( dock->appWin ) {
        RemoveAppWindow(dock->appWin);
        dock->appWin = NULL;
    }

    delete_port(dock->awPort);
    dock->awPort = NULL;

    if( dock->hoverWin ) {
        CloseWindow(dock->hoverWin);
        dock->hoverWin = NULL;
    }
    
    if( dock->win ) {

        if( dock->menu ) {

            ClearMenuStrip(dock->win);
            FreeMenus(dock->menu);
            dock->menu = NULL;
        }

    	CloseWindow(dock->win);
        dock->win = NULL;
    }
}

struct DockWindow* create_dock(VOID)
{
	struct DockWindow *dock;

    if( dock = (struct DockWindow *)DB_AllocMem(sizeof(struct DockWindow), MEMF_CLEAR) ) {

        dock->runState = RS_STARTING;
        dock->disableLayout = TRUE;
        dock->cfg.align = DA_CENTER;
        dock->cfg.pos = DP_RIGHT;
        dock->cfg.showGadgetLabels = TRUE;

        if( dock->pubPort = CreatePort(APP_NAME, 0L) ) {

            if( init_gadget_classes(dock) ) {
                        
                if( init_gadgets(dock) ) {    
                
                    if( init_config_notification(dock) ) {
                        
                        if( init_timer_notification(dock) ) {

                            return dock;
                        }
                    }        
                }
            }
        }

        // Uh oh!    
        free_dock(dock);
    }

    return NULL;
}

VOID free_dock(struct DockWindow* dock)
{
    free_cx_broker(dock);

    hide_dock_window(dock);

    remove_dock_gadgets(dock);

    if( dock->notifyPort ) {

        EndNotify(&dock->notifyReq);
        delete_port(dock->notifyPort);
    }

    free_gadget_classes(dock);

    if( dock->timerPort ) {

        if( dock->timerReq ) {

            CloseDevice((struct IORequest *)dock->timerReq);    
            DeleteExtIO((struct IORequest *)dock->timerReq);    
        }    

        delete_port(dock->timerPort);
    }

    free_app_icon(dock);

    delete_port(dock->pubPort);

	DB_FreeMem(dock, sizeof(struct DockWindow));
}

