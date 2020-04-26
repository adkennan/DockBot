/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/wb_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/layers_protos.h>

#include "dock_handle.h"

enum {
    MENU_IDX_Project   =  0,
    MENU_IDX_About     =  1,
    MENU_IDX_Help      =  2,
    MENU_IDX_Iconify   =  4,
    MENU_IDX_Quit      =  6,
    MENU_IDX_Settings  =  7,
    MENU_IDX_Edit      =  8,
    MENU_IDX_Save      =  9,
    MENU_IDX_Revert    = 10,
    MENU_IDX_OpenPrefs = 11
};

struct NewMenu mainMenu[] = {

    { NM_TITLE, 0 /*MSG_MENU_Project*/,     0, 0, 0, 0 },
    {  NM_ITEM, 0 /*MSG_MENU_About*/,       0, 0, 0, (APTR)MI_ABOUT },
    {  NM_ITEM, 0 /*MSG_MENU_Help*/,        0, 0, 0, (APTR)MI_HELP },
    {  NM_ITEM, NM_BARLABEL,                0, 0, 0, 0 }, 
    {  NM_ITEM, 0 /*MSG_MENU_Iconify*/,     0, 0, 0, (APTR)MI_HIDE },
    {  NM_ITEM, NM_BARLABEL,                0, 0, 0, 0 }, 
    {  NM_ITEM, 0 /*MSG_MENU_Quit*/,        0, 0, 0, (APTR)MI_QUIT },

    { NM_TITLE, 0 /*MSG_MENU_Settings*/,    0, 0, 0, 0 },
    {  NM_ITEM, 0 /*MSG_MENU_Edit*/,        0, CHECKIT|MENUTOGGLE, 0, (APTR)MI_EDIT },
    {  NM_ITEM, 0 /*MSG_MENU_SaveSettings*/,0, 0, 0, (APTR)MI_SAVE },
    {  NM_ITEM, 0 /*MSG_MENU_RevertSettings*/, 0, 0, 0, (APTR)MI_REVERT },
    {  NM_ITEM, 0 /*MSG_MENU_OpenPrefs*/,   0, 0, 0, (APTR)MI_OPEN_PREFS }, 

    { NM_END,   0,                       0, 0, 0, 0 }
};


BOOL build_menu(struct DockWindow *dock) {

    BOOL result = FALSE;
	struct Screen *screen;
    struct DrawInfo *di;
    APTR* vi;

    DEBUG(printf("build_menu\n"));

    mainMenu[MENU_IDX_Project].nm_Label    = (STRPTR)MSG_MENU_Project;
    mainMenu[MENU_IDX_About].nm_Label      = (STRPTR)MSG_MENU_About;
    mainMenu[MENU_IDX_About].nm_CommKey    = (STRPTR)MSG_KEY_About;
    mainMenu[MENU_IDX_Help].nm_Label       = (STRPTR)MSG_MENU_Help;
    mainMenu[MENU_IDX_Iconify].nm_Label    = (STRPTR)MSG_MENU_Iconify;
    mainMenu[MENU_IDX_Iconify].nm_CommKey  = (STRPTR)MSG_KEY_Iconify;
    mainMenu[MENU_IDX_Quit].nm_Label       = (STRPTR)MSG_MENU_Quit;
    mainMenu[MENU_IDX_Quit].nm_CommKey     = (STRPTR)MSG_KEY_Quit;

    mainMenu[MENU_IDX_Settings].nm_Label   = (STRPTR)MSG_MENU_Settings;
    mainMenu[MENU_IDX_Edit].nm_Label   = (STRPTR)MSG_MENU_EditMode;
    mainMenu[MENU_IDX_Edit].nm_CommKey = (STRPTR)MSG_KEY_EditMode;
    mainMenu[MENU_IDX_Save].nm_Label   = (STRPTR)MSG_MENU_Save;
    mainMenu[MENU_IDX_Save].nm_CommKey = (STRPTR)MSG_KEY_Save;
    mainMenu[MENU_IDX_Revert].nm_Label =    (STRPTR)MSG_MENU_Revert;
    mainMenu[MENU_IDX_Revert].nm_CommKey =  (STRPTR)MSG_KEY_Revert;
    mainMenu[MENU_IDX_OpenPrefs].nm_Label   = (STRPTR)MSG_MENU_OpenPrefs;
    mainMenu[MENU_IDX_OpenPrefs].nm_CommKey = (STRPTR)MSG_KEY_OpenPrefs;

	if( screen = LockPubScreen(NULL) ) {

        if( di = GetScreenDrawInfo(screen) ) {

            if( vi = GetVisualInfo(dock->win->WScreen, TAG_END) ) {
    
                if( dock->menu = CreateMenus(mainMenu, GTMN_FrontPen, di->dri_Pens[TEXTPEN], TAG_END) ) {           

                    if( LayoutMenus(dock->menu, vi, TAG_END) ) {

                        if( SetMenuStrip(dock->win, dock->menu) ) {

                            result = TRUE;
                        }
                    }
                }
                FreeVisualInfo(vi);  
            }
            FreeScreenDrawInfo(screen, di);   
        }
        UnlockPubScreen(NULL, screen);        
    }

    if( ! result ) {
        DEBUG(printf(" Failed\n"));
    }

    return result;
}

