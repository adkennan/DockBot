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
#include <clib/graphics_protos.h>
#include <clib/layers_protos.h>

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

VOID free_render_bitmap(struct DockWindow *dock)
{
    struct Region *r;

    dock->renderRP.BitMap = NULL;
    dock->renderRP.Layer = NULL;
    dock->renderW = 0;
    dock->renderH = 0;

    if( dock->renderBM ) {
        FreeBitMap(dock->renderBM);
        dock->renderBM = NULL;
        DEBUG(printf(__FUNC__ ": FreeBitMap\n"));
    }

    if( dock->renderL ) {
        if( r = InstallClipRegion(dock->renderL, NULL) ) {
            DisposeRegion(r);
            DEBUG(printf(__FUNC__ ": DisposeRegion\n"));
        }

        dock->renderL->rp->BitMap = NULL;

        DeleteLayer(0L, dock->renderL);
        dock->renderL = NULL;
        DEBUG(printf(__FUNC__ ": DeleteLayer\n"));
    }

    if( dock->renderLI ) {
        DisposeLayerInfo(dock->renderLI);
        dock->renderLI = NULL;
        DEBUG(printf(__FUNC__ ": DisposeLayerInfo\n"));
    }   
}

VOID update_render_bitmap(struct DockWindow *dock, UWORD w, UWORD h)
{
    struct Screen *screen;   
    struct Region *region;
    struct Rectangle r;
    ULONG d, il;

    if( w == dock->renderW && h == dock->renderH ) {
        return;
    }

    DEBUG(printf(__FUNC__ ": New window size: %d, %d\n", w, h));

    free_render_bitmap(dock);

    if( screen = LockPubScreen(NULL) ) {

        d = GetBitMapAttr(screen->RastPort.BitMap, BMA_DEPTH);
        il = GetBitMapAttr(screen->RastPort.BitMap, BMA_FLAGS) & BMF_INTERLEAVED;

        if( dock->renderBM = AllocBitMap(
                                w, h, 
                                d, 
                                BMF_DISPLAYABLE | il, 
                                NULL) ) {
  
            dock->renderW = w;
            dock->renderH = h;
  
            if( dock->renderLI = NewLayerInfo() ) {
                
                if( dock->renderL = CreateUpfrontLayer(
                                      dock->renderLI,
                                      dock->renderBM, 
                                      0, 0, 
                                      w - 1, h - 1,
                                      LAYERSIMPLE,
                                      NULL) ) {

                    dock->renderRP.BitMap = dock->renderBM;
                    dock->renderRP.Layer = dock->renderL;
    
                    if( region = NewRegion() ) {

                        r.MinX = 0;
                        r.MaxX = w - 1;
                        r.MinY = 0;
                        r.MaxY = h - 1;

                        OrRectRegion(region, &r);

                        InstallClipRegion(dock->renderL, region);
                    }
                }
            }
        }

        UnlockPubScreen(NULL, screen);
    }
}

VOID layout_gadgets(struct DockWindow *dock)
{
    UWORD w, h, x, y, i, max, size, maxSize, wx, wy;
    struct DgNode *curr;
    struct Screen *screen;   
    UWORD* sizes;
    UWORD gadgetCount;
    struct GadgetEnvironment env;

    DEBUG(printf(__FUNC__ "\n"));

    if( dock->disableLayout ) {
        return;
    }

    env.align = dock->cfg.align;
    env.pos = dock->cfg.pos;
    env.showBorders = dock->cfg.showGadgetBorders;

    if( screen = LockPubScreen(NULL)) {

        if( ! dock->cfg.showGadgetBorders ) {
            size = 1;   
            x = 1;
            y = 1;                    
        } else {
            size = 0;
            y = 0;
            x = 0;
        }      
        maxSize = get_max_window_size(screen, dock->cfg.pos);

        gadgetCount = 0;    
        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            gadgetCount++;
        }

        if( sizes = (UWORD*)DB_AllocMem(sizeof(UWORD) * gadgetCount, MEMF_CLEAR) ) {

            max = 0;

            if( DOCK_HORIZONTAL(dock) ) {

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
             
                wx = get_window_left(screen, dock->cfg.pos, dock->cfg.align, size);
                wy = get_window_top(screen, dock->cfg.pos, dock->cfg.align, max);

                i = 0;

                y = max;
                env.gadgetBounds.y = 0;

                if( ! dock->cfg.showGadgetBorders ) {
                    y += 1;
                    env.gadgetBounds.y = 1;
                }
                
                env.windowBounds.x = wx;
                env.windowBounds.y = wy;
                env.windowBounds.w = x;
                env.windowBounds.h = y;

                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
                
                    env.index = i;
                    env.isLast = i == gadgetCount - 1;
                    env.gadgetBounds.x = x;
                    env.gadgetBounds.w = sizes[i];
                    env.gadgetBounds.h = max;

                    dock_gadget_set_environment(curr->dg, &env);
                    
                    x += sizes[i];
                    i++;
                }                

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

                wx = get_window_left(screen, dock->cfg.pos, dock->cfg.align, max);
                wy = get_window_top(screen, dock->cfg.pos, dock->cfg.align, size);
                
                i = 0;             

                x = max;
                env.gadgetBounds.x = 0;

                if( ! dock->cfg.showGadgetBorders ) {
                    x += 1;
                    env.gadgetBounds.x = 1;
                }
                
                env.windowBounds.x = wx;
                env.windowBounds.y = wy;
                env.windowBounds.w = x;
                env.windowBounds.h = y;

                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
                
                    env.index = i;
                    env.isLast = i == gadgetCount - 1;
                    env.gadgetBounds.y = y;
                    env.gadgetBounds.w = max;
                    env.gadgetBounds.h = sizes[i];

                    dock_gadget_set_environment(curr->dg, &env);

                    y += sizes[i];
                    i++;
                }                

            }

            ChangeWindowBox(dock->win, wx, wy, x, y);

            update_render_bitmap(dock, x, y);
            
            DB_FreeMem(sizes, sizeof(UWORD) * gadgetCount);
        }

        UnlockPubScreen(NULL, screen);
    }
}

VOID disable_layout(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));
    dock->disableLayout = TRUE;
}

VOID enable_layout(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

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

