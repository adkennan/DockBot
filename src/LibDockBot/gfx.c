/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfx.h>

#include <clib/alib_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "dockbot.h"
#include "dockbot_protos.h"

#include "lib.h"

#define PDTA_DestMode (DTA_Dummy + 251)
#define PDTA_MaskPlane (DTA_Dummy + 258)

extern struct Library *IntuitionBase;
extern struct DockBotLibrary *DockBotBaseFull;

struct Brush {
    struct Node n;
    APTR image;
    struct BitMap *bm;
    PLANEPTR maskPlane;
    UWORD w, h, d;
    BOOL interleaved;
    BOOL freeMask;
};

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

PLANEPTR CreateMaskPlane(struct BitMap *bm, UWORD w, UWORD h, UWORD d, BOOL interleaved)
{
    struct RastPort srcRp;
    PLANEPTR mask = NULL, byte, plane;
    UWORD x, y, p, bit, stride, bpr;
    UBYTE v = 0;

    InitRastPort(&srcRp);
    srcRp.BitMap = bm;
    
    stride = 0;    
    bpr = bm->BytesPerRow;

    if( interleaved ) {

        /* ILBMs man...

           Because the blit will copy all planes at once
           the mask must multiply each line by the number
           of planes.

        */
        bpr = bm->BytesPerRow / d; // Number of _actual_ bytes per row.
        stride = bpr * (d - 1);    // Step between each row.
    }

    if( mask = AllocMemInternal(DockBotBaseFull, bm->BytesPerRow * bm->Rows, MEMF_CHIP|MEMF_CLEAR)) {

        byte = mask;

        for( y = 0; y < h; y++ ) {

            v = 0;
            bit = 7;

            for( x = 0; x < w; x++ ) {

                if( ReadPixel(&srcRp, x, y) ) {
                    v |= (1 << bit);
                }

                if( bit == 0 ) {

                    // Copy the byte to each plane.
                    for( p = 0, plane = byte; p < d; p++, plane += bpr ) {
                        *plane = v;
                    }

                    byte++;
                    bit = 7;
                    v = 0;

                } else {
                    bit--;
                }
            }

            byte += stride;
        }
    }

    return mask;
}

APTR __asm __saveds DB_LoadBrush(
    register __a0 STRPTR fileName,
    register __d0 BOOL createMask)
{
    struct Brush *b;
    APTR img;
    struct Screen *screen;
    struct gpLayout lo;

    UWORD l = strlen(fileName) + 1;

    for( b = (struct Brush *)DockBotBaseFull->l_Brushes.lh_Head;
         b->n.ln_Succ;
         b = (struct Brush *)b->n.ln_Succ ) {
    
        if( strcmp(fileName, b->n.ln_Name) == 0 ) {
            return b;
        }
    }

    if( screen = LockPubScreen(NULL) ) {    

        if( img = NewDTObject(fileName, DTA_GroupID, GID_PICTURE, 
                                        PDTA_Remap, FALSE,
                                        PDTA_Screen, screen, 
                                        OBP_Precision, PRECISION_GUI,
                                        OBP_FailIfBad, FALSE,
                                        PDTA_FreeSourceBitMap, TRUE,
                                        PDTA_DestMode, 1L, // PMODE_V43
                                        TAG_END) ) {

            if( b = (struct Brush *)AllocMemInternal(DockBotBaseFull,
                                        sizeof(struct Brush) + l,
                                         MEMF_ANY | MEMF_CLEAR) ) {

                b->n.ln_Name = ((UBYTE *)b) + sizeof(struct Brush);
                CopyMem(fileName, b->n.ln_Name, l);

                b->image = img;

                lo.MethodID = DTM_PROCLAYOUT;
                lo.gpl_GInfo = NULL;
                lo.gpl_Initial = 1L;

                if( DoMethodA(img, (Msg)&lo) ) {

                    GetDTAttrs(img, 
                        PDTA_BitMap, &b->bm, 
                        TAG_END);

                    b->w = GetBitMapAttr(b->bm, BMA_WIDTH);
                    b->h = GetBitMapAttr(b->bm, BMA_HEIGHT);
                    b->d = GetBitMapAttr(b->bm, BMA_DEPTH);
                    b->interleaved = GetBitMapAttr(b->bm, BMA_FLAGS) & BMF_INTERLEAVED;

                    if( createMask ) {
                        GetDTAttrs(img,
                            PDTA_MaskPlane, &b->maskPlane,
                            TAG_END);

                        if( ! b->maskPlane ) {
                            b->freeMask = TRUE;
                            b->maskPlane = CreateMaskPlane(b->bm, b->w, b->h, b->d, b->interleaved);
                        }
                    }

                    AddTail(&DockBotBaseFull->l_Brushes, (struct Node *)b);

                    return b;
                } else {
                    DEBUG(DebugLog("DTM_PROCLAYOUT failed\n"));
                }        

            } else {
                DEBUG(DebugLog("Failed to allocate memory\n"));
            }

            DisposeDTObject(img);

        } else {
            DEBUG(DebugLog("Failed to load image >%s<\n", fileName));
        }

        UnlockPubScreen(NULL, screen);
    }

    return NULL;
}

VOID __asm __saveds DB_FreeBrush(
    register __a0 APTR brush)
{
    struct Brush *b = (struct Brush *)brush;

    if( b->maskPlane && b->freeMask ) {
        FreeMemInternal(DockBotBaseFull, b->maskPlane, b->bm->BytesPerRow * b->bm->Rows);
    }

    if( b->image ) {
        DisposeDTObject(b->image);
    }

    Remove(&b->n);

    FreeMemInternal(DockBotBaseFull, b, sizeof(struct Brush) + strlen(b->n.ln_Name) + 1);
}

VOID __asm __saveds DB_DrawBrush(
    register __a0 APTR brush,
    register __a1 struct RastPort *rp,
    register __d0 UWORD srcX,
    register __d1 UWORD srcY,
    register __d2 UWORD dstX,
    register __d3 UWORD dstY,
    register __d4 UWORD w,
    register __d5 UWORD h)
{
    struct Brush *b = (struct Brush *)brush;

    if( ! b->bm ) {
        return;
    }

    if( b->maskPlane ) {

        BltMaskBitMapRastPort(b->bm, srcX, srcY, rp, dstX, dstY, w, h, 0xE0, b->maskPlane);

    } else {

        BltBitMapRastPort(b->bm, srcX, srcY, rp, dstX, dstY, w, h, 0xC0);
    }
}

VOID __asm __saveds DB_GetBrushSize(
    register __a0 APTR brush,
    register __a1 UWORD *w,
    register __a2 UWORD *h)
{
    struct Brush *b = (struct Brush *)brush;

    *w = b->w;
    *h = b->h;
}