struct MenuItem *find_menu_item(struct Menu *menu, MenuIndex ix)
{
    struct MenuItem *item = menu->FirstItem; 

    while( item ) {
        if( (MenuIndex)GTMENUITEM_USERDATA(item) == ix ) {
            return item;
        }
    
        item = item->NextItem;
    }

    if( menu->NextMenu ) {
        return find_menu_item(menu->NextMenu, ix);
    }

    return NULL;
}

VOID update_settings_menu(struct DockWindow *dock)
{
    if( dock->runState == RS_EDITING ) {
    
        find_menu_item(dock->menu, MI_EDIT)->Flags |= CHECKED;
        find_menu_item(dock->menu, MI_SAVE)->Flags |= ITEMENABLED;
        find_menu_item(dock->menu, MI_REVERT)->Flags |= ITEMENABLED;

    } else {
    
        find_menu_item(dock->menu, MI_EDIT)->Flags &= ~CHECKED;
        find_menu_item(dock->menu, MI_SAVE)->Flags &= ~ITEMENABLED;
        find_menu_item(dock->menu, MI_REVERT)->Flags &= ~ITEMENABLED;

    }

    ResetMenuStrip(dock->win, dock->menu);
}

BOOL show_dock_window(struct DockWindow *dock)
{
    BOOL result = FALSE;

	struct TagItem tags[] = {
		{ WA_Left, 0 },
		{ WA_Top, 0 },
		{ WA_Width, 1 },
		{ WA_Height, 1 },
		{ WA_Borderless, TRUE },
        { WA_SimpleRefresh, TRUE },
        { WA_NewLookMenus, TRUE },
		{ WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW | IDCMP_MENUPICK },
		{ TAG_DONE, NULL }
	};

    DEBUG(printf("show_dock_window\n"));

	if( dock->win = OpenWindowTagList(NULL, tags) ) {
	
        if( build_menu(dock) ) {

            if( dock->awPort = CreateMsgPort() ) {

                if( dock->appWin = AddAppWindow(1, 0, dock->win, dock->awPort, NULL) ) {
                    
                    result = TRUE;
                                    
                } else {
                    DEBUG(printf(" AddAppWindow Failed\n"));
                }
            
            } else {
                DEBUG(printf(" CreateMsgPort Failed\n"));
            }
        }
    } else {
       DEBUG(printf(" OpenWindow Failed\n"));
    }
    return result;     
}

