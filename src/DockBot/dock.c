
#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/wb_protos.h>
#include <clib/alib_protos.h>
#include <clib/layers_protos.h>
#include <clib/graphics_protos.h>

#include <stdio.h>
#include <string.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock.h"
#include "dock_gadget.h"
#include "dock_handle.h"
#include "dock_button.h"
#include "dock_settings.h"

#define CONFIG_FILE "ENV:DockBot.prefs"

struct DockWindow 
{
	DockPosition pos;
	DockAlign align;
	struct Window *win;
    struct AppWindow *appWin;
    struct MsgPort *awPort;
	struct MinList gadgets;
    Class *gadgetClass;
    Class *handleClass;
    Class *buttonClass;
    struct MsgPort* notifyPort;
    struct NotifyRequest notifyReq; 
    BOOL disableLayout;
};

#define DOCK_SIG(dw) (1 << dw->awPort->mp_SigBit)
#define WIN_SIG(dw) (1 << dw->win->UserPort->mp_SigBit)
#define NOTIFY_SIG(dw) (1 << dw->notifyPort->mp_SigBit)

#define S_ALIGN "align"
#define S_POSITION "position"
#define S_GADGET "gadget"

struct Values AlignValues[] = {
    { "left", DA_LEFT },
    { "center", DA_CENTER },
    { "right", DA_RIGHT },
    { NULL, 0 }
};

struct Values PositionValues[] = {
    { "left", DP_LEFT },
    { "right", DP_RIGHT },
    { "top", DP_TOP },
    { "bottom", DP_BOTTOM },
    { NULL, 0 }
};


struct DgNode
{
    struct MinNode n;
    Object *dg;
};


ULONG get_window_top(struct Screen* screen, DockPosition pos, DockAlign align, UWORD height)
{
	ULONG t;
	switch( pos ) 
	{
		case DP_LEFT:
		case DP_RIGHT:
			switch( align ) 
			{
				case DA_LEFT:
					t = screen->BarHeight + screen->BarVBorder;
					break;
				case DA_CENTER:
					t = (screen->Height - height) / 2;
					break;
				case DA_RIGHT:
					t = screen->Height - height;
					break;
			}
			break;
		case DP_TOP:
			t = screen->BarHeight + screen->BarVBorder;
			break;
		case DP_BOTTOM:
			t = screen->Height - height;
			break;
		
	}
	return t;
}

ULONG get_window_left(struct Screen* screen, DockPosition pos, DockAlign align, UWORD width)
{
	ULONG l;
	switch( pos )
	{
		case DP_TOP:
		case DP_BOTTOM:
			switch( align )
			{
				case DA_LEFT:
					l = 0;
					break;
				case DA_CENTER:
					l = (screen->Width + width) / 2;
					break;
				case DA_RIGHT:
					l = screen->Width - width;
					break;
			}
			break;
		case DP_LEFT:
			l = 0;
			break;
		case DP_RIGHT:
			l = screen->Width - width;
			break;
			
			
	}
	return l;
}

BOOL create_dock_gadget(struct DockWindow *dock, struct DockSettings *settings)
{
    struct DockSettingValue v;
    Object *gad;
    char gadName[50];

    while( ReadSetting(settings, &v) ) {
        if( IS_KEY(S_GADGET, v) ) {
            CopyMem(v.Value, &gadName, v.ValueLength);
            if( gad = NewObjectA(NULL, gadName, TAG_DONE ) ) {
                dock_gadget_read_settings(gad, settings);
                add_dock_gadget(dock, gad);

                return TRUE;                
            }
            break;
        }
    }
    return FALSE;
}


BOOL load_config(struct DockWindow *dock)
{
    struct DockSettings *s;
    struct DockSettingValue v;
    struct Values* vals;
    UWORD l;
    BOOL r;

    r = TRUE;
    if( s = OpenSettingsRead(CONFIG_FILE) ) {

        if( ReadBeginBlock(s) ) {

            while( TRUE ) {

                if( ReadBeginBlock(s) ) {
                    if( ! create_dock_gadget(dock, s) ) {
                        r = FALSE;
                        break;
                    }
                    if( !ReadEndBlock(s) ) {
                        r = FALSE;
                        break;
                    }
                }
                if( ReadEndBlock(s) ) {
                    break;
                }
                if( ReadSetting(s, &v) ) {
                    if( IS_KEY(S_ALIGN, v) ) {
                        GET_VALUE(v, AlignValues, vals, l, dock->align)
                    }

                    else if( IS_KEY(S_POSITION, v) ) {
                        GET_VALUE(v, PositionValues, vals, l, dock->pos)
                    }
                }
            }
        }
        CloseSettings(s);
    }
    return r;
    
}

