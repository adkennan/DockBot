/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <intuition/classes.h>
#include <devices/inputevent.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/keymap_protos.h>

#include <stdio.h>

#include "class_def.h"


/* Raw key codes for keyboard selection of windows

  1 2 3 4 5 6 7 8 9
   Q W E R T Y U I O P
    A S D F G H J K L

  That's probably enough. How many windows do you have open?
*/
UWORD rawKeys[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x00
};


VOID build_window_list(struct SwitcherGadgetData *data)
{
    struct Screen *screen;
    struct DrawInfo *di;
    struct WindowListItem *item, *prevItem = NULL;
    struct Window *win;
    UWORD len, index, width;
    struct InputEvent ie = {0};
    BYTE buf[2];

    ie.ie_Class = IECLASS_RAWKEY;
    ie.ie_Qualifier = IEQUALIFIER_CAPSLOCK;
    ie.ie_EventAddress = NULL;

    if( screen = LockPubScreen(NULL) ) {
        if( di = GetScreenDrawInfo(screen) ) {

            data->width = 0;
            data->height = 0;
            index = 0;
            win = screen->FirstWindow;
            while( win && rawKeys[index] ) {

                if( win->Title ) {

                    len = strlen(win->Title) + 5;
                    if( item = (struct WindowListItem *)DB_AllocMem(sizeof(struct WindowListItem) + len, MEMF_CLEAR) ) {

                        item->selected = FALSE;
                        item->win = win;
                        item->rawKey = rawKeys[index];
                        item->labelLen = len;
                        item->height = di->dri_Font->tf_YSize;
                        item->n.ln_Name = (STRPTR)(((BYTE *)item) + sizeof(struct WindowListItem));

                        ie.ie_Code = item->rawKey;

                        MapRawKey(&ie, (STRPTR)&buf, 1, NULL);
                        buf[1] = '\0';
    
                        sprintf(item->n.ln_Name, "%s: %s", &buf, win->Title);

                        item->text.ITextFont = &item->ta;
                        item->text.ITextFont->ta_Name = di->dri_Font->tf_Message.mn_Node.ln_Name;
                        item->text.ITextFont->ta_YSize = item->height;
                        item->text.ITextFont->ta_Style = di->dri_Font->tf_Style;
                        item->text.ITextFont->ta_Flags = di->dri_Font->tf_Flags;
                        item->text.BackPen = di->dri_Pens[BACKGROUNDPEN];
                        item->text.DrawMode = JAM1;
                        item->text.IText = item->n.ln_Name;
                        item->text.NextText = NULL;
                        item->text.LeftEdge = 0;
                        item->text.TopEdge = data->height;
                
                        AddTail(&data->items, (struct Node *)item);

                        width = IntuiTextLength(&item->text);
                        if( width > data->width ) {
                            data->width = width;
                        }
                        data->height += item->height;
                        DEBUG(DB_Printf(__METHOD__ "item->height = %ld, data->height = %ld\n", item->height, data->height));

                        if( prevItem ) {
                            prevItem->text.NextText = &item->text;
                        }   
                        prevItem = item;

                        index++;
                    }
                }

                win = win->NextWindow;                
            }

            FreeScreenDrawInfo(screen, di);
        }

        UnlockPubScreen(NULL, screen);
    }
}

VOID free_window_list(struct SwitcherGadgetData *data) 
{
    struct WindowListItem *item;

    while( ! IsListEmpty(&data->items) ) {

        if( item = (struct WindowListItem *)RemTail(&data->items) ) {

            DB_FreeMem(item, sizeof(struct WindowListItem) + item->labelLen);
        }
    }

    data->width = 0;
    data->height = 0;
}

VOID draw_text(struct SwitcherGadgetData *data)
{
    struct Screen *screen;
    struct DrawInfo *di;
    struct WindowListItem *item;

    if( screen = LockPubScreen(NULL) ) {

        if( di = GetScreenDrawInfo(screen) ) {

            FOR_EACH_ITEM(&data->items, item) {

                item->text.FrontPen = item->selected ? di->dri_Pens[SHINEPEN] : di->dri_Pens[TEXTPEN];
            }
        }
    }
    PrintIText(data->win->RPort, &((struct WindowListItem *)data->items.lh_Head)->text, MARGIN_H, MARGIN_V);
}

