/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "button.h"

#include <clib/graphics_protos.h>

#define MIN_SIZE 16

struct TROD_Icon {
    struct TROD_DisplayObject DO;
    struct DiskObject *DiskObj;
    APTR Brush;
    UWORD IconWidth;
    UWORD IconHeight;
};

TR_CONSTRUCTOR(Icon) 
{
    struct TROM_SetAttributeData saDat;
    struct IconInit *ii;

    Self.DiskObj = NULL;
    Self.Brush = NULL;
    Self.IconWidth = 0;
    Self.IconHeight = 0;

    if(! TR_SUPERMETHOD) {
        return NULL;
    }

    Self.DO.MinWidth = MIN_SIZE;
    Self.DO.MinHeight = MIN_SIZE;

    if( data->parseargs ) {
        if( data->itemdata ) {
            ii = (struct IconInit *)data->itemdata;

            if( ii->DiskObj ) {
                saDat.attribute = TRAT_Icon_DiskObj;
                saDat.value = (ULONG)ii->DiskObj;

                TR_DoMethod((struct TROD_Object *)&Self, TROM_SETATTRIBUTE, (APTR)&saDat);
            }

            if( ii->Brush ) {
                saDat.attribute = TRAT_Icon_Brush;
                saDat.value = (ULONG)ii->Brush;

                TR_DoMethod((struct TROD_Object *)&Self, TROM_SETATTRIBUTE, (APTR)&saDat);
            }

            DB_FreeMem(ii, sizeof(struct IconInit));
        }
    }
    Self.DO.XResize = TRUE;
    Self.DO.YResize = TRUE;

    return object;
}

TR_SIMPLEMETHOD(Icon, INSTALL_REFRESH)
{
    struct TR_Project *project = Self.DO.O.Project;
    struct RastPort *rp = project->trp_Window->RPort;
    ULONG l, t, w, h;
    struct Rect bounds;

    TR_SUPERMETHOD;

    if( Self.IconWidth > 0 && Self.IconHeight > 0 ) {

        l = Self.DO.Left;
        t = Self.DO.Top;
        w = Self.DO.Width;
        h = Self.DO.Height;

        SetAPen(rp, 0);
        RectFill(rp, l, t, l + w, t + h);

        bounds.x = l + (w - Self.IconWidth) / 2,
        bounds.y = t + (h - Self.IconHeight) / 2,
        bounds.w = Self.IconWidth + 2;
        bounds.h = Self.IconHeight + 2;

        if( Self.Brush ) {

            DB_DrawBrush(Self.Brush,
                         rp,
                         0, 0, 
                         bounds.x + 1, bounds.y + 1, 
                         bounds.w - 1, bounds.h - 1);

        } else if( Self.DiskObj ) {

            DrawIconStateA(rp, Self.DiskObj,
                        NULL,
                        bounds.x + 1,
                        bounds.y + 1,
                        0, NULL);

        }

        DB_DrawOutsetFrame(rp, &bounds);
    }

    return 1;
}

TR_METHOD(Icon, SETATTRIBUTE, SetAttributeData)
{
    struct Rectangle r;
    switch(data->attribute)
    {
        case TRAT_Icon_DiskObj:
            Self.DiskObj = (struct DiskObject *)data->value;
            break;

        case TRAT_Icon_Brush:
            Self.Brush = (APTR)data->value;
            break;

        default:
            return TR_SUPERMETHOD;
    }

    if( Self.Brush ) {
    
        DB_GetBrushSize(Self.Brush, &Self.IconWidth, &Self.IconHeight);
    
    } else if( Self.DiskObj 
            && GetIconRectangleA(NULL, Self.DiskObj, NULL, &r, NULL) ) {
    
        Self.IconWidth = (r.MaxX - r.MinX) + 1;
        Self.IconHeight = (r.MaxY - r.MinY) + 1;
    }

    if( Self.DO.MinWidth < Self.IconWidth + 4 ) {
        Self.DO.MinWidth = Self.IconWidth + 4;
    }
    
    if( Self.DO.MinHeight < Self.IconHeight + 4 ) {
        Self.DO.MinHeight = Self.IconHeight + 4;
    }

    if( Self.DO.Installed ) {
        return TR_DoMethod(&Self.DO.O, TROM_REFRESH, NULL);
    }

    return 1;
}

TR_SIMPLEMETHOD(Icon, GETATTRIBUTE)
{
    switch((ULONG)data) 
    {
        case TRAT_Icon_DiskObj:
            return (ULONG)Self.DiskObj;

        case TRAT_Icon_Brush:
            return (ULONG)Self.Brush;
            
        default:
            return TR_SUPERMETHOD;
    }
}

TR_METHOD(Icon, EVENT, EventData)
{

    return TROM_EVENT_CONTINUE;
}

BOOL init_icon_class(TR_App *app)
{
    return TR_AddClassTags(app, 
        TROB_Icon, TROB_DisplayObject, 
        NULL, sizeof(struct TROD_Icon),

        TROM_NEW,           TRDP_Icon_NEW,
        TROM_INSTALL,       TRDP_Icon_INSTALL_REFRESH,
        TROM_REFRESH,       TRDP_Icon_INSTALL_REFRESH,
        TROM_SETATTRIBUTE,  TRDP_Icon_SETATTRIBUTE,
        TROM_GETATTRIBUTE,  TRDP_Icon_GETATTRIBUTE,
        TROM_EVENT,         TRDP_Icon_EVENT,
        TAG_END);   
}