BOOL create_dock_handle(struct DockWindow *dock)
{
    Object *gad;
    
    if( gad = NewObjectA(dock->handleClass, NULL, TAG_DONE) ) {

        add_dock_gadget(dock, gad);

        return TRUE;
    }

    return FALSE;
}

struct DockWindow* create_dock_window(VOID)
{
	struct Screen *screen;
	struct DockWindow *dock;
	UWORD x,y;

	struct TagItem tags[] = {
		{ WA_Left, 0 },
		{ WA_Top, 0 },
		{ WA_Width, 1 },
		{ WA_Height, 1 },
		{ WA_Borderless, TRUE },
        { WA_SmartRefresh, TRUE },
		{ WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW },
		{ TAG_DONE, NULL }
	};

	if( screen = LockPubScreen(NULL) ) {
		
		x = get_window_left(screen, DA_CENTER, DP_RIGHT, DEFAULT_SIZE);
		y = get_window_top(screen, DA_CENTER, DP_RIGHT, DEFAULT_SIZE);

		tags[0].ti_Data = x;
		tags[1].ti_Data = y;

        if( dock = (struct DockWindow *)AllocMem(sizeof(struct DockWindow), MEMF_CLEAR) ) {
		
    		if( dock->win = OpenWindowTagList(NULL, tags) ) {
	    		
                if( dock->awPort = CreateMsgPort() ) {

                    if( dock->appWin = AddAppWindow(1, 0, dock->win, dock->awPort, NULL) ) {
        
                        dock->disableLayout = TRUE;
                        dock->align = DA_CENTER;
                        dock->pos = DP_RIGHT;

                        if( dock->gadgetClass = init_dock_gadget_class() ) {

                            if( dock->handleClass = init_dock_handle_class() ) {

                                if( dock->buttonClass = init_dock_button_class() ) {
                        
                                    NewList((struct List *)&(dock->gadgets));
    
                                    if( create_dock_handle(dock) ) {
        
                                        if( load_config(dock) ) {
    
                                            if( dock->notifyPort = CreateMsgPort() ) {
                                                dock->notifyReq.nr_Name = CONFIG_FILE;
                                                dock->notifyReq.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
                                                dock->notifyReq.nr_stuff.nr_Msg.nr_Port = dock->notifyPort;
    
                                                if( StartNotify(&dock->notifyReq) ) {
        
                                                } else {
                                                    // Can't notify.
                                                }
                                            } else {
                                                // Can't create notify port.
                                                goto error;
                                            }
    
                                        } else {
                                            // Config is borked.
                                            goto error;
                                        }

                                    } else {
                                        // Can't create handle
                                        goto error;
                                    }
                                } else {
                                    // Can't create button class
                                    goto error;
                                }
                            } else {
    
                                // Can't create handle class
                                goto error;
                            }
                        } else {

                            // Can't create gadget root class
                            goto error;
                        }
    	            } else {
                        // Can't add AppWindow
                        goto error;
                    }
                } else {
                    // Can't create port
                    goto error;
                }

	    	} else {
		    	// Can't open window
                goto error;
    		}

		} else {
			// Can't allocate dock
		}
		UnlockPubScreen(NULL, screen);
	}
	else 
	{
		// Can't lock pub screen
		return NULL;
	}
	return dock;

error:
    printf("Failed to create window\n");
    if( dock ) {
        close_dock_window(dock);
    }

    if( screen ) {
        UnlockPubScreen(NULL, screen);
    }

    return NULL;
}

VOID remove_dock_gadgets(struct DockWindow *dock)
{
    struct DgNode *dg;

    disable_layout(dock);

    while( ! IsListEmpty((struct List *)&dock->gadgets) ) {
        if( dg = (struct DgNode *)RemTail((struct List *)&dock->gadgets) ) {
            DisposeObject(dg->dg);
            FreeMem(dg, sizeof(struct DgNode));
        }
    }
}

VOID close_dock_window(struct DockWindow* dock)
{
    struct Message *msg;

    remove_dock_gadgets(dock);

    if( dock->appWin ) {
        RemoveAppWindow(dock->appWin);
    }

    if( dock->win ) {
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
        free_dock_button_class(dock->buttonClass);
    }

    if( dock->handleClass ) {
        free_dock_handle_class(dock->handleClass);
    }

    if( dock->gadgetClass ) {
        free_dock_gadget_class(dock->gadgetClass);
    }
    
	FreeMem(dock, sizeof(struct DockWindow));
}

