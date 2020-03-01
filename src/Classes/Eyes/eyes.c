/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2020 Andrew Kennan
**
************************************/

#include <math.h>

#include <intuition/intuition.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

#include <libraries/triton.h>
#include <proto/triton.h>

#define MIN_EYES 1
#define MAX_EYES 4
#define DEFAULT_EYES 2

// Config key
#define S_NUMEYES "numberOfEyes"

// Prefs UI ID.
enum {
    OBJ_NUM_EYES = 1001
};

extern struct GfxBase *GfxBase;

ULONG __saveds eyes_lib_init(struct EyesLibData* eld)
{
    if( eld->gfxBase = OpenLibrary("graphics.library", 39) ) {
        GfxBase = (struct GfxBase *)eld->gfxBase;
    
        return 1;
    }
    return 0;
}

ULONG __saveds eyes_lib_expunge(struct EyesLibData *eld)
{
    if( eld->gfxBase ) {
        CloseLibrary(eld->gfxBase);
    }

    return 1;
}

VOID init_eyes(struct EyesGadgetData *data)
{
    UWORD i;
    UWORD xs = ((data->w - 4) / (data->eyeCount * 2));
    UWORD ys = ((data->h - 4) / 2);
    UWORD rx = xs - 1;
    UWORD ry = ys - 1;
    struct Eye *eye;
    struct RastPort rp;
    LONG fillPen;
    struct Screen *screen;

    DEBUG(DB_Printf(__METHOD__ "\n"));

    // Get ready to draw the mask.
    InitRastPort(&rp);
    rp.BitMap = data->mask;
    rp.AreaInfo = &data->ai;
    rp.TmpRas = &data->tr;

    if( screen = LockPubScreen(NULL) ) {

        fillPen = ObtainBestPenA(screen->ViewPort.ColorMap, 0, 0, 0, NULL);

        SetAPen(&rp, fillPen);

        if( data->eyes = (struct Eye *)DB_AllocMem(data->eyeCount * sizeof(struct Eye), MEMF_CLEAR) ) {

            for( i = 0; i < data->eyeCount; i++ ) {

                eye = &data->eyes[i];
                eye->cx = 2 + xs + (2 * xs * i);
                eye->cy = 2 + ys;
                eye->sx = data->sx + eye->cx;
                eye->sy = data->sy + eye->cy;
                eye->rx = rx;
                eye->ry = ry;
                eye->ix = eye->cx;
                eye->iy = eye->cy;

                AreaEllipse(&rp, eye->cx, eye->cy, eye->rx, eye->ry); 
            }

            AreaEnd(&rp);
        }
        UnlockPubScreen(NULL, screen);
    }   
}

VOID free_eyes(struct EyesGadgetData *data)
{
    DEBUG(DB_Printf(__METHOD__ "\n"));

    if( data->eyes ) {
        DB_FreeMem(data->eyes, data->eyeCount * sizeof(struct Eye));
        data->eyes = NULL;
    }
}

BOOL measure_eye(struct Eye *eye, UWORD mx, UWORD my)
{
    double dx, dy, a;
    UWORD ix, iy;
    BOOL hasChanged;

    dx = mx - eye->sx;
    dy = my - eye->sy;

    a = atan2(dy, dx);   

    ix = eye->cx + (WORD)(cos(a) * (double)(eye->rx / 2));
    iy = eye->cy + (WORD)(sin(a) * (double)(eye->ry / 2));

    hasChanged = ix != eye->ix || iy != eye->iy;

    eye->ix = ix;
    eye->iy = iy;

    return hasChanged;    
}

VOID measure_eyes(struct EyesGadgetData *data)
{
    UWORD i;
    struct Screen *screen;

//    DEBUG(DB_Printf(__METHOD__ "\n"));

    if( screen = LockPubScreen(NULL) ) {

        for( i = 0; i < data->eyeCount; i++ ) {

            if( measure_eye(&data->eyes[i], screen->MouseX, screen->MouseY) ) {

                data->needsRedraw = TRUE;
            }
        }

        UnlockPubScreen(NULL, screen);
    }
}

