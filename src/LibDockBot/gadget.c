/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
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

VOID __asm __saveds DB_RequestLaunch(
    register __a0 Object *obj,
    register __a1 STRPTR path,
    register __a2 STRPTR args,
    register __a3 STRPTR console,
    register __d0 BOOL wb) 
{
    struct DockMessageLaunch msg = {
        DM_REQ_LAUNCH
    };

    msg.path = path;
    msg.args = args;
    msg.console = console;
    msg.wb = wb;

    DoMethodA(obj, (Msg)&msg);    
}

