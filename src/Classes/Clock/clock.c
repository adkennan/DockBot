

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

struct Library *TimerBase;
struct Library *UtilityBase;

ULONG __saveds clock_dispose(Class *c, Object *o, Msg m)
{
    struct ClockGadgetData *cd = INST_DATA(c, o);

    if( cd->clockData ) {
        DB_FreeMem(cd->clockData, sizeof(struct ClockData));
    }
    
    if( cd->tv ) {  
        DB_FreeMem(cd->tv, sizeof(struct timeval));
    }

    if( cd->tr ) {    
        CloseDevice((struct IORequest*)cd->tr);
        DB_FreeMem(cd->tr, sizeof(struct timerequest));
    }
    
    if( cd->utilityBase ) {
        CloseLibrary(cd->utilityBase);
    }

    return 1;
}

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

VOID __saveds init_clock(struct ClockGadgetData *cd)
{
    if( cd->tv ) {
        return;
    }

    if( cd->utilityBase = OpenLibrary("utility.library", 37) ) {

        if( cd->tr = DB_AllocMem(sizeof(struct timerequest), MEMF_CLEAR) ) {

            if( cd->tv = DB_AllocMem(sizeof(struct timeval), MEMF_CLEAR) ) {

                if( cd->clockData = DB_AllocMem(sizeof(struct ClockData), MEMF_CLEAR) ) {

                    if( !(OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)cd->tr, 0) ) ) {

                        UtilityBase = cd->utilityBase;
                        TimerBase = (struct Library *)cd->tr->tr_node.io_Device;
                        return;
                    }
                    DB_FreeMem(cd->clockData, sizeof(struct ClockData));
                }
                DB_FreeMem(cd->tv, sizeof(struct timeval));
            }
            DB_FreeMem(cd->tr, sizeof(struct timerequest));
        }
        CloseLibrary(cd->utilityBase);
    }
    cd->tv = NULL;
}

ULONG __saveds clock_tick(Class *c, Object *o, Msg m)
{
    struct ClockGadgetData *cd = INST_DATA(c, o);

    if( cd->counter > 0 ) {
        cd->counter--;
        return 1;
    }

    cd->counter = 20;

    init_clock(cd);
                    
    GetSysTime(cd->tv);

    Amiga2Date(cd->tv->tv_secs, cd->clockData);

    if( cd->hours != cd->clockData->hour 
     || cd->minutes != cd->clockData->min ) {
    
        sprintf((STRPTR)&cd->time, "%0d:%0d", cd->clockData->hour, cd->clockData->min);
    
        DB_RequestDockGadgetDraw(o);
    }

    return 1;
}

ULONG __saveds clock_get_size(Class *c, Object *o, Msg m)
{
    struct Screen *screen;
    struct DrawInfo *drawInfo;
    struct DockMessageGetSize *s = (struct DockMessageGetSize *)m;

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            s->h = drawInfo->dri_Font->tf_YSize + 4;
            s->w = DEFAULT_SIZE;

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);            
    }    

    return 1;
}

ULONG __saveds clock_read_config(Class *c, Object *o, Msg m)
{
    return 1;
}

