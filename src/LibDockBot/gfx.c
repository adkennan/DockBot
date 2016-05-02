/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <pragmas/intuition_pragmas.h>

#include "dockbot.h"
#include "dockbot_protos.h"

extern struct Library *IntuitionBase;

static VOID draw_frame(struct RastPort *rp, struct Rect* b, int p1, int p2)
{
    struct Screen *screen;
    struct DrawInfo *di;
    struct Border shineBorder;
    struct Border shadowBorder;
    ULONG shadow, shine;

    WORD tl[] = { 0, 0, 0, 0, 0, 0 };
    WORD br[] = { 0, 0, 0, 0, 0, 0 };

    tl[0] = b->w - 1;
    tl[5] = b->h - 1;
    br[0] = b->w - 1;
    br[2] = b->w - 1;
    br[3] = b->h - 1;
    br[5] = b->h - 1;

    shadow = 1;
    shine = 2;
    if( screen = LockPubScreen(NULL) ) {
        if( di = GetScreenDrawInfo(screen) ) {

            shadow = di->dri_Pens[p1];
            shine = di->dri_Pens[p2];            

            FreeScreenDrawInfo(screen, di);
        }
        UnlockPubScreen(NULL, screen);
    }
    
    shineBorder.LeftEdge = b->x;
    shineBorder.TopEdge = b->y;
    shineBorder.DrawMode = JAM1;
    shineBorder.FrontPen = shine;
    shineBorder.XY = (WORD*)&tl;
    shineBorder.Count = 3;
    shineBorder.NextBorder = &shadowBorder;

    shadowBorder.LeftEdge = b->x;
    shadowBorder.TopEdge = b->y;
    shadowBorder.DrawMode = JAM1;
    shadowBorder.FrontPen = shadow;
    shadowBorder.XY = (WORD*)&br;
    shadowBorder.Count = 3;
    shadowBorder.NextBorder = NULL;

    DrawBorder(rp, &shineBorder, 0, 0);
}

VOID __asm __saveds DB_DrawOutsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds)
{
    draw_frame(rp, bounds, SHADOWPEN, SHINEPEN);
}

VOID __asm __saveds DB_DrawInsetFrame(
	register __a0 struct RastPort *rp,  
	register __a1 struct Rect *bounds)
{
    draw_frame(rp, bounds, SHINEPEN, SHADOWPEN);
}

