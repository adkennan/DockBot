
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include "dockbot.h"
#include "dockbot_protos.h"


VOID __asm __saveds GetDockGadgetBounds(
	register __a0 Object *obj, 
	register __a1 struct Rect *bounds)
{
    
    struct DockMessageGetBounds msg = {
        DM_GETBOUNDS
    };
    msg.b = bounds;
    DoMethodA(obj, (Msg)&msg);
}