VOID draw_eyes(struct EyesGadgetData *data)
{
    UWORD i;
    LONG fillPen, outlinePen;
    struct Screen *screen;
    struct Eye *eye;
    struct Rect b;

    if( screen = LockPubScreen(NULL) ) {

        fillPen = ObtainBestPenA(screen->ViewPort.ColorMap, 0xff << 24, 0xff << 24, 0xff << 24, NULL);
        outlinePen = ObtainBestPenA(screen->ViewPort.ColorMap, 0, 0, 0, NULL);

        SetAPen(&data->rp, fillPen);

        for( i = 0; i < data->eyeCount; i++ ) {

            eye = &data->eyes[i];

            AreaEllipse(&data->rp, eye->cx, eye->cy, eye->rx, eye->ry); 
        }

        AreaEnd(&data->rp);

        SetAPen(&data->rp, outlinePen);

        for( i = 0; i < data->eyeCount; i++ ) {

            eye = &data->eyes[i];

            AreaEllipse(&data->rp, eye->ix, eye->iy, 3, 3);

            DrawEllipse(&data->rp, eye->cx, eye->cy, eye->rx, eye->ry);
        }
    
        AreaEnd(&data->rp);

        data->needsRedraw = FALSE;

        ReleasePen(screen->ViewPort.ColorMap, fillPen);
        ReleasePen(screen->ViewPort.ColorMap, outlinePen);

        UnlockPubScreen(NULL, screen);
    }   

    b.x = 0;
    b.y = 0;
    b.w = data->w;
    b.h = data->h;
}

VOID free_offscreen_bm(struct EyesGadgetData *data)
{
    DEBUG(DB_Printf(__METHOD__ "\n"));

    data->rp.BitMap = NULL;
    data->rp.AreaInfo = NULL;
    data->rp.TmpRas = NULL;

    if( data->initialized ) {

        FreeBitMap(data->bm);
        FreeBitMap(data->mask);

        data->initialized = FALSE; 
    }

    if( data->aiBuf ) {
        DB_FreeMem(data->aiBuf, data->aiBufSize);
        data->aiBuf = NULL;
    }

    if( data->trBuf ) {
        DB_FreeMem(data->trBuf, data->trBufSize);
        data->trBuf = NULL;
    }
}

VOID init_offscreen_bm(Object *o, struct EyesGadgetData *data)
{
    struct Screen *screen;
    struct GadgetEnvironment env;

    if( data->initialized ) {
        free_offscreen_bm(data);
    }  

    DB_GetDockGadgetEnvironment(o, &env);

    data->trBufSize = RASSIZE(env.gadgetBounds.w * 2, env.gadgetBounds.h * 2);
    data->aiBufSize = 20 * data->eyeCount;

    if( screen = LockPubScreen(NULL) ) {

        if( data->trBuf = DB_AllocMem(data->trBufSize, MEMF_CHIP|MEMF_CLEAR) ) {

            if( data->aiBuf = DB_AllocMem(data->aiBufSize, MEMF_CHIP|MEMF_CLEAR) ) {

                if( data->bm = AllocBitMap(env.gadgetBounds.w, env.gadgetBounds.h, screen->BitMap.Depth, BMF_CLEAR, NULL) ) {

                    if( data->mask = AllocBitMap(env.gadgetBounds.w, env.gadgetBounds.h, 1, BMF_CLEAR, NULL) ) {

                        InitArea(&data->ai, data->aiBuf, data->aiBufSize);

                        InitTmpRas(&data->tr, data->trBuf, data->trBufSize);

                        InitRastPort(&data->rp);

                        data->rp.BitMap = data->bm;
                        data->rp.AreaInfo = &data->ai;
                        data->rp.TmpRas = &data->tr;

                        data->w = env.gadgetBounds.w;
                        data->h = env.gadgetBounds.h;
                        data->cx = env.gadgetBounds.x;
                        data->cy = env.gadgetBounds.y;
                        data->sx = env.windowBounds.x + env.gadgetBounds.x;
                        data->sy = env.windowBounds.y + env.gadgetBounds.y;
                        data->d = screen->BitMap.Depth;
    
                        data->initialized = TRUE;
                    } else {
                        DEBUG(DB_Printf(__METHOD__ "AllocBitMap for mask failed\n"));
                    }

                } else {
                    DEBUG(DB_Printf(__METHOD__ "AllocBitMap failed\n"));
                }
            }
        }

        UnlockPubScreen(NULL, screen);
    }
}

DB_METHOD_D(NEW)

    data->eyeCount = DEFAULT_EYES;

    data->initialized = FALSE;

    return 1;
}

DB_METHOD_D(DISPOSE)

    free_eyes(data);

    free_offscreen_bm(data);

    return 1;
}

