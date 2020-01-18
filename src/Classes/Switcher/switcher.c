/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <devices/inputevent.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>

#include <stdio.h>

#include "class_def.h"

struct Library *KeymapBase;
struct Library *GfxBase;
struct Library *UtilityBase;

ULONG __saveds switcher_lib_init(struct SwitcherLibData* cld)
{
    if( cld->keymapBase = OpenLibrary("keymap.library", 37) ) {

        KeymapBase = cld->keymapBase;
        
        if( cld->gfxBase = OpenLibrary("graphics.library", 37) ) {
            GfxBase = cld->gfxBase;

            if( cld->utilityBase = OpenLibrary("utility.library", 37) ) {
                UtilityBase = cld->utilityBase;
            
                return 1;
            }
        }
    }
    return 0;
}

ULONG __saveds switcher_lib_expunge(struct SwitcherLibData *cld)
{
    if( cld->keymapBase ) {
        CloseLibrary(cld->keymapBase);
    }
    if( cld->gfxBase ) {
        CloseLibrary(cld->gfxBase);
    }
    if( cld->utilityBase ) {
        CloseLibrary(cld->utilityBase);
    }
    return 1;
}

DB_METHOD_D(NEW)

    data->win = NULL;

    NewList(&data->items);

    return 1;
}

DB_METHOD_D(DISPOSE)

    close_window(o, data);

    FREE_STRING(data->hotkey);

    return 1;
}

DB_METHOD_DM(DRAW, DockMessageDraw)

    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct Rect b;
    UWORD winX, winY, off1, off2, x, y;
    struct Border b1;
    WORD xy[] = {      0,      0, 
                  ICON_W,      0, 
                  ICON_W, ICON_H,
                       0, ICON_H,
                       0,      1,
                  ICON_W,      1 };

    DB_GetDockGadgetBounds(o, &b, &winX, &winY);
    x = b.x + (b.w - (ICON_W + ICON_OFF * 2)) / 2;    
    y = b.y + (b.h - (ICON_H + ICON_OFF * 2)) / 2;
    

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            if( data->closeTimer == 0 ) {

                DB_DrawOutsetFrame(msg->rp, &b);

                off1 = ICON_OFF;
                off2 = ICON_OFF * 2;
            } else {

                DB_DrawInsetFrame(msg->rp, &b);

                off1 = ICON_OFF * 2;
                off2 = ICON_OFF;
            }

            b1.LeftEdge = 0;
            b1.TopEdge = 0;
            b1.XY = (WORD *)&xy;
            b1.Count = 6;
            b1.DrawMode = JAM1;
            b1.FrontPen = drawInfo->dri_Pens[SHADOWPEN];
            b1.NextBorder = NULL;

            DrawBorder(msg->rp, &b1, x, y);

            b1.LeftEdge = off1;
            b1.TopEdge = off1;

            SetAPen(msg->rp, drawInfo->dri_Pens[BACKGROUNDPEN]);
            RectFill(msg->rp, x + b1.LeftEdge, 
                              y + b1.TopEdge, 
                              x + b1.LeftEdge + ICON_W, 
                              y + b1.TopEdge + ICON_H);          

            DrawBorder(msg->rp, &b1, x, y);
                
            b1.FrontPen = drawInfo->dri_Pens[SHINEPEN];

            b1.LeftEdge = off2;
            b1.TopEdge = off2;

            RectFill(msg->rp, x + b1.LeftEdge, 
                              y + b1.TopEdge, 
                              x + b1.LeftEdge + ICON_W, 
                              y + b1.TopEdge + ICON_H);          

            DrawBorder(msg->rp, &b1, x, y);

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }            

    return 1;
}

DB_METHOD_D(CLICK)

    toggle_window(o, data);

    return 1;
}

DB_METHOD_DM(MESSAGE, DockMessagePort)

    struct IntuiMessage *im;
    UWORD keyCode = 0;
    UWORD mouseY = 0;

    DEBUG(DB_Printf(__METHOD__ "data = %lx\n"));

    while( im = (struct IntuiMessage *)GetMsg(msg->port) ) {

        switch( im->Class ) {
            case IDCMP_MOUSEBUTTONS:
                if( im->Code == SELECTDOWN ) {
                    mouseY = im->MouseY;
                    
                }
                break;

            case IDCMP_RAWKEY:
                keyCode = im->Code & ~0x80;                
                break;

            default:
                break;
        }

        ReplyMsg((struct Message *)im);
    }

    if( mouseY > 0 ) {
        DEBUG(DB_Printf(__METHOD__ "mouseY = %ld\n", (LONG)mouseY));
        switch_by_mouse(o, data, mouseY);

    } else if( keyCode > 0 ) {
        DEBUG(DB_Printf(__METHOD__ "keyCode = %ld\n", (LONG)keyCode));
        switch_by_key(o, data, keyCode);
    }

    return 1;
}

DB_METHOD_D(HOTKEY)

    toggle_window(o, data);

    return 1;
}

DB_METHOD_DM(GETSIZE,DockMessageGetSize)

    msg->w = ICON_W + ICON_OFF * 4;
    msg->h = ICON_H + ICON_OFF * 4;

    data->dockPos = msg->position;  
    data->dockAlign = msg->align;

    return 1;
}

DB_METHOD_D(TICK)

    if( data->closeTimer > 0 ) {
        
        data->closeTimer--;
        if( data->closeTimer == 0 ) {
            close_window(o, data);
        }
    }

    return 1;
}

DB_METHOD_DM(GETHOTKEY,DockMessageGetHotKey)

    msg->hotKey = data->hotkey;

    return 1;
}

