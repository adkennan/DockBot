
#include <intuition/intuition.h>
#include <intuition/classes.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dock_gadget.h"
#include "dock_handle.h"



#define HANDLE_SIZE 6


VOID dock_handle_draw(Object *o, struct RastPort *rp)
{
    struct Rect bounds;

    GetDockGadgetBounds(o, &bounds);

    DrawOutsetFrame(rp, &bounds);
}

ULONG __saveds dock_handle_dispatch(Class *c, Object *o, Msg msg)
{
    struct DockMessageDraw *dm;
    struct DockMessageGetSize *gsm;

    switch( msg->MethodID ) 
    {

        case DM_CLICK:

            break;

        case DM_DRAW:
            dm = (struct DockMessageDraw *)msg;
            dock_handle_draw(o, dm->rp);
            break;

        case DM_GETSIZE:
            gsm = (struct DockMessageGetSize *)msg;
            switch( gsm->position ) {
                case DP_TOP:
                case DP_BOTTOM:
                    gsm->w = HANDLE_SIZE;
                    gsm->h = DEFAULT_SIZE;
                    break;
                case DP_LEFT:
                case DP_RIGHT:
                    gsm->w = DEFAULT_SIZE;
                    gsm->h = HANDLE_SIZE;
                    break;
            }
            break;

        default:
            return DoSuperMethodA(c, o, msg);
    }

    return NULL;
}

Class *init_dock_handle_class(VOID) 
{
    ULONG HookEntry();
    Class *c = NULL;
    if( c = MakeClass(NULL, DB_ROOT_CLASS, NULL, 0, 0) )
    {
        c->cl_Dispatcher.h_Entry = HookEntry;
        c->cl_Dispatcher.h_SubEntry = dock_handle_dispatch;
    }

    return c;
}

VOID free_dock_handle_class(Class *c)
{
    FreeClass(c);
}