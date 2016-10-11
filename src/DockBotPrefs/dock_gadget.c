
#include "prefs.h"

#include <exec/types.h>
#include <intuition/classes.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>


STRPTR dock_gadget_get_name(Object *obj) 
{
    return (STRPTR)DoMethod(obj, DM_GET_NAME);
}

VOID dock_gadget_set_name(Object *obj, STRPTR name)
{
    struct DockMessageSetName msg = {
        DM_SET_NAME
    };
    msg.name = name;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_get_info(Object *obj
            , STRPTR *name, STRPTR *version
            , STRPTR *description, STRPTR *copyright)
{
    struct DockMessageGetInfo msg = {
        DM_GET_INFO
    };

    DoMethodA(obj, (Msg)&msg);

    *name = msg.name;
    *version = msg.version;
    *description = msg.description;
    *copyright = msg.copyright;   
}

BOOL dock_gadget_can_edit(Object *obj)
{
    return (BOOL)DoMethod(obj, DM_CAN_EDIT);
}

struct TagItem *dock_gadget_get_editor(Object *obj)
{
    return (struct TagItem *)DoMethod(obj, DM_GET_EDITOR);
}

VOID dock_gadget_handle_event(Object *obj, struct TR_Message *msg)
{
    struct DockMessageHandleEvent m = {
        DM_HANDLE_EVENT
    };
    m.msg = msg;
    
    DoMethodA(obj, (Msg)&m); 
}

VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageReadSettings msg = {
        DM_READ_CONFIG
    };
    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_write_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageReadSettings msg = {
        DM_WRITE_CONFIG
    };
    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_update(Object *obj, struct TR_Project *project)
{
    struct DockMessageUpdate msg = {
        DM_UPDATE
    };  
    msg.project = project;

    DoMethodA(obj, (Msg)&msg);
}