DB_METHOD_DM(DRAW,DockMessageDraw)

    struct GadgetEnvironment env;

    DB_GetDockGadgetEnvironment(o, &env);

    if( env.gadgetBounds.x != data->cx || 
        env.gadgetBounds.y != data->cy || 
        env.gadgetBounds.w != data->w || 
        env.gadgetBounds.h != data->h ) {
        free_eyes(data);
        free_offscreen_bm(data);
    }

    if( ! data->initialized ) {

        init_offscreen_bm(o, data);
        init_eyes(data);
        measure_eyes(data);
    }

    if( data->initialized ) {

        if( data->needsRedraw ) {
            draw_eyes(data);
        }

        BltMaskBitMapRastPort(
            data->bm, 0, 0, 
            msg->rp, 
            env.gadgetBounds.x, env.gadgetBounds.y, 
            env.gadgetBounds.w, env.gadgetBounds.h, 
            0xE0,
            data->mask->Planes[0]
        );

        if( env.showBorders ) {
            DB_DrawOutsetFrame(msg->rp, &env.gadgetBounds);
        }
    }

    return 1;    
}

DB_METHOD_D(TICK)

    measure_eyes(data);

    if( data->needsRedraw ) {
        DB_RequestDockGadgetDraw(o);
    }

    return 1;
}

DB_METHOD_M(GETSIZE,DockMessageGetSize)

    struct Screen *screen;
    struct DisplayInfo dispInfo;
    ULONG dispMode;
    UBYTE xAspect = 1, yAspect = 1;

    if( screen = LockPubScreen(NULL) ) {

        dispMode = GetVPModeID(&screen->ViewPort);
        if( GetDisplayInfoData(NULL, (UBYTE *)&dispInfo, sizeof(struct DisplayInfo), DTAG_DISP, dispMode) ) {

            xAspect = dispInfo.Resolution.x;
            yAspect = dispInfo.Resolution.y;

            DEBUG(DB_Printf(__METHOD__ "xAspect = %ld, yAspect = %ld\n", (LONG)xAspect, (LONG)yAspect));
        }

        UnlockPubScreen(NULL, screen);
    }

    msg->w = DEFAULT_SIZE;
    msg->h = ((DEFAULT_SIZE * 2 / 3) * xAspect) / yAspect;

    DEBUG(DB_Printf(__METHOD__ "w = %ld, h = %ld\n", (LONG)msg->w, (LONG)msg->h));

    return 1;
}

DB_METHOD_DM(GETEDITOR,DockMessageGetEditor)

    msg->uiTags = DB_MakeTagList(   
        VertGroupA,
            Space,
            LineArray,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_NumEyes),
                    Space,
                    SliderGadget(MIN_EYES, MAX_EYES, data->eyeCount, OBJ_NUM_EYES),
                    Space,
                    Integer(data->eyeCount), TRAT_ID, OBJ_NUM_EYES,
                    Space,
                EndLine,
                Space,
            EndArray,
        EndGroup,
        TAG_END);

    return 1;
}

DB_METHOD_DM(EDITORUPDATE, DockMessageEditorUpdate)

    struct TR_Project *proj = msg->window;

    data->eyeCount = (UWORD)TR_GetAttribute(proj, OBJ_NUM_EYES, TRAT_Value);

    return 1;
}

DB_METHOD_M(CANEDIT, DockMessageCanEdit)

    msg->canEdit = TRUE;

    return 1;
}

DB_METHOD_DM(READCONFIG,DockMessageConfig)

    struct DockSettingValue v;

    while( DB_ReadSetting(msg->settings, &v) ) {
        
        if( IS_KEY(S_NUMEYES, v) ) {
            GET_INTEGER(v, data->eyeCount)
        }
    }

    if( data->eyeCount < MIN_EYES ) {
        data->eyeCount = MIN_EYES;
    } else if( data->eyeCount > MAX_EYES ) {
        data->eyeCount = MAX_EYES;
    }
    
    return 1;
}

DB_METHOD_DM(WRITECONFIG,DockMessageConfig)

    struct DockSettings *s = msg->settings;
    UBYTE eyeCount[3] = {0,0,0};

    eyeCount[0] = '0' + (UBYTE)data->eyeCount;

    DB_WriteSetting(s, S_NUMEYES, (STRPTR)&eyeCount);

    return 1;
}


