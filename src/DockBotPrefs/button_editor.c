
#include <exec/types.h>
#include <intuition/classes.h>
#include <libraries/mui.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>

#include "pref_editor.h"
#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "button_editor.h"

#define S_NAME "name"
#define S_PATH "path"
#define S_ICON "icon"
#define S_START "start"
#define S_ARGS "args"
#define S_CON "console"

#define ST_WB 0
#define ST_SH 1

struct DockButtonData {
    STRPTR name;
    STRPTR path;
    STRPTR icon;
    STRPTR args;
    STRPTR con;
    UWORD startType;
};

struct Values StartValues[] = {
    { "wb", ST_WB },
    { "sh", ST_SH },
    { NULL, 0 }
};

BOOL dock_button_read_config(Class *c, Object *obj, Msg msg)
{
    struct DockSettingValue v;
    struct Values *vals;
    UWORD len;
    struct DockPrefMsgConfig *m = (struct DockPrefMsgConfig *)msg;
    struct DockButtonData *dbd = INST_DATA(c, obj);

    while( DB_ReadSetting(m->settings, &v) ) {
        
        if( IS_KEY(S_NAME, v) ) {
            GET_STRING(v, dbd->name)     
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
        else if( IS_KEY(S_ARGS, v) ) {
            GET_STRING(v, dbd->args)
        }
        else if( IS_KEY(S_CON, v) ) {
            GET_STRING(v, dbd->con)
        }
    }    

    return TRUE;
}

/*
BOOL dock_button_write_config(Class *c, Object *obj, Msg msg)
{
    return FALSE;
}

BOOL dock_button_about(Class *c, Object *obj, Msg msg)
{
    return FALSE
}

BOOL dock_button_display_name(Class *c, Object *obj, Msg msg)
{
    
}
*/

ULONG dock_button_dispatch(Class *c, Object *obj, Msg msg)
{
    switch( msg->MethodID ) {
        //case DM_DISPLAYNAME:
        //    return (ULONG)dock_button_display_name(c, obj, msg);

//        case DM_ABOUT:
//            return (ULONG)dock_button_about(c, obj, msg);

        case DM_READCONFIG:
            return (ULONG)dock_button_read_config(c, obj, msg);
        
//        case DM_WRITECONFIG:
//            return (ULONG)dock_button_write_config(c, obj, msg);

        default:
            return DoSuperMethodA(c, obj, msg);
    }
}


Class *button_editor_init(VOID) {
    ULONG HookEntry();
    Class *c;

    if( c = MakeClass(BUTTON_EDITOR_CLASS, DB_ROOT_CLASS, NULL, sizeof(struct DockButtonData), 0)) {

        c->cl_Dispatcher.h_Entry = HookEntry;
        c->cl_Dispatcher.h_SubEntry = dock_button_dispatch;

        AddClass(c);
    }

    return c;
}

VOID button_editor_free(Class *c) {
    RemoveClass(c);
    FreeClass(c);
}

