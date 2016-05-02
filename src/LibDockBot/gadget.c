/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  � 2016 Andrew Kennan
**
************************************/

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include "dockbot.h"
#include "dockbot_protos.h"


VOID __asm __saveds DB_GetDockGadgetBounds(
	register __a0 Object *obj, 
	register __a1 struct Rect *bounds)
{
    
    struct DockMessageGetBounds msg = {
        DM_GETBOUNDS
    };
    msg.b = bounds;
    DoMethodA(obj, (Msg)&msg);
}


VOID __asm __saveds DB_RequestDockQuit(
	register __a0 Object *obj)
{
    DoMethod(obj, DM_REQ_QUIT);
}


VOID __asm __saveds DB_RequestDockGadgetDraw(
	register __a0 Object *obj) 
{
    DoMethod(obj, DM_REQ_DRAW);
}

