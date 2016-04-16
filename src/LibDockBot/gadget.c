
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


VOID __asm __saveds RequestDockQuit(
	register __a0 Object *obj)
{
    DoMethod(obj, DM_REQ_QUIT);
}


VOID __asm __saveds RequestDockGadgetDraw(
	register __a0 Object *obj) 
{
    DoMethod(obj, DM_REQ_DRAW);
}

