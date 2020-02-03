/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include "dockbot.h"
#include "dock_gadget.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <utility/hooks.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include <stdio.h>

VOID dock_gadget_added(Object *obj, struct MsgPort *dockPort)
{
    struct DockMessageAdded msg = {
        DM_ADDED
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.dockPort = dockPort;
    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_removed(Object *obj)
{
    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethod(obj, DM_REMOVED);
}

VOID dock_gadget_tick(Object *obj)
{
//    DEBUG(printf(__FUNC__ ": %8x\n", obj));

	DoMethod(obj, DM_TICK);
}

VOID dock_gadget_click(Object *obj, UWORD x, UWORD y)
{
	struct DockMessageClick msg = {
		DM_CLICK
	};

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

	msg.x = x;
	msg.y = y;

	DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_drop(Object *obj, STRPTR* paths, UWORD count)
{
	struct DockMessageDrop msg = {
		DM_DROP
	};

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

	msg.paths = paths;
    msg.pathCount = count;
	
	DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_set_environment(Object *obj, struct GadgetEnvironment *env)
{
	struct DockMessageSetEnvironment msg = {
		DM_SETENV
	};

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

	msg.env = env;

	DoMethodA(obj, (Msg)&msg);
}

BOOL dock_gadget_hit_test(Object *obj, UWORD x, UWORD y)
{
    struct DockMessageHitTest msg = {
        DM_HITTEST
    };

//    DEBUG(printf(__FUNC__ ": %8x\n", obj));

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

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

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

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.rp = rp;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_remap(Object *obj) 
{
    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethod(obj, DM_REMAP);
}

VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageConfig msg = {
        DM_READCONFIG
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

BOOL dock_gadget_builtin(Object *obj) 
{
    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    return (BOOL)DoMethod(obj, DM_BUILTIN);
}

VOID dock_gadget_get_info(Object *obj
            , STRPTR *name, STRPTR *version
            , STRPTR *description, STRPTR *copyright) 
{
    struct DockMessageGetInfo msg = {
        DM_GETINFO
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethodA(obj, (Msg)&msg);

    *name = msg.name;
    *version = msg.version;
    *description = msg.description;
    *copyright = msg.copyright;
}

VOID dock_gadget_get_hotkey(Object *obj
			, STRPTR *hotKey)
{
    struct DockMessageGetHotKey msg = {
        DM_GETHOTKEY,
        NULL
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethodA(obj, (Msg)&msg);

    *hotKey = msg.hotKey;
}

VOID dock_gadget_hotkey(Object *obj)
{
    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethod(obj, DM_HOTKEY);
}

VOID dock_gadget_get_label(Object *obj
            , STRPTR *label)
{
    struct DockMessageGetLabel msg = {
        DM_GETLABEL,
        NULL
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethodA(obj, (Msg)&msg);

    *label = msg.label;
}

BOOL dock_gadget_can_edit(Object *obj)
{
    struct DockMessageCanEdit msg = {
        DM_CANEDIT
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethodA(obj, (Msg)&msg);

    return msg.canEdit;
}

VOID dock_gadget_init_button(Object *obj, STRPTR name, STRPTR path)
{
    struct DockMessageInitButton msg = {
        DM_INITBUTTON
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.name = name;
    msg.path = path;

    DoMethodA(obj, (Msg)&msg);
}

struct TagItem * dock_gadget_get_editor(Object *obj)
{
    struct DockMessageGetEditor msg = {
        DM_GETEDITOR
    };
    
    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    DoMethodA(obj, (Msg)&msg);

    return msg.uiTags;
}

VOID dock_gadget_editor_event(Object *obj, struct TR_Project *window, struct TR_Message *message)
{
    struct DockMessageEditorEvent msg = {
        DM_EDITOREVENT
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.window = window;
    msg.message = message;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_editor_update(Object *obj, struct TR_Project *window)
{
    struct DockMessageEditorUpdate msg = {
        DM_EDITORUPDATE
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.window = window;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_init_edit(Object *obj, struct TR_App *app)
{
    struct DockMessageEditorInit msg = {
        DM_INITEDIT
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.app = app;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_launched(Object *obj, STRPTR path, STRPTR args, STRPTR console, BOOL wb)
{
    struct DockMessageLaunch msg = {
        DM_LAUNCHED
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.path = path;
    msg.args = args;
    msg.console = console;
    msg.wb = wb;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_message(Object *obj, struct MsgPort *port)
{
    struct DockMessagePort msg = {
        DM_MESSAGE
    };

    DEBUG(printf(__FUNC__ ": %8x\n", obj));

    msg.port = port;

    DoMethodA(obj, (Msg)&msg);
}

