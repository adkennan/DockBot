/************************************
**
**  DockBot - A Dock For AmigaOS
**
**  © 2019 Andrew Kennan    
**
************************************/

#include "dock.h"

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/layers_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfxmacros.h>

#include "dock_handle.h"

BOOL create_dock_handle(struct DockWindow *dock)
{
    struct DgNode *dg;

    DEBUG(printf(__FUNC__ "\n"));

    if( dg = DB_AllocGadget(HANDLE_CLASS_NAME) ) {

        AddTail(&dock->cfg.gadgets, (struct Node *)dg);

        DEBUG(printf("  Created handle.\n"));

        return TRUE;
    }

    DEBUG(printf(__FUNC__ ": Failed to create handle.\n"));

    return FALSE;
}

BOOL init_gadget_classes(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

    if( dock->gadgetPort = CreateMsgPort() ) {
        if( dock->handleClass = init_dock_handle_class() ) {
            return TRUE;
        }   
    }

    DEBUG(printf(__FUNC__ ": Failed\n"));
    return FALSE;
}

BOOL free_gadget_classes(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

    if( dock->handleClass ) {
        
        if( ! free_dock_handle_class(dock->handleClass) ) {
            return FALSE;
        }
        dock->handleClass = NULL;
    }

    delete_port(dock->gadgetPort);
    dock->gadgetPort = NULL;        

    return TRUE;
}

BOOL init_gadgets(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

    NewList(&dock->cfg.gadgets);

    return create_dock_handle(dock);
}


VOID remove_dock_gadgets(struct DockWindow *dock)
{
    struct DgNode *dg;

    DEBUG(printf(__FUNC__ "\n"));

    disable_layout(dock);

    while( !IsListEmpty(&dock->cfg.gadgets) ) {

        if( dg = (struct DgNode *)RemTail(&dock->cfg.gadgets) ) {

            DB_FreeGadget(dg);
        }
    }
}

VOID fill_background(struct DockWindow *dock, struct RastPort *rp, struct Rect *b)
{
    UWORD x, y, bx, by, bw, bh, stepX, stepY, ex, ey;

    DEBUG(printf(__FUNC__ ": (%ld, %ld) -> (%ld, %ld)\n", (LONG)b->x, (LONG)b->y, (LONG)b->w, (LONG)b->h));

    if( ! dock->bgBrush ) {

        SetAPen(rp, 0);
        RectFill(rp, b->x, b->y, b->x + b->w - 1, b->y + b->h - 1);

        return;
    }

    DB_GetBrushSize(dock->bgBrush, &stepX, &stepY);

    x = b->x;
    y = b->y;
    
    bx = 0;
    if( x > 0 ) {
        bx = x % stepX;
    }
    
    by = 0;
    if( y > 0 ) {
        by = y % stepY;
    }    

    ex = b->x + b->w - 1;
    if( ex >= dock->win->Width ) {
        ex = dock->win->Width - 1;
    }

    ey = b->y + b->h - 1;
    if( ey >= dock->win->Height ) {
        ey = dock->win->Height - 1;
    }
    
    if( y + stepY >= ey ) {
        bh = ey - y;
    } else if( by > 0 ) {
        bh = stepY - by;
    } else {
        bh = stepY;
    }

    DEBUG(printf(__FUNC__ ": y = %ld, stepY = %ld, ey = %ld, bh = %ld\n",
       (LONG)y, (LONG)stepY, (LONG)ey, (LONG)bh));

    while( y < ey ) {

        if( x + stepX >= ex ) {
            bw = ex - x;
        } else if( bx > 0 ) {
            bw = stepX - bx;
        } else {
            bw = stepX;
        }
    
        DB_DrawBrush(dock->bgBrush, rp, bx, by, x, y, bw, bh);

        bx = 0;
        x += bw;
        if( x >= ex ) {
            x = b->x;
            if( x > 0 ) {
                bx = x % stepX;
            }

            y += bh;    
            by = 0;

            if( y + stepY >= ey ) {
                bh = ey - y;
            } else if( by > 0 ) {
                bh = stepY - by;
            } else {
                bh = stepY;
            }

            DEBUG(printf(__FUNC__ ": y = %ld, stepY = %ld, ey = %ld, bh = %ld\n",
                (LONG)y, (LONG)stepY, (LONG)ey, (LONG)bh));
        }
    }    
}

