

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/cghooks.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/intuition.h>

#include <dos/datetime.h>
#include <devices/timer.h>
#include <proto/utility.h>
#include <clib/utility_protos.h>
#include <clib/timer_protos.h>


#include <stdio.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

ULONG __saveds clock_draw(Class *c, Object *o, Msg m)
{   
    struct Rect b;
    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct IntuiText text;
    struct TextAttr ta;
    struct ClockGadgetData *g = INST_DATA(c, o);
    struct DockMessageDraw *d = (struct DockMessageDraw *)m;

    DB_GetDockGadgetBounds(o, &b);

    DB_DrawOutsetFrame(d->rp, &b);

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            ta.ta_Name = drawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
            ta.ta_YSize = drawInfo->dri_Font->tf_YSize;
            ta.ta_Style = drawInfo->dri_Font->tf_Style;
            ta.ta_Flags = drawInfo->dri_Font->tf_Flags;
    
            text.FrontPen = drawInfo->dri_Pens[TEXTPEN];
            text.BackPen = drawInfo->dri_Pens[BACKGROUNDPEN];
            text.DrawMode = JAM2;
            text.LeftEdge = 2;
            text.TopEdge = 2;
            text.ITextFont = &ta;
            text.IText = g->time;
            text.NextText = NULL;
            
            PrintIText(d->rp, &text, b.x, b.y);

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     

    return 1;    
}

struct Library *TimerBase;
struct Library *UtilityBase;

ULONG __saveds clock_tick(Class *c, Object *o, Msg m)
{
    struct ClockGadgetData *cd = INST_DATA(c, o);
    struct ClockData *clockData;
    struct timerequest *tr;
    struct timeval *tv;

    if( cd->counter == 0 ) {
        cd->counter = 20;
    }
    cd->counter--;

    if( UtilityBase = OpenLibrary("utility.library", 37) ) {

        if( tr = DB_AllocMem(sizeof(struct timerequest), MEMF_CLEAR) ) {

            if( tv = DB_AllocMem(sizeof(struct timeval), MEMF_CLEAR) ) {

                if( clockData = DB_AllocMem(sizeof(struct ClockData), MEMF_CLEAR) ) {

                    if( !(OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)tr, 0) ) ) {

                        TimerBase = (struct Library *)tr->tr_node.io_Device;
                
                        GetSysTime(tv);

                        Amiga2Date(tv->tv_secs, clockData);

                        sprintf((STRPTR)&cd->time, "%0d:%0d", clockData->hour, clockData->min);

                        CloseDevice((struct IORequest *)tr);

                        DB_RequestDockGadgetDraw(o);
                    }     
                    DB_FreeMem(clockData, sizeof(struct ClockData));
                }

                DB_FreeMem(tv, sizeof(struct timeval));
            }

            DB_FreeMem(tr, sizeof(struct timerequest));
        }

        CloseLibrary(UtilityBase);
    }

    return 1;
}

ULONG __saveds clock_click(Class *c, Object *o, Msg m)
{
    return 1;
}

ULONG __saveds clock_drop(Class *c, Object *o, Msg m)
{
    return 1;
}

ULONG __saveds clock_get_size(Class *c, Object *o, Msg m)
{
    struct DockMessageGetSize *s = (struct DockMessageGetSize *)m;
    s->w = DEFAULT_SIZE;
    s->h = 16;

    return 1;
}

ULONG __saveds clock_read_config(Class *c, Object *o, Msg m)
{
    return 1;
}

