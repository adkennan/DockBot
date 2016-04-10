
#include <intuition/intuition.h>
#include <intuition/classes.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>

#include <stdio.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "dock_button.h"
#include "dock_settings.h"

#define CLASS_NAME "dockbutton"

#define S_NAME "name"
#define S_PATH "path"
#define S_ICON "icon"
#define S_START "start"

#define ST_WB 0
#define ST_SH 1

struct DockButtonData
{
    STRPTR name;
    STRPTR path;
    STRPTR icon;
    UWORD startType;
};

struct Values StartValues[] = {
    { "wb", ST_WB },
    { "sh", ST_SH },
    { NULL, 0 }
};

VOID dock_button_draw(Object *o, struct RastPort *rp)
{
    struct Rect bounds;

    GetDockGadgetBounds(o, &bounds);

    DrawOutsetFrame(rp, &bounds);
}

VOID read_button_settings(struct DockButtonData *dbd, Msg msg)
{
    struct DockMessageReadSettings *m;
    struct DockSettingValue v;
    struct Values *vals;
    UWORD len;

    m = (struct DockMessageReadSettings *)msg;

    while( ReadSetting(m->settings, &v) ) {
        
        if( IS_KEY(S_NAME, v) ) {
            GET_STRING(v, dbd->name)     
            printf("button = %s\n", dbd->name);
        }
        else if( IS_KEY(S_PATH, v) ) {
            GET_STRING(v, dbd->path)
        }
        else if( IS_KEY(S_ICON, v) ) {
            GET_STRING(v, dbd->icon)
        }
        else if( IS_KEY(S_START, v) ) {
            GET_VALUE(v, StartValues, vals, len, dbd->startType)
        }
    }    
}

VOID dispose_button_data(struct DockButtonData *dbd) 
{
    FREE_STRING(dbd->name);
    FREE_STRING(dbd->path);
    FREE_STRING(dbd->icon);
}

ULONG __saveds dock_button_dispatch(Class *c, Object *o, Msg msg)
{
    struct DockMessageDraw *dm;
    struct DockMessageGetSize *gsm;

    switch( msg->MethodID ) 
    {
		case OM_NEW:	
			return DoSuperMethodA(c, o, msg);

        case OM_DISPOSE:
            dispose_button_data(INST_DATA(c,o));
            return DoSuperMethodA(c, o, msg);            

        case DM_CLICK:

            break;

        case DM_DRAW:
            dm = (struct DockMessageDraw *)msg;
            dock_button_draw(o, dm->rp);
            break;

        case DM_GETSIZE:
            gsm = (struct DockMessageGetSize *)msg;
            switch( gsm->position ) {
                case DP_TOP:
                case DP_BOTTOM:
                    gsm->w = DEFAULT_SIZE;
                    gsm->h = DEFAULT_SIZE;
                    break;
                case DP_LEFT:
                case DP_RIGHT:
                    gsm->w = DEFAULT_SIZE;
                    gsm->h = DEFAULT_SIZE;
                    break;
            }
            break;

        case DM_READCONFIG:
            read_button_settings(INST_DATA(c, o), msg);
            break;

        default:
            return DoSuperMethodA(c, o, msg);
    }

    return NULL;
}

Class *init_dock_button_class(VOID) 
{
    ULONG HookEntry();
    Class *c = NULL;
    if( c = MakeClass(CLASS_NAME, DB_ROOT_CLASS, NULL, sizeof(struct DockButtonData), 0) )
    {
        c->cl_Dispatcher.h_Entry = HookEntry;
        c->cl_Dispatcher.h_SubEntry = dock_button_dispatch;

        AddClass(c);
    }

    return c;
}

VOID free_dock_button_class(Class *c)
{
    RemoveClass(c);

    FreeClass(c);
}