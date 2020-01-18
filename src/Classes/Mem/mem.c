/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/cghooks.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include <stdio.h>
#include <string.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

extern struct GfxBase *GfxBase;

ULONG __saveds mem_lib_init(struct MemLibData *mld)
{
    if( mld->graphicsLib = OpenLibrary("graphics.library", 37) ) {
        GfxBase = (struct GfxBase *)mld->graphicsLib;
        return 1;
    }
    return 0;
}

ULONG __saveds mem_lib_expunge(struct MemLibData *mld)
{
    if( mld->graphicsLib ) {
        CloseLibrary(mld->graphicsLib);
    }
    return 1;
}

VOID __saveds set_text_font(struct IntuiText *text)
{
    struct Screen *screen;
    struct DrawInfo *drawInfo;
    
    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            text->ITextFont->ta_Name = drawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
            text->ITextFont->ta_YSize = drawInfo->dri_Font->tf_YSize;
            text->ITextFont->ta_Style = drawInfo->dri_Font->tf_Style;
            text->ITextFont->ta_Flags = drawInfo->dri_Font->tf_Flags;
            text->DrawMode = JAM1;
            text->NextText = NULL;

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     
}


VOID draw_mem(struct RastPort *rp, struct DrawInfo *di, struct Rect *bounds, UWORD ix, STRPTR label, ULONG total, ULONG free)
{
    struct IntuiText text;
    struct TextAttr ta;
    BYTE buf[32];
    struct Rect frame;
    
    frame.x = bounds->x;
    frame.h = bounds->h / 2;
    frame.y = bounds->y + (frame.h * ix);
    frame.w = bounds->w;

    SetAPen(rp, di->dri_Pens[BACKGROUNDPEN]);
    RectFill(rp, frame.x, frame.y, frame.x + frame.w, frame.y + frame.h);

    DB_DrawOutsetFrame(rp, &frame);
    
    frame.x += 1;
    frame.y += 1;
    frame.w -= 2;
    frame.h -= 2;

    DB_DrawInsetFrame(rp, &frame);

    frame.x += 1;
    frame.y += 1;
    frame.w = (((((total - free) * 100) / total) * frame.w) / 100) - 2;
    frame.h -= 2;

    SetAPen(rp, di->dri_Pens[FILLPEN]);
    RectFill(rp, frame.x, frame.y, frame.x + frame.w - 2, frame.y + frame.h - 2);

    DB_DrawOutsetFrame(rp, &frame);
    
    sprintf((STRPTR)&buf, label, (free * 100) / total);

    text.ITextFont = &ta;
    text.ITextFont->ta_Name = di->dri_Font->tf_Message.mn_Node.ln_Name;
    text.ITextFont->ta_YSize = di->dri_Font->tf_YSize;
    text.ITextFont->ta_Style = di->dri_Font->tf_Style;
    text.ITextFont->ta_Flags = di->dri_Font->tf_Flags;
    text.DrawMode = JAM1;
    text.NextText = NULL;
    text.IText = (STRPTR)&buf;
    text.FrontPen = di->dri_Pens[TEXTPEN];
    text.LeftEdge = frame.x + 2;
    text.TopEdge = frame.y + (frame.h - di->dri_Font->tf_YSize) / 2;

    PrintIText(rp, &text, 0, 0);
}

DB_METHOD_M(DRAW,DockMessageDraw)

    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct Rect b;
    UWORD winX, winY;
    ULONG totalChip, freeChip, totalFast, freeFast;
    
    totalChip = AvailMem(MEMF_CHIP | MEMF_TOTAL);
    freeChip = AvailMem(MEMF_CHIP);
    
    totalFast = AvailMem(MEMF_FAST | MEMF_TOTAL);
    freeFast = AvailMem(MEMF_FAST);

    DB_GetDockGadgetBounds(o, &b, &winX, &winY);

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            draw_mem(msg->rp, drawInfo, &b, 0, (STRPTR)MSG_LBL_Chip, totalChip, freeChip);

            if( totalFast > 0 ) {

                draw_mem(msg->rp, drawInfo, &b, 1, (STRPTR)MSG_LBL_Fast, totalFast, freeFast);
            }

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     
    return 1;    
}

DB_METHOD_D(TICK)

    if( data->counter > 0 ) {
        data->counter--;
        return 1;
    }

    data->counter = 50;

    DB_RequestDockGadgetDraw(o);

    return 1;
}

DB_METHOD_M(GETSIZE,DockMessageGetSize)

    struct IntuiText text;
    struct TextAttr ta;
    UWORD rows, w;    
    BYTE buf[32];
    struct Screen *screen;
    struct DrawInfo *di;

    sprintf((STRPTR)buf, strlen(MSG_LBL_Chip) > strlen(MSG_LBL_Fast) ? MSG_LBL_Chip : MSG_LBL_Fast, 99);
    
    if( screen = LockPubScreen(NULL) ) {
    
        if( di = GetScreenDrawInfo(screen) ) {

            text.ITextFont = &ta;
            text.ITextFont->ta_Name = di->dri_Font->tf_Message.mn_Node.ln_Name;
            text.ITextFont->ta_YSize = di->dri_Font->tf_YSize;
            text.ITextFont->ta_Style = di->dri_Font->tf_Style;
            text.ITextFont->ta_Flags = di->dri_Font->tf_Flags;
            text.IText = (STRPTR)buf;
            text.LeftEdge = 0;
            text.TopEdge = 0;
            text.NextText = NULL;

            w = IntuiTextLength(&text) + 10;

            FreeScreenDrawInfo(screen, di);
        }

        UnlockPubScreen(NULL, screen);
    }     

    rows = (AvailMem(MEMF_CHIP|MEMF_TOTAL) > 0 ? 1 : 0)
            + (AvailMem(MEMF_FAST|MEMF_TOTAL) > 0 ? 1 : 0);

    msg->w = w > DEFAULT_SIZE ? w : DEFAULT_SIZE;
    msg->h = rows * (2 + text.ITextFont->ta_YSize) + 8;
 
    return 1;
}