VOID hide_dock_window(struct DockWindow *dock)
{
    DEBUG(printf("hide_dock_window\n"));

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

VOID launch_dropped_icon(struct DockWindow *dock, struct AppMessage *msg)
{
    struct WBArg *arg;
    UWORD i, len;
    STRPTR tmpBuf = NULL;
    STRPTR* buffers;
    struct DgNode *dg;
    Object *gadget;

    if( dg = get_gadget_at(dock, msg->am_MouseX, msg->am_MouseY) ) {
        gadget = dg->dg;
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

    if( tmpBuf != NULL ) {
        DB_FreeMem(tmpBuf, 2048);
    }
}

VOID handle_dropped_icon(struct DockWindow *dock, struct AppMessage *msg)
{
    add_dropped_icon(dock, msg->am_ArgList->wa_Lock, msg->am_ArgList->wa_Name);
}

VOID handle_drop_event(struct DockWindow* dock)
{
    struct AppMessage *msg;

    while( msg = (struct AppMessage *)GetMsg(dock->awPort)) {
        switch( msg->am_Type ) {
          case AMTYPE_APPWINDOW:

            if( msg->am_NumArgs > 0 ) {

                if( dock->runState == RS_RUNNING ) {
        
                    launch_dropped_icon(dock, msg);
        
                } else if( dock->runState == RS_EDITING ) {

                    handle_dropped_icon(dock, msg);
                }
            }
            break;

          case AMTYPE_APPICON:
            dock->runState = RS_UNICONIFYING;
            break;
        }

        ReplyMsg((struct Message*)msg);
    }
}

VOID handle_edit_click(struct DockWindow *dock, struct DgNode *dg, UWORD mouseX, UWORD mouseY)
{
    struct GadgetEnvironment env;
    UWORD x, y, cx, cy, op;

    DB_GetDockGadgetEnvironment(dg->dg, &env);
    
    if( env.index == 0 ) {
        return;
    }

    x = mouseX - env.gadgetBounds.x;
    y = mouseY - env.gadgetBounds.y;
    
    cx = env.gadgetBounds.w / 2;
    cy = env.gadgetBounds.h / 2;

    op = EO_NONE;

    if( y < cy ) {
        if( x < cx ) {
            if( env.index > 1 ) {
                op = EO_MOVE_UP;
            }    
        } else if( DOCK_HORIZONTAL(dock) ) {
            if( ! env.isLast ) {
                op = EO_MOVE_DOWN;  
            }   
        } else {
            op = EO_DELETE;
        }             
    } else if( x < cx ) {
     
       if( DOCK_HORIZONTAL(dock) ) {
            op = EO_DELETE;
   
        } else {
            if( ! env.isLast ) {
                op = EO_MOVE_DOWN;
            }
        }
    }

    if( op != EO_NONE ) {

        dock->runState = RS_CHANGING;
        dock->editOp = op;
        dock->editNode = dg;
        dock->editCount = 2;
    }    
}


VOID handle_window_event(struct DockWindow *dock)
{
    struct IntuiMessage *msg;
    struct DgNode *dg;
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
                if( msgCode == SELECTDOWN ) {
                    if( dg = get_gadget_at(dock, mouseX, mouseY) ) {
      
                        if( dock->runState == RS_RUNNING ) {

                            dock_gadget_click(dg->dg, mouseX, mouseY);

                        } else {

                            handle_edit_click(dock, dg, mouseX, mouseY);
                        }
                    }
                }
                break;
    
            case IDCMP_CHANGEWINDOW:
            case IDCMP_REFRESHWINDOW:
                update_entire_window(dock);
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
                            dock->runState = RS_ICONIFYING;
                            break;

                       case MI_OPEN_PREFS:
                            open_settings(dock);
                            break;

                       case MI_HELP:
                            open_help(dock);
                            break;

                       case MI_EDIT:
                            if( dock->runState == RS_EDITING ) {
                                dock->runState = RS_STOP_EDIT;
                            } else {
                                dock->runState = RS_START_EDIT;
                            }
                            break;
            
                       case MI_SAVE:
                            save_config(dock);
                            dock->runState = RS_STOP_EDIT;
                            break;

                       case MI_REVERT:
                            dock->runState = RS_LOADING;
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
