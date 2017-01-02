
#include "button.h"

#define TRAT_Icon_DiskObj TRTG_PRVOAT(1)


struct TROD_Icon {
    struct TROD_DisplayObject DO;
    struct DiskObject *DiskObj;
    UWORD IconWidth;
    UWORD IconHeight;
};

TR_CONSTRUCTOR(Icon) 
{
    struct TROM_SetAttributeData saDat;

    Self.DiskObj = NULL;

    if(! TR_SUPERMETHOD) {
        return NULL;
    }

    Self.DO.MinWidth = 16;
    Self.DO.MinHeight = 16;

    if( data->parseargs ) {
        if( data->itemdata ) {
            saDat.attribute = TRAT_Icon_DiskObj;
            saDat.value = data->itemdata;

            TR_DoMethod((struct TROD_Object *)&Self, TROM_SETATTRIBUTE, (APTR)&saDat);

            if( Self.DiskObj ) {
                Self.DO.MinWidth = Self.IconWidth;
                Self.DO.MinHeight = Self.IconHeight;
            }
        }
    }
    Self.DO.XResize = TRUE;
    Self.DO.YResize = TRUE;

    return object;
}

TR_SIMPLEMETHOD(Icon, INSTALL_REFRESH)
{
    struct TR_Project *project;
    ULONG l, t, w, h;
    struct Rect bounds;

    TR_SUPERMETHOD;

    project = Self.DO.O.Project;
    l = Self.DO.Left;
    t = Self.DO.Top;
    w = Self.DO.Width;
    h = Self.DO.Height;

    if( Self.DiskObj ) {

        bounds.x = l + (w - Self.IconWidth) / 2,
        bounds.y = t + (h - Self.IconHeight) / 2,
        bounds.w = Self.IconWidth;
        bounds.h = Self.IconHeight;

        DrawIconStateA(project->trp_Window->RPort, Self.DiskObj,
                    NULL,
                    bounds.x,
                    bounds.y,
                    0, NULL);

        DB_DrawOutsetFrame(project->trp_Window->RPort, &bounds);
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
            if( Self.DiskObj && GetIconRectangleA(NULL, Self.DiskObj, NULL, &r, NULL) ) {
                Self.IconWidth = (r.MaxX - r.MinX) + 1;
                Self.IconHeight = (r.MaxY - r.MinY) + 1;
            }
            break;

        default:
            return TR_SUPERMETHOD;
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

        default:
            return TR_SUPERMETHOD;
    }
}

TR_METHOD(Icon, EVENT, EventData)
{

    return TROM_EVENT_CONTINUE;
}

BOOL init_icon_class(VOID)
{
    return TR_AddClassTags(Application, 
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