VOID draw_edit_controls(struct DockWindow *dock, struct Screen *screen, struct RastPort *rp, Object *gadget)
{
    struct DrawInfo *drawInfo;
    struct GadgetEnvironment env;
    ULONG pen;
    USHORT pat[] = { 0x5555, 0xAAAA };
    UWORD i1, i2, i3;
   
    DB_GetDockGadgetEnvironment(gadget, &env);

    if( OCLASS(gadget) == dock->handleClass ) {
        return;
    }

    if( drawInfo = GetScreenDrawInfo(screen) ) {

        pen = ObtainBestPenA(screen->ViewPort.ColorMap, 255, 255, 255, NULL);

        SetDrMd(rp, JAM1);
        SetAPen(rp, drawInfo->dri_Pens[BACKGROUNDPEN]);
        SetBPen(rp, pen);
        SetAfPt(rp, pat, 1);    
    
        RectFill(rp, env.gadgetBounds.x, env.gadgetBounds.y,
                     env.gadgetBounds.x + env.gadgetBounds.w - 1, 
                     env.gadgetBounds.y + env.gadgetBounds.h - 1);

        SetAfPt(rp, NULL, 0);
        ReleasePen(screen->ViewPort.ColorMap, pen);

        FreeScreenDrawInfo(screen, drawInfo);
    }

    if( DOCK_HORIZONTAL(dock) ) {

        i1 = II_ARROW_LEFT;
        i2 = II_ARROW_RIGHT;
        i3 = II_DELETE;

        if( dock->runState == RS_CHANGING && dock->editCount & 1 ) {
            switch( dock->editOp ) {
                case EO_MOVE_UP:
                    i1 = II_ARROW_LEFT_INV;
                    break;

                case EO_MOVE_DOWN:
                    i2 = II_ARROW_RIGHT_INV;
                    break;

                case EO_DELETE:
                    i3 = II_DELETE_INV;
                    break;
            }
        }

        if( env.isLast ) {
            i2 = II_NONE;
        }

    } else {

        i1 = II_ARROW_UP;
        i2 = II_DELETE;
        i3 = II_ARROW_DOWN;

        if( dock->runState == RS_CHANGING && dock->editCount & 1 ) {
            switch( dock->editOp ) {
                case EO_MOVE_UP:
                    i1 = II_ARROW_UP_INV;
                    break;

                case EO_MOVE_DOWN:
                    i3 = II_ARROW_DOWN_INV;
                    break;

                case EO_DELETE:
                    i2 = II_DELETE_INV;
                    break;

            }
        }

        if( env.isLast ) {
            i3 = II_NONE;
        }
    }

    if( env.index <= 1 ) {
        i1 = II_NONE;
    }


    if( i1 != II_NONE ) {
        DB_DrawBrush(dock->iconBrush, rp, i1 * dock->iconW, 0,
            env.gadgetBounds.x, env.gadgetBounds.y, dock->iconW, dock->iconH);
    }
    
    if( i2 != II_NONE ) {
        DB_DrawBrush(dock->iconBrush, rp, i2 * dock->iconW, 0,
                env.gadgetBounds.x + env.gadgetBounds.w - 1 - dock->iconW,
                env.gadgetBounds.y, dock->iconW, dock->iconH);
    }

    if( i3 != II_NONE ) {
        DB_DrawBrush(dock->iconBrush, rp, i3 * dock->iconW, 0,
                env.gadgetBounds.x,
                env.gadgetBounds.y + env.gadgetBounds.h - 1 - dock->iconH,
                dock->iconW, dock->iconH);
    }
}

VOID update_entire_window(struct DockWindow *dock)
{
    DEBUG(printf(__FUNC__ "\n"));

    BeginRefresh(dock->win);
    draw_gadgets(dock);
    EndRefresh(dock->win, TRUE);
}

