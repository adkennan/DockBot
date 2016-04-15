
#include "dock_gadget.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <utility/hooks.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include <stdio.h>

struct DockGadgetData {
	struct Rect bounds;
};


VOID draw_default_image(Class *c, Object *o, struct RastPort *rp, struct DockGadgetData *dgd)
{
    DrawOutsetFrame(rp, &dgd->bounds);
}

VOID read_settings(Msg msg)
{
    struct DockMessageReadSettings *m;
    struct DockSettingValue v;

    m = (struct DockMessageReadSettings *)msg;

    while( ReadSetting(m->settings, &v) ) {
        // Do nothing
    }    
}

ULONG __saveds dock_gadget_dispatch(Class *c, Object *o, Msg msg)
{
	struct DockMessageGetSize* gs;
	struct DockMessageSetBounds* sb;
    struct DockMessageGetBounds* gb;
    struct DockMessageDraw* dm;
    struct DockMessageHitTest* ht;
	struct DockGadgetData *dgd;
	Object *no = NULL;

	switch( msg->MethodID ) 
	{
		case OM_NEW:	
			if( no = (Object *)DoSuperMethodA(c, o, msg) )
			{	
				dgd = INST_DATA(c, no);
				dgd->bounds.x = 0;
				dgd->bounds.y = 0;
                dgd->bounds.w = 0;
                dgd->bounds.h = 0;
            }
			return (ULONG)no;

        case OM_DISPOSE:
            return DoSuperMethodA(c, o, msg);

		case DM_DRAW:
            dm = (struct DockMessageDraw *)msg;
			draw_default_image(c, o, dm->rp, INST_DATA(c, o));
			break;

		case DM_TICK:
            break;

		case DM_CLICK:
            break;

		case DM_DROP:
			break;

		case DM_SETBOUNDS:
			sb = (struct DockMessageSetBounds *)msg;			
			dgd = INST_DATA(c, o);
			dgd->bounds.x = sb->b->x;
			dgd->bounds.y = sb->b->y;			
            dgd->bounds.w = sb->b->w;
            dgd->bounds.h = sb->b->h;
			break;

        case DM_GETBOUNDS:
            gb = (struct DockMessageGetBounds *)msg;
            dgd = INST_DATA(c, o);
            gb->b->x = dgd->bounds.x;
            gb->b->y = dgd->bounds.y;
            gb->b->w = dgd->bounds.w;
            gb->b->h = dgd->bounds.h;
            break;

		case DM_GETSIZE:
			gs = (struct DockMessageGetSize *)msg;
			gs->w = DEFAULT_SIZE;
			gs->h = DEFAULT_SIZE;
			break;

        case DM_HITTEST:
            ht = (struct DockMessageHitTest *)msg;
            dgd = INST_DATA(c, o);
            if( dgd->bounds.x <= ht->x && ht->x < (dgd->bounds.x + dgd->bounds.w) &&
                dgd->bounds.y <= ht->y && ht->y < (dgd->bounds.y + dgd->bounds.h) ) {
                return 1;
            }
            break;

        case DM_READCONFIG:
            read_settings(msg);
            break;

		default:
			return DoSuperMethodA(c, o, msg);
			
	}
	return NULL;
}

Class *init_dock_gadget_class(VOID)
{
	ULONG HookEntry();
	Class	*c = NULL;
	if( c = MakeClass(DB_ROOT_CLASS, ROOTCLASS, NULL, sizeof(struct DockGadgetData), 0) )
	{
        c->cl_Dispatcher.h_Entry = HookEntry;
		c->cl_Dispatcher.h_SubEntry = dock_gadget_dispatch;

        AddClass(c);
	}

	return c;
}

ULONG free_dock_gadget_class(Class * c)
{
    RemoveClass(c);

    return (ULONG)FreeClass(c);
}

VOID dock_gadget_tick(Object *obj)
{
	DoMethod(obj, DM_TICK);
}

VOID dock_gadget_click(Object *obj, UWORD x, UWORD y)
{
	struct DockMessageClick msg = {
		DM_CLICK
	};
	msg.x = x;
	msg.y = y;

	DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_drop(Object *obj, STRPTR path)
{
	struct DockMessageDrop msg = {
		DM_DROP
	};
	msg.path = path;
	
	DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_set_bounds(Object *obj, struct Rect *bounds)
{
	struct DockMessageSetBounds msg = {
		DM_SETBOUNDS
	};
	msg.b = bounds;

	DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_get_bounds(Object *obj, struct Rect *bounds)
{
    struct DockMessageGetBounds msg = {
        DM_GETBOUNDS
    };
    DoMethodA(obj, (Msg)&msg);
    msg.b = bounds;
}

BOOL dock_gadget_hit_test(Object *obj, UWORD x, UWORD y)
{
    struct DockMessageHitTest msg = {
        DM_HITTEST
    };
    msg.x = x;
    msg.y = y;

    return (BOOL)(DoMethodA(obj, (Msg)&msg) ? TRUE : FALSE);
}

VOID dock_gadget_get_size(Object *obj
		, DockPosition position, DockAlign align
		, UWORD *w, UWORD *h)
{
	struct DockMessageGetSize msg = {
		DM_GETSIZE
	};
	msg.position = position;
	msg.align = align;

	DoMethodA(obj, (Msg)&msg);
	
	*w = msg.w;
	*h = msg.h;
}

VOID dock_gadget_draw(Object *obj, struct RastPort *rp)
{
    struct DockMessageDraw msg = {
        DM_DRAW       
    };
    msg.rp = rp;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageReadSettings msg = {
        DM_READCONFIG
    };
    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