VOID open_window(Object *o, struct SwitcherGadgetData *data)
{
    struct GadgetEnvironment env;
    LONG wX = 0, wY = 0, dwXl, dwYl, w, h, bx, by, bw, bh;
    struct Screen *screen;

    if( data->win ) {
        return;
    }

    data->bounceTimer = 2;

    DB_RequestDockGadgetDraw(o);

    build_window_list(data);    

    if( IsListEmpty(&data->items) ) {
        return;
    }

    DB_GetDockGadgetEnvironment(o, &env);

    DEBUG(DB_Printf(__METHOD__ "G = %ld,%ld,%ld,%ld W = %ld,%ld,%ld,%ld\n",
        (LONG)env.gadgetBounds.x,(LONG)env.gadgetBounds.y,(LONG)env.gadgetBounds.w,(LONG)env.gadgetBounds.h,
        (LONG)env.windowBounds.x,(LONG)env.windowBounds.y,(LONG)env.windowBounds.w,(LONG)env.windowBounds.h));

    dwXl = env.windowBounds.x;
    dwYl = env.windowBounds.y;
    bx = env.gadgetBounds.x;
    by = env.gadgetBounds.y;
    bw = env.gadgetBounds.w;
    bh = env.gadgetBounds.h;
    w = data->width + MARGIN_H * 2;    
    h = data->height + MARGIN_V * 2;   

    if( screen = LockPubScreen(NULL) ) {

        if( data->position == POS_DOCK ) {
            switch( env.pos ) {
                case DP_TOP:
                    wX = (dwXl + bx) + ((bw - w) >> 1);
                    wY = dwYl + env.windowBounds.h;
                    break;

                case DP_BOTTOM:
                    wX = (dwXl + bx) + ((bw - w) >> 1);
                    wY = dwYl - h;
                    break;
    
                case DP_LEFT:
                    wX = dwXl + env.windowBounds.w;
                    wY = (dwYl + by) + ((bh - h) >> 1);
                    break;
    
                case DP_RIGHT:
                    wX = dwXl - w;
                    wY = (dwYl + by) + ((bh - h) >> 1);
                    break;

                default:
                    DB_ShowError("WTF?");
                    break;
            }
        } else {

            wX = (screen->Width - w) / 2;
            wY = (screen->Height - h) / 2;
        }


        if( w > screen->Width ) {
            w = screen->Width;
        }

        if( h > screen->Height ) {
            h = screen->Height;
        }

        if( wX < 0 ) {
            wX = 0;
        } else if( wX + w > screen->Width ) {
            wX = screen->Width - w;
        }
    
        if( wY < 0 ) {
            wY = 0;
        } else if( wY + h > screen->Height ) {
            wY = screen->Height - h;
        }

        UnlockPubScreen(NULL, screen);
    }

    data->win = OpenWindowTags(NULL, 
        WA_Left, wX, WA_Top, wY,
        WA_Width, w,
        WA_Height, h,
        WA_SmartRefresh, TRUE,
        WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY,
        WA_Activate, TRUE,
        TAG_DONE);

    if( data->win ) {

        DB_RegisterPort(o, data->win->UserPort);

        draw_text(data);

        data->closeTimer = 30;
    }
}

VOID close_window(Object *o, struct SwitcherGadgetData *data)
{
    struct Window *win;

    if( ! data->win ) {
        return;
    }

    win = data->win;
    data->win = NULL;

    DB_UnregisterPort(o, win->UserPort);

    CloseWindow(win);

    data->closeTimer = 0;
    data->bounceTimer = 0;

    free_window_list(data);

    DB_RequestDockGadgetDraw(o);
}

VOID toggle_window(Object *o, struct SwitcherGadgetData *data)
{
    if( data->win ) {
        close_window(o, data);
    } else {
        open_window(o, data);
    }
}

VOID switch_to_selection(Object *o, struct SwitcherGadgetData *data, struct WindowListItem *item)
{
    struct Window *selectedWin, *win;
    struct Screen *screen;
   
    selectedWin = item->win;
    data->closeTimer = 1;

    item->selected = TRUE;
    draw_text(data);

    Forbid();

    if( screen = LockPubScreen(NULL) ) {

        win = screen->FirstWindow;
        while( win ) {

            if( win == selectedWin ) {

                WindowToFront(win);
                ActivateWindow(win);
                break;
            }
        
            win = win->NextWindow;                
        }

        UnlockPubScreen(NULL, screen);
    }

    Permit();
}

VOID switch_by_mouse(Object *o, struct SwitcherGadgetData *data, UWORD yPos)
{
    UWORD y = 0;
    struct WindowListItem *item;

    if( yPos >= MARGIN_V ) {
        yPos -= MARGIN_V;

        FOR_EACH_ITEM(&data->items, item) {

            DEBUG(DB_Printf(__METHOD__ "%s : yPos = %ld, y = %ld, item->height = %ld\n", item->n.ln_Name, (LONG)yPos, (LONG)y, (LONG)item->height));

            y += item->height;

            if( y > yPos ) {

                DEBUG(DB_Printf(__METHOD__ "Selected %s\n", item->n.ln_Name));
                switch_to_selection(o, data, item);
                break;
            }
        }       
    }

    DEBUG(DB_Printf(__METHOD__ "No item selected\n"));

    data->closeTimer = 1;
}

VOID switch_by_key(Object *o, struct SwitcherGadgetData *data, UWORD key)
{
    struct WindowListItem *item;

    FOR_EACH_ITEM(&data->items, item) {

        DEBUG(DB_Printf(__METHOD__ "%s : item->rawKey = %ld, key = %ld, item->height = %ld\n", item->n.ln_Name, (LONG)item->rawKey, (LONG)key, (LONG)item->height));

        if( item->rawKey == key ) {

            DEBUG(DB_Printf(__METHOD__ "Selected %s\n", item->n.ln_Name));

            switch_to_selection(o, data, item);
            break;
        }
    }

    DEBUG(DB_Printf(__METHOD__ "No item selected\n"));

    data->closeTimer = 1;
}