struct Region *clip_to_gadget(struct DockWindow *dock, struct GadgetEnvironment *env)
{
    struct Region *oldR, *newR;
    struct Rectangle bounds;

    if( newR = NewRegion() ) {
    
        bounds.MinX = env->gadgetBounds.x;
        bounds.MinY = env->gadgetBounds.y;
        bounds.MaxX = env->gadgetBounds.x + env->gadgetBounds.w - 1;
        bounds.MaxY = env->gadgetBounds.y + env->gadgetBounds.h - 1;

        DEBUG(printf(__FUNC__ ": Clip to %ld,%ld -> %ld,%ld\n", 
            bounds.MinX, bounds.MinY, bounds.MaxX, bounds.MaxY));

        OrRectRegion(newR, &bounds);

        oldR = InstallClipRegion(dock->win->WLayer, newR);
    
    }

    return oldR;
}

VOID unclip_gadget(struct DockWindow *dock, struct Region *r)
{
    struct Region *oldR;
    if( r ) {

        oldR = InstallClipRegion(dock->win->WLayer, r);

        if( oldR ) {
            DisposeRegion(oldR);
        }
    }
}

VOID draw_gadgets(struct DockWindow *dock)
{
    struct RastPort *rp;
    struct DgNode *curr;
    struct GadgetEnvironment env;
    struct Rect r;
    struct Screen *screen;
    struct Region *region;

    if( dock->win ) {

        rp = dock->win->RPort;
        r.x = 0;
        r.y = 0;
        r.w = dock->win->Width;
        r.h = dock->win->Height;

        fill_background(dock, rp, &r);

        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
    
            DEBUG(printf(__FUNC__ ": Drawing %s\n", curr->n.ln_Name));

            DB_GetDockGadgetEnvironment(curr->dg, &env);

            if( region = clip_to_gadget(dock, &env) ) {

                dock_gadget_draw(curr->dg, rp);

                if( dock->hoverGad == curr->dg && DOCK_EDITING(dock) ) {

                    if( screen = LockPubScreen(NULL) ) {
                    
                        draw_edit_controls(dock, screen, rp, dock->hoverGad);

                        UnlockPubScreen(NULL, screen);
                    }                
                }

                unclip_gadget(dock, region);
            }
        }
        
        if( ! dock->cfg.showGadgetBorders ) {
            DB_DrawOutsetFrame(rp, &r);
        }
    }
}


VOID draw_gadget(struct DockWindow *dock, Object *gadget)
{
    struct Screen *screen;
    struct RastPort *rp;
    struct GadgetEnvironment env;
    struct Rect r;
    struct Region *region;

    if( dock->win ) {

        DB_GetDockGadgetEnvironment(gadget, &env);
    
        if( region = clip_to_gadget(dock, &env) ) {
            
            rp = dock->win->RPort;

            fill_background(dock, rp, &env.gadgetBounds);

            dock_gadget_draw(gadget, rp);

            if( ! dock->cfg.showGadgetBorders ) {

                r.x = 0;
                r.y = 0;
                r.w = dock->win->Width;
                r.h = dock->win->Height;

                DB_DrawOutsetFrame(rp, &r);
            }

            if( DOCK_EDITING(dock) && gadget == dock->hoverGad ) {

                if( screen = LockPubScreen(NULL) ) {

                    draw_edit_controls(dock, screen, rp, gadget);
                
                    UnlockPubScreen(NULL, screen);
                }
            }

            unclip_gadget(dock, region);
        }
    }
}

struct DgNode *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y)
{
    struct DgNode *curr;

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
                        
        if( dock_gadget_hit_test(curr->dg, x, y) ) {

            return curr;
        }
    }
    return NULL;    
}

VOID hide_gadget_label(struct DockWindow *dock)
{
    if( dock->hoverWin ) {
        CloseWindow(dock->hoverWin);
        dock->hoverWin = NULL;
    }
}