VOID handle_drop_event(struct DockWindow* dock)
{
    struct AppMessage *msg;
    struct WBArg *arg;
    int i;
    STRPTR buffer = NULL;

    while( msg = (struct AppMessage *)GetMsg(dock->awPort)) {
        printf("AppMsg: Type=%ld, ID=%ld, NumArgs=%ld, X = %d, Y = %d\n", 
                msg->am_Type, msg->am_ID, msg->am_NumArgs, msg->am_MouseX, msg->am_MouseY);
        arg = msg->am_ArgList;
        if( buffer == NULL ) {
            buffer = AllocMem(2048, MEMF_ANY);
        }
        for( i = 0; i < msg->am_NumArgs; i++ ) {
            NameFromLock(arg->wa_Lock, buffer, 2048);
            AddPart(buffer, arg->wa_Name, 2048);
            printf("  %ld: Name=\"%s\"\n",
                i, buffer);
            arg++;
        }        
        ReplyMsg((struct Message*)msg);
    }
    if( buffer != NULL ) {
        FreeMem(buffer, 2048);
    }
}


VOID draw_gadgets(struct DockWindow *dock)
{
    struct Window *win = dock->win;
    struct RastPort *rp = win->RPort;
    struct DgNode *curr;
/*    struct Region *origClipRegion;
    struct Region *newClipRegion;
    struct Rectangle r;

    r.MinX = win->BorderLeft;
    r.MinY = win->BorderTop;
    r.MaxX = win->Width - win->BorderRight - 1;
    r.MaxY = win->Height - win->BorderBottom - 1;
    
    LockLayerInfo(&win->WScreen->LayerInfo);

    newClipRegion = NewRegion();
    OrRectRegion(newClipRegion, &r);

    origClipRegion = InstallClipRegion(win->WLayer, newClipRegion);
*/
    BeginRefresh(win);

    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                curr->n.mln_Succ; 
                curr = (struct DgNode *)curr->n.mln_Succ ) {

        dock_gadget_draw(curr->dg, rp);
    }

    EndRefresh(win, TRUE);
/*
    InstallClipRegion(win->WLayer, origClipRegion);

    UnlockLayerInfo(&win->WScreen->LayerInfo);

    DisposeRegion(newClipRegion);
*/
}


BOOL handle_window_event(struct DockWindow *dock)
{
    struct IntuiMessage *msg;
    struct DgNode *curr;
    BOOL done, redraw;
    UWORD ix = 0;    

    done = FALSE;
    redraw = FALSE;
    while( msg = (struct IntuiMessage *)GetMsg(dock->win->UserPort) ) {
    
        switch( msg->Class )
        {
            case IDCMP_MOUSEBUTTONS: 
                if( msg->Code == SELECTUP ) {
                    for( curr = (struct DgNode *)dock->gadgets.mlh_Head;
                         curr->n.mln_Succ;
                         curr = (struct DgNode *)curr->n.mln_Succ ) {
                        
                        if( dock_gadget_hit_test(curr->dg, msg->MouseX, msg->MouseY) ) {
                
                            dock_gadget_click(curr->dg, msg->MouseX, msg->MouseY);
                            if( ix == 0 ) {
                                done = TRUE;
                            }
            
                        }
                        ix++;
                    }
                }
                break;

            case IDCMP_CHANGEWINDOW:
            case IDCMP_REFRESHWINDOW:
                redraw = TRUE;
                break;

        }
        
        ReplyMsg((struct Message *)msg);

    }
    if( redraw ) {
        draw_gadgets(dock);
    }
    return done;
}

VOID handle_notify_message(struct DockWindow *dock)
{
    struct Message *msg;

    while( msg = GetMsg(dock->notifyPort) ) {
        ReplyMsg(msg);
    }

    remove_dock_gadgets(dock);

    create_dock_handle(dock);

    load_config(dock);

    enable_layout(dock);
}

VOID run_event_loop(struct DockWindow *dock)
{
    BOOL done = FALSE;
    ULONG signals, winsig, docksig, notifysig;
    
    winsig = WIN_SIG(dock);
    docksig = DOCK_SIG(dock);
    notifysig = NOTIFY_SIG(dock);

    while( !done ) {

        signals = Wait( winsig | docksig | notifysig );

        if( signals & winsig ) {
            
            done = handle_window_event(dock);          
        }

        if( signals & docksig ) {

            handle_drop_event(dock);
        }

        if( signals & notifysig ) {

            handle_notify_message(dock);
        }
    }
}

