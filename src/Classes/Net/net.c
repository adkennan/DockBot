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
#include <proto/socket.h>

#include <stdio.h>
#include <string.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

struct Library *SocketBase;

extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;

#define NET_TOOL "C:SampleNetSpeed"

/**********************************************************
 ** 
 ** PRIVATE: PrintSize
 ** 
 **********************************************************/

UBYTE * PrintSize(ULONG s, UBYTE * buf)
{
    STRPTR u[] = { "B", "KB", "MB", "GB", "TB" };
    ULONG r, i = 0;
    
    while (s > 1024) { r = s % 1024; s >>= 10; i++; }
    if (r > 1000) { r = 0; s++; }
    r /= 100;
    
    sprintf(buf, "%lu.%lu %s", s, r, u[i]);
    
    return(buf);
}

/**********************************************************
 ** 
 ** PRIVATE: LIB_INIT
 ** 
 **********************************************************/

ULONG __saveds net_lib_init(struct NetLibData *nld)
{
    if( nld->graphicsLib = OpenLibrary("graphics.library", 37) )
    {
        GfxBase = (struct GfxBase *)nld->graphicsLib;
    }

    if( nld->socketLib = OpenLibrary("bsdsocket.library", 4) )
    {
        SocketBase = (struct Library *)nld->socketLib;
    }

    if( nld->graphicsLib && nld->socketLib )
    {
        return 1;
    }

    return 0;
}

/**********************************************************
 ** 
 ** PRIVATE: LIB_EXPUNGE
 ** 
 **********************************************************/

ULONG __saveds net_lib_expunge(struct NetLibData *nld)
{
    if( nld->graphicsLib )
    {
        CloseLibrary(nld->graphicsLib);
    }

    if( nld->socketLib )
    {
        CloseLibrary(nld->socketLib);
    }

    return 1;
}

/**********************************************************
 ** 
 ** PRIVATE: SET_TEXT_FONT
 ** 
 **********************************************************/

VOID __saveds set_text_font(struct IntuiText *text)
{
    struct Screen *screen;
    struct DrawInfo *drawInfo;

    if( screen = LockPubScreen(NULL) )
    {
        if( drawInfo = GetScreenDrawInfo(screen) )
        {
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


/**********************************************************
 ** 
 ** PRIVATE: DRAW
 ** 
 **********************************************************/

VOID draw_net(struct RastPort *rp, struct DrawInfo *di, struct Rect *bounds, UWORD ix, STRPTR label, ULONG value)
{
    struct IntuiText text;
    struct TextAttr ta;
    BYTE buf[32];
    BYTE svalue[32];

    struct Rect frame;

    frame.x = bounds->x;
    frame.h = bounds->h / 2;
    frame.y = bounds->y + (frame.h * ix);
    frame.w = bounds->w;

    SetAPen(rp, di->dri_Pens[BACKGROUNDPEN]);
    RectFill(rp, frame.x, frame.y, frame.x + frame.w - 1, frame.y + frame.h - 1);

    DB_DrawOutsetFrame(rp, &frame);
    
    frame.x += 1;
    frame.y += 1;
    frame.w -= 2;
    frame.h -= 2;

    DB_DrawInsetFrame(rp, &frame);
    
    PrintSize(value, svalue);
    sprintf((STRPTR)&buf, label, svalue);

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

/**********************************************************
 ** 
 ** METHOD: DRAW
 ** 
 **********************************************************/

DB_METHOD_M(DRAW,DockMessageDraw)

    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct GadgetEnvironment env;

    if( screen = LockPubScreen(NULL) )
    {
        if( drawInfo = GetScreenDrawInfo(screen) )
        {
            SBQUAD_T byteRecv;
            SBQUAD_T byteSent;
            
            struct TagItem tags[3];
            
            tags[0].ti_Tag  = SBTM_GETREF(SBTC_GET_BYTES_RECEIVED);
            tags[0].ti_Data = (ULONG)&byteRecv;
            
            tags[1].ti_Tag  = SBTM_GETREF(SBTC_GET_BYTES_SENT);
            tags[1].ti_Data = (ULONG)&byteSent;
            
            tags[2].ti_Tag  = TAG_END;
            tags[2].ti_Data = 0;
            
            if (SocketBaseTagList(tags) == 0)
            {
                DB_GetDockGadgetEnvironment(o, &env);
                
                draw_net(msg->rp, drawInfo, &env.gadgetBounds, 0, (STRPTR)MSG_LBL_RECV, byteRecv.sbq_Low);
                draw_net(msg->rp, drawInfo, &env.gadgetBounds, 1, (STRPTR)MSG_LBL_SENT, byteSent.sbq_Low);
            }
            
            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     
    return 1;    
}

/**********************************************************
 ** 
 ** METHOD: CLICK
 ** 
 **********************************************************/

DB_METHOD_D(CLICK)

    DB_RequestDockGadgetDraw(o);

    DEBUG(DB_Printf(__METHOD__));
    
    data->clicked = TRUE;

    DB_RequestLaunch(o, NET_TOOL, NULL, NULL, TRUE);   

    return 1;
}

/**********************************************************
 ** 
 ** METHOD: LAUNCHED
 ** 
 **********************************************************/

DB_METHOD_D(LAUNCHED)

    DEBUG(DB_Printf(__METHOD__));
    
    data->clicked = FALSE;

    DB_RequestDockGadgetDraw(o);

    return 1;
}

/**********************************************************
 ** 
 ** METHOD: TICK
 ** 
 **********************************************************/

DB_METHOD_D(TICK)

    DEBUG(DB_Printf(__METHOD__));
    
    if( data->counter > 0 )
    {
        data->counter--;
        return 1;
    }

    data->counter = 10;

    DB_RequestDockGadgetDraw(o);

    return 1;
}

/**********************************************************
 ** 
 ** METHOD: GETSIZE
 ** 
 **********************************************************/

DB_METHOD_M(GETSIZE,DockMessageGetSize)

    struct IntuiText text = { 0 };
    struct TextAttr ta;
    UWORD rows, w = 0;
    BYTE buf[32];
    struct Screen *screen;
    struct DrawInfo *di;

    sprintf((STRPTR)buf, "U: 999.9 MB");
    
    if( screen = LockPubScreen(NULL) )
    {
        if( di = GetScreenDrawInfo(screen) )
        {
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

    rows = 2;

    msg->w = w > DEFAULT_SIZE ? w : DEFAULT_SIZE;
    msg->h = rows * (2 + text.ITextFont->ta_YSize) + 8;
 
    return 1;
}

/**********************************************************
 ** 
 ** END OF FILE
 ** 
 **********************************************************/