VOID show_gadget_label(struct DockWindow *dock, Object *gadget, STRPTR label)
{
    struct IntuiText text;
    struct TextAttr ta;
    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct GadgetEnvironment env;
    struct Rect b;
    UWORD w;

	struct TagItem tags[] = {
		{ WA_Left, 0 },
		{ WA_Top, 0 },
		{ WA_Width, 0 },
		{ WA_Height, 0 },
		{ WA_Borderless, TRUE },
        { WA_SmartRefresh, TRUE },
		{ TAG_DONE, NULL }
	};

    hide_gadget_label(dock);

    text.ITextFont = &ta;
   
    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            text.ITextFont->ta_Name = drawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
            text.ITextFont->ta_YSize = drawInfo->dri_Font->tf_YSize;
            text.ITextFont->ta_Style = drawInfo->dri_Font->tf_Style;
            text.ITextFont->ta_Flags = drawInfo->dri_Font->tf_Flags;
            text.FrontPen = drawInfo->dri_Pens[TEXTPEN];
            text.BackPen = drawInfo->dri_Pens[BACKGROUNDPEN];
            text.DrawMode = JAM2;

            FreeScreenDrawInfo(screen, drawInfo);

            text.ITextFont = &ta;
            text.IText = label;
            text.LeftEdge= 2;
            text.TopEdge = 2;
            text.NextText = NULL;
                        
            w = IntuiTextLength(&text);
            tags[2].ti_Data = w + 4;
            tags[3].ti_Data = text.ITextFont->ta_YSize + 4;

            DB_GetDockGadgetEnvironment(gadget, &env);

            switch( dock->cfg.pos ) {
                case DP_LEFT:
                    tags[0].ti_Data = dock->win->LeftEdge + dock->win->Width + 8;
                    tags[1].ti_Data = dock->win->TopEdge + env.gadgetBounds.y + (env.gadgetBounds.h - text.ITextFont->ta_YSize - 4) / 2;
                    break;

                case DP_RIGHT:
                    tags[0].ti_Data = dock->win->LeftEdge - w - 8;
                    tags[1].ti_Data = dock->win->TopEdge + env.gadgetBounds.y + (env.gadgetBounds.h - text.ITextFont->ta_YSize - 4) / 2;
                    break;

                case DP_TOP:
                    tags[0].ti_Data = dock->win->LeftEdge + env.gadgetBounds.x + (env.gadgetBounds.w - w - 4) / 2;
                    tags[1].ti_Data = dock->win->TopEdge + dock->win->Height + 8;
                    break;

                case DP_BOTTOM: 
                    tags[0].ti_Data = dock->win->LeftEdge + env.gadgetBounds.x + (env.gadgetBounds.w - w - 4) / 2;
                    tags[1].ti_Data = dock->win->TopEdge - text.ITextFont->ta_YSize - 12;
                    break;
            }

            if( (dock->hoverWin = OpenWindowTagList(NULL, tags) ) ) {
    
                PrintIText(dock->hoverWin->RPort, &text, 0, 0);
                b.x = 0;
                b.y = 0;
                b.w = tags[2].ti_Data;
                b.h = tags[3].ti_Data;
                DB_DrawOutsetFrame(dock->hoverWin->RPort, &b);
            }
        }

        UnlockPubScreen(NULL, screen);
    }         
}

VOID update_hover_gadget(struct DockWindow *dock)
{
    WORD mx = dock->win->MouseX;
    WORD my = dock->win->MouseY;
    struct DgNode *node;
    Object *gadget;
    Object *oldHoverGad;
    STRPTR label;

    oldHoverGad = dock->hoverGad;

    if( mx < 0 || my < 0
     || mx > dock->win->LeftEdge + dock->win->Width
     || my > dock->win->TopEdge + dock->win->Height ) {

        hide_gadget_label(dock);     

        dock->hoverGad = NULL;
        dock->hoverCount = HOVER_COUNT;

        if( oldHoverGad ) {
            draw_gadget(dock, oldHoverGad);
        }

    } else if( (node = get_gadget_at(dock, mx, my) ) ) {

        gadget = node->dg;

        if( gadget == dock->hoverGad ) {
            return;
        }

        dock->hoverGad = gadget;

        if( dock->cfg.showGadgetLabels ) {
            if( dock->hoverCount <= 0 ) {
                dock_gadget_get_label(gadget, &label);
        
                if( label && strlen(label) > 0 ) {

                    show_gadget_label(dock, gadget, label);
                }                
            } else {
                dock->hoverCount--;
            }
        }

        if( dock->runState == RS_EDITING ) {

            if( oldHoverGad ) {
                draw_gadget(dock, oldHoverGad);
            }

            if( dock->hoverGad ) {
                draw_gadget(dock, dock->hoverGad);
            }
        }
    }
}


