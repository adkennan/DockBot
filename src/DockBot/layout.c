/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <exec/memory.h>
#include <intuition/screens.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>


UWORD get_max_window_size(struct Screen *screen, DockPosition pos)
{
    switch( pos ) {
        case DP_LEFT:
        case DP_RIGHT:
            return (UWORD)(screen->Height - screen->BarHeight - screen->BarVBorder);

        case DP_TOP:
        case DP_BOTTOM:
            return (UWORD)screen->Width;        
    }
    return 0;
}


VOID layout_gadgets(struct DockWindow *dock)
{
    UWORD w, h, x, y, i, max, size = 0, maxSize;
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
              
        maxSize = get_max_window_size(screen, dock->cfg.pos);

        gadgetCount = 0;    
        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            gadgetCount++;
        }

        if( sizes = (UWORD*)DB_AllocMem(sizeof(UWORD) * gadgetCount, MEMF_CLEAR) ) {

            max = 0;

            if( dock->cfg.pos == DP_TOP || dock->cfg.pos == DP_BOTTOM ) {

                i = 0;
                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

                    dock_gadget_get_size(curr->dg, dock->cfg.pos, dock->cfg.align, &w, &h);

                    if( size + w > maxSize ) {
                        break;
                    }
                                       
                    max = max < h ? h : max;
                    size += w;
                    sizes[i] = w;
                    i++;
                }
             
                i = 0;
                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
                
                    b.x = x;
                    b.y = y;
                    b.w = sizes[i];
                    b.h = max;
                    dock_gadget_set_bounds(curr->dg, &b);
                    
                    x += sizes[i];
                    i++;
                }                

                ChangeWindowBox(dock->win, 
                    get_window_left(screen, dock->cfg.pos, dock->cfg.align, x),
                    get_window_top(screen, dock->cfg.pos, dock->cfg.align, max),
                    x, max);
            
            } else {
    
                i = 0;
                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

                    dock_gadget_get_size(curr->dg, dock->cfg.pos, dock->cfg.align, &w, &h);

                    if( size + h > maxSize ) {
                        break;
                    }                                       

                    max = max < w ? w : max;
                    size += h;
                    sizes[i] = h;
                    i++;
                }
                
                i = 0;             
                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

                    b.x = x;
                    b.y = y;
                    b.w = max;
                    b.h = sizes[i];
                    dock_gadget_set_bounds(curr->dg, &b);

                    y += sizes[i];
                    i++;
                }                

                ChangeWindowBox(dock->win, 
                    get_window_left(screen, dock->cfg.pos, dock->cfg.align, max),
                    get_window_top(screen, dock->cfg.pos, dock->cfg.align, y),
                    max, y);
     
            }

            DB_FreeMem(sizes, sizeof(UWORD) * gadgetCount);
        }

        UnlockPubScreen(NULL, screen);
    }

    dock->runState = RS_RUNNING;
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
					l = (screen->Width - width) / 2;
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

