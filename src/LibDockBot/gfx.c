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
    APTR image;
    struct BitMap *bm;
    PLANEPTR maskPlane;
    UWORD w, h, d;
    BOOL interleaved;
    BOOL freeMask;
};

static VOID DrawFrame(struct RastPort *rp, struct Rect* b, int p1, int p2)
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
    DrawFrame(rp, bounds, SHADOWPEN, SHINEPEN);
}

VOID __asm __saveds DB_DrawInsetFrame(
	register __a0 struct RastPort *rp,  
	register __a1 struct Rect *bounds)
{
    DrawFrame(rp, bounds, SHINEPEN, SHADOWPEN);
}

PLANEPTR CreateMaskPlane(struct BitMap *bm, UWORD w, UWORD h, UWORD d, BOOL interleaved, UWORD tc)
{
    struct RastPort srcRp;
    PLANEPTR mask = NULL, byte, plane;
    UWORD x, y, p, bit, stride = 0, bpr, rep = 1;
    UBYTE v = 0;

    DEBUG(DebugLog(__FUNC__ "\n"));

    InitRastPort(&srcRp);
    srcRp.BitMap = bm;
    
    bpr = bm->BytesPerRow;
    if( interleaved ) {

        /* ILBMs man...

           Because the blit will copy all planes at once
           the mask must multiply each line by the number
           of planes.

        */
        bpr = bm->BytesPerRow / d; // Number of _actual_ bytes per row.
        stride = bpr * (d - 1);    // Step between each row.
        rep = d;
    }

    DEBUG(DebugLog(__FUNC__ ": Mask size: %ld * %ld = %ld\n",
        bm->BytesPerRow, bm->Rows, bm->BytesPerRow * bm->Rows));
    DEBUG(DebugLog(__FUNC__ ": w = %ld, h = %ld, bpr = %ld, stride = %ld, rep = %ld\n",
        (ULONG)w, (ULONG)h, (ULONG)bpr, (ULONG)stride, (ULONG)rep));

    if( mask = AllocMemInternal(DockBotBaseFull, bm->BytesPerRow * bm->Rows, MEMF_CHIP|MEMF_CLEAR)) {

        byte = mask;

        for( y = 0; y < h; y++ ) {

            v = 0;
            bit = 7;

            for( x = 0; x < w; x++ ) {

                if( ReadPixel(&srcRp, x, y) != tc ) {
                    v |= (1 << bit);
                }

                if( bit == 0 ) {

                    // Copy the byte to each plane.
                    for( p = 0, plane = byte; p < rep; p++, plane += bpr ) {
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

VOID TrimBrush(struct Brush *b, UWORD tc)
{
    UWORD x, y, mx = 0, my = 0;
    struct RastPort rp;

    InitRastPort(&rp);
    rp.BitMap = b->bm;

    for( y = 0; y < b->h; y++ ) {
        for( x = 0; x < b->w; x++ ) {
            if( ReadPixel(&rp, x, y) != tc ) {
                if( x > mx ) {
                    mx = x;
                }
                if( y > my ) {
                    my = y;
                }
            }
        }
    }

    b->w = mx;
    b->h = my;
}

APTR __asm __saveds DB_LoadBrush(
    register __a0 STRPTR fileName,
    register __d0 UWORD flags)
{
    struct Brush *b;
    APTR img;
    struct Screen *screen;
    struct gpLayout lo;
    struct BitMapHeader *bmh;

    DEBUG(DebugLog("Load Brush %s\n", fileName));

    if( screen = LockPubScreen(NULL) ) {    

        if( img = NewDTObject(fileName, DTA_GroupID, GID_PICTURE, 
                                        PDTA_Remap, TRUE,
                                        PDTA_Screen, screen, 
                                        OBP_Precision, PRECISION_IMAGE,
                                        OBP_FailIfBad, TRUE,
                                        PDTA_FreeSourceBitMap, TRUE,
                                        PDTA_DestMode, 1L, // PMODE_V43
                                        TAG_END) ) {

            if( b = (struct Brush *)AllocMemInternal(DockBotBaseFull,
                                        sizeof(struct Brush),
                                         MEMF_ANY | MEMF_CLEAR) ) {
                b->image = img;

                lo.MethodID = DTM_PROCLAYOUT;
                lo.gpl_GInfo = NULL;
                lo.gpl_Initial = 1L;

                if( DoMethodA(img, (Msg)&lo) ) {

                    GetDTAttrs(img, 
                        PDTA_BitMap, &b->bm, 
                        PDTA_BitMapHeader, &bmh,
                        TAG_END);

                    b->w = GetBitMapAttr(b->bm, BMA_WIDTH);
                    b->h = GetBitMapAttr(b->bm, BMA_HEIGHT);
                    b->d = GetBitMapAttr(b->bm, BMA_DEPTH);
                    b->interleaved = GetBitMapAttr(b->bm, BMA_FLAGS) & BMF_INTERLEAVED;

                    if( flags & BF_CREATE_MASK ) {
                        GetDTAttrs(img,
                            PDTA_MaskPlane, &b->maskPlane,
                            TAG_END);

                        if( ! b->maskPlane ) {
                            b->freeMask = TRUE;
                            b->maskPlane = CreateMaskPlane(b->bm, b->w, b->h, b->d, b->interleaved, bmh->bmh_Transparent);
                        }
                    }

                    b->w = bmh->bmh_Width;
                    b->h = bmh->bmh_Height;
                    b->d = bmh->bmh_Depth;

                    if( flags & BF_TRIM ) {   
                        TrimBrush(b, bmh->bmh_Transparent);
                    }                    

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

    DEBUG(DebugLog("Free %lx\n", b));

    if( b->maskPlane && b->freeMask ) {
        DEBUG(DebugLog("  Mask Plane %ld bytes\n", b->bm->BytesPerRow * b->bm->Rows));
        FreeMemInternal(DockBotBaseFull, b->maskPlane, b->bm->BytesPerRow * b->bm->Rows);
    }

    if( b->image ) {
        DEBUG(DebugLog("  DT Object\n"));
        DisposeDTObject(b->image);
    }

    DEBUG(DebugLog("  Brush\n"));    
    FreeMemInternal(DockBotBaseFull, b, sizeof(struct Brush));
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

    if( w == 0 ) {
        w = b->w;
    }

    if( h == 0 ) {
        h = b->h;
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
