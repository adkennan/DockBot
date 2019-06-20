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

#include "dock_gadget.h"
#include "dock_handle.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <stdio.h>

BOOL add_dock_gadget(struct DockWindow *dock, Object *dg)
{
    struct DgNode *n;

    if( n = DB_AllocMem(sizeof(struct DgNode), MEMF_CLEAR) ) {
        n->dg = dg;
        AddTail(&dock->cfg.gadgets, (struct Node *)n);

        return TRUE;
    }

    return FALSE;
}

BOOL create_dock_handle(struct DockWindow *dock)
{
    Object *gad;
    
    if( gad = NewObjectA(dock->handleClass, NULL, TAG_DONE) ) {

        if( add_dock_gadget(dock, gad) ) {

            return TRUE;

        }
    }

    return FALSE;
}

BOOL init_gadget_classes(struct DockWindow *dock)
{
    if( dock->gadgetPort = CreateMsgPort() ) {
        if( dock->handleClass = init_dock_handle_class() ) {
            return TRUE;
        }   
    }
    return FALSE;
}

BOOL free_gadget_classes(struct DockWindow *dock)
{
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
    NewList(&dock->cfg.gadgets);

    if( create_dock_handle(dock) ) {
        return TRUE;
    }
    return FALSE; 
}


VOID remove_dock_gadgets(struct DockWindow *dock)
{
    struct DgNode *dg;

    disable_layout(dock);

    while( ! IsListEmpty(&dock->cfg.gadgets) ) {
        if( dg = (struct DgNode *)RemTail(&dock->cfg.gadgets) ) {
            DisposeObject(dg->dg);
            if( dg->n.ln_Name ) {
                FREE_STRING(dg->n.ln_Name);
            }
            DB_FreeMem(dg, sizeof(struct DgNode));
        }
    }
}


VOID draw_gadgets(struct DockWindow *dock)
{
    struct Window *win;
    struct RastPort *rp;
    struct DgNode *curr;

    if( dock->win ) {
    
        win = dock->win;
        rp = win->RPort;

        SetAPen(rp, 0);
        RectFill(rp, 0, 0, win->Width, win->Height);

        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            dock_gadget_draw(curr->dg, rp);
        }
    }
}


VOID draw_gadget(struct DockWindow *dock, Object *gadget)
{
    struct Window *win;
    struct RastPort *rp;
    struct Rect gb;

    if( dock->win ) {
    
        win = dock->win;
        rp = win->RPort;

        DB_GetDockGadgetBounds(gadget, &gb);

        SetAPen(rp, 0);
        RectFill(rp, gb.x, gb.y, gb.w, gb.h);
    
        dock_gadget_draw(gadget, rp);
    }
}

Object *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y)
{
    struct DgNode *curr;

    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {
                        
        if( dock_gadget_hit_test(curr->dg, x, y) ) {

            return curr->dg;
        }
    }
    return NULL;    
}

VOID hide_gadget_label(struct DockWindow *dock)
{
    dock->hoverGad = NULL;
    
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

    dock->hoverGad = gadget;

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

            DB_GetDockGadgetBounds(gadget, &b);

            switch( dock->cfg.pos ) {
                case DP_LEFT:
                    tags[0].ti_Data = dock->win->LeftEdge + dock->win->Width + 8;
                    tags[1].ti_Data = dock->win->TopEdge + b.y + (b.h - text.ITextFont->ta_YSize - 4) / 2;
                    break;

                case DP_RIGHT:
                    tags[0].ti_Data = dock->win->LeftEdge - w - 8;
                    tags[1].ti_Data = dock->win->TopEdge + b.y + (b.h - text.ITextFont->ta_YSize - 4) / 2;
                    break;

                case DP_TOP:
                    tags[0].ti_Data = dock->win->LeftEdge + b.x + (b.w - w - 4) / 2;
                    tags[1].ti_Data = dock->win->TopEdge + dock->win->Height + 8;
                    break;

                case DP_BOTTOM: 
                    tags[0].ti_Data = dock->win->LeftEdge + b.x + (b.w - w - 4) / 2;
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
    Object *gadget;
    STRPTR label;

    if( !dock->cfg.showGadgetLabels ) {
        return;
    }

    if( mx < 0 || my < 0
     || mx > dock->win->LeftEdge + dock->win->Width
     || my > dock->win->TopEdge + dock->win->Height ) {

        hide_gadget_label(dock);     

        dock->hoverCount = HOVER_COUNT;

    } else if( (gadget = get_gadget_at(dock, mx, my) ) ) {

        if( gadget == dock->hoverGad ) {
            return;
        }

        if( dock->hoverCount <= 0 ) {
            dock_gadget_get_label(gadget, &label);
        
            if( label && strlen(label) > 0 ) {

                show_gadget_label(dock, gadget, label);
            }                
        } else {
            dock->hoverCount--;
        }
    }
}