VOID layout_gadgets(struct DockWindow *dock)
{
    UWORD w, h, x, y, i, max;
    struct DgNode *curr;
    struct Screen *screen;   
    struct Rect b;
    UWORD* sizes;
    UWORD gadgetCount;

    if( dock->disableLayout ) {
        return;
    }

    if( screen = LockPubScreen(NULL)) {

        y = 0;
        x = 0;
              
        for( curr = (struct DgNode *)dock->gadgets.mlh_Head, gadgetCount = 0; 
                    curr->n.mln_Succ; 
                    curr = (struct DgNode *)curr->n.mln_Succ ) {
            gadgetCount++;
        }

        if( sizes = (UWORD*)AllocMem(sizeof(UWORD) * gadgetCount, MEMF_CLEAR) ) {

            max = 0;

            if( dock->pos == DP_TOP || dock->pos == DP_BOTTOM ) {
    
                for( curr = (struct DgNode *)dock->gadgets.mlh_Head, i = 0; 
                            curr->n.mln_Succ; 
                            curr = (struct DgNode *)curr->n.mln_Succ, i++ ) {

                    dock_gadget_get_size(curr->dg, dock->pos, dock->align, &w, &h);
                                       
                    max = max < h ? h : max;

                    sizes[i] = w;
                }
             
                for( curr = (struct DgNode *)dock->gadgets.mlh_Head, i = 0; 
                            curr->n.mln_Succ; 
                            curr = (struct DgNode *)curr->n.mln_Succ, i++ ) {
                
                    b.x = x;
                    b.y = y;
                    b.w = sizes[i];
                    b.h = max;
                    dock_gadget_set_bounds(curr->dg, &b);

                    x += sizes[i];
                }                

                ChangeWindowBox(dock->win, 0,0,1,1);
                ChangeWindowBox(dock->win, 
                    get_window_left(screen, dock->pos, dock->align, x),
                    get_window_top(screen, dock->pos, dock->align, max),
                    x, max);
            
            } else {
    
                for( curr = (struct DgNode *)dock->gadgets.mlh_Head, i = 0; 
                            curr->n.mln_Succ; 
                            curr = (struct DgNode *)curr->n.mln_Succ, i++ ) {

                    dock_gadget_get_size(curr->dg, dock->pos, dock->align, &w, &h);
                                       
                    max = max < w ? w : max;

                    sizes[i] = h;
                }
             
                for( curr = (struct DgNode *)dock->gadgets.mlh_Head, i = 0; 
                            curr->n.mln_Succ; 
                            curr = (struct DgNode *)curr->n.mln_Succ, i++ ) {
                
                    b.x = x;
                    b.y = y;
                    b.w = max;
                    b.h = sizes[i];
                    dock_gadget_set_bounds(curr->dg, &b);

                    y += sizes[i];
                }                

                ChangeWindowBox(dock->win, 0,0,1,1);
                ChangeWindowBox(dock->win, 
                    get_window_left(screen, dock->pos, dock->align, max),
                    get_window_top(screen, dock->pos, dock->align, y),
                    max, y);
     
            }

            FreeMem(sizes, sizeof(UWORD) * gadgetCount);
        }

        UnlockPubScreen(NULL, screen);
    }
}

VOID add_dock_gadget(struct DockWindow *dock, Object *dg)
{
    struct DgNode *n;
    if( n = AllocMem(sizeof(struct DgNode), MEMF_CLEAR) ) {
        n->dg = dg;
        AddTail((struct List *)&(dock->gadgets), (struct Node *)n);
    }

    layout_gadgets(dock);
}

VOID remove_dock_gadget(struct DockWindow *dock, Object *dg)
{
    struct DgNode *curr;
    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                curr->n.mln_Succ; 
                curr = (struct DgNode *)curr->n.mln_Succ ) {
        if( curr->dg == dg ) {

            Remove((struct Node *)curr);
            FreeMem(curr, sizeof(struct DgNode));
            break;
        }     
    } 

    layout_gadgets(dock);
}


VOID disable_layout(struct DockWindow *dock)
{
    dock->disableLayout = TRUE;
}

VOID enable_layout(struct DockWindow *dock)
{
    dock->disableLayout = FALSE;

    layout_gadgets(dock);
}
