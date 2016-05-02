/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include "dock.h"

#include <exec/memory.h>
#include <intuition/screens.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include "dock_gadget.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

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

    dock->runState = RS_LAYOUT;

    if( screen = LockPubScreen(NULL)) {

        y = 0;
        x = 0;
              
        for( curr = (struct DgNode *)dock->gadgets.mlh_Head, gadgetCount = 0; 
                    curr->n.mln_Succ; 
                    curr = (struct DgNode *)curr->n.mln_Succ ) {
            gadgetCount++;
        }

        if( sizes = (UWORD*)DB_AllocMem(sizeof(UWORD) * gadgetCount, MEMF_CLEAR) ) {

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

                ChangeWindowBox(dock->win, 
                    get_window_left(screen, dock->pos, dock->align, max),
                    get_window_top(screen, dock->pos, dock->align, y),
                    max, y);
     
            }

            DB_FreeMem(sizes, sizeof(UWORD) * gadgetCount);
        }

        UnlockPubScreen(NULL, screen);
    }

    dock->runState = RS_RUNNING;
/*
    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
         curr->n.mln_Succ; 
         curr = (struct DgNode *)curr->n.mln_Succ ) {

        RequestDockGadgetDraw(curr->dg);
    }
*/
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


ULONG get_window_top(struct Screen* screen, DockPosition pos, DockAlign align, UWORD height)
{
	ULONG t = 0;
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
	ULONG l = 0;
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