VOID launch(struct DockWindow *dock, struct GadgetMessageLaunch *msg)
{
    execute_external(
        dock,
        msg->path,
        msg->args, 
        msg->console,
        msg->wb
    );

    dock_gadget_launched(msg->m.sender,
                         msg->path,
                         msg->args,
                         msg->console,
                         msg->wb);
}

struct DgNode *get_gadget_node(struct DockWindow *dock, Object *obj)
{
    struct DgNode *curr;

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
        if( obj == curr->dg ) {
            return curr;
        }
    }

    return NULL;
}

VOID register_gadget_port(struct DgNode *g, struct GadgetMessagePort *portMsg)
{
    struct PortReg *pr;

    DEBUG(printf(__FUNC__ ": %s %lx\n", g->n.ln_Name, portMsg->port));
    
    if( pr = DB_AllocMem(sizeof(struct PortReg), MEMF_CLEAR) ) {

        pr->port = portMsg->port;

        AddTail((struct List *)&g->ports, (struct Node *)pr);
    }
}

VOID unregister_gadget_port(struct DgNode *g, struct GadgetMessagePort *portMsg)
{
    struct PortReg *pr;

    DEBUG(printf(__FUNC__ ": %s %lx\n", g->n.ln_Name, portMsg->port));

    FOR_EACH_PORTREG(&g->ports, pr) {
        if( pr->port == portMsg->port ) {
            Remove((struct Node *)pr);

            DB_FreeMem(pr, sizeof(struct PortReg));

            break;
        }
    }         
}

VOID handle_gadget_message(struct DockWindow *dock)
{
    struct DgNode *curr;
    struct GadgetMessage *msg;
    
    while( msg = (struct GadgetMessage *)GetMsg(dock->gadgetPort) ) {
        if( dock->runState == RS_RUNNING ) {

            curr = get_gadget_node(dock, msg->sender);
            if( curr == NULL ) {
                break;
            }

            switch( msg->messageType ) {
                case GM_DRAW:
                    draw_gadget(dock, msg->sender);
                    break;

                case GM_QUIT:
                    dock->runState = RS_QUITTING;
                    break;    

                case GM_LAUNCH:
                    launch(dock, (struct GadgetMessageLaunch *)msg);
                    break;

                case GM_REGISTER_PORT:
                    register_gadget_port(curr, (struct GadgetMessagePort *)msg);
                    break;

                case GM_UNREGISTER_PORT:
                    unregister_gadget_port(curr, (struct GadgetMessagePort *)msg);
                    break;                
            }
        }

        DB_FreeMem(msg, msg->m.mn_Length);
    }
}

VOID remap_gadgets(struct DockWindow *dock)
{
    struct DgNode *curr;

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
        dock_gadget_remap(curr->dg);
    }
}

ULONG get_custom_sigs(struct DockWindow *dock)
{
    struct DgNode *curr;
    struct PortReg *pr;
    ULONG sig = 0;

//    DEBUG(printf("get_custom_sigs\n"));

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
        FOR_EACH_PORTREG(&curr->ports, pr) {

            sig |= (1 << pr->port->mp_SigBit);
        }
    }

    return sig;
}

VOID handle_custom_message(struct DockWindow *dock, ULONG signal)
{
    struct DgNode *curr;
    struct PortReg *pr;

    DEBUG(printf("handle_custom_message\n"));

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
        FOR_EACH_PORTREG(&curr->ports, pr) {

            DEBUG(printf("  dg = %s, p = %lx\n", curr->n.ln_Name, pr->port));

            if( signal & (1 << pr->port->mp_SigBit) ) {
    
                dock_gadget_message(curr->dg, pr->port);
            }
        }
    }
}

