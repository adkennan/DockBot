/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>
#include <clib/utility_protos.h>

#include <libraries/triton.h>


/****
** Icon Library v44+
*/
extern struct Library *IconBase;
#define CONST

#include "iconlib/icon.h"
#include "iconlib/icon_protos.h"
#include "iconlib/icon_pragmas.h"

/**
****/

#include <stdio.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "prefs.h"
#include "dock_settings.h"

#define CLASS_NAME "dockbuttonPrefs"

#define S_NAME      "name"
#define S_PATH      "path"
#define S_START     "start"
#define S_ARGS      "args"
#define S_CON       "console"
#define S_HOTKEY    "key"

#define ST_WB 0
#define ST_SH 1


struct DockButtonData
{
    STRPTR name;
    STRPTR path;
    STRPTR args;
    STRPTR con;
    STRPTR hotKey;
    struct DiskObject *diskObj;
    UWORD startType;

    STRPTR origName;
    STRPTR origArgs;
    STRPTR origCon;
    STRPTR origHotKey;
    UWORD origStartType;
};

struct Values StartValues[] = {
    { "wb", ST_WB },
    { "sh", ST_SH },
    { NULL, 0 }
};


VOID read_button_settings(Class *c, Object *o, Msg msg)
{
    struct DockButtonData *dbd = INST_DATA(c,o);
    struct DockMessageReadSettings *m = (struct DockMessageReadSettings *)msg;
    struct DockSettingValue v;
    struct Values *vals;
    struct Screen *screen;
    UWORD len;

    while( DB_ReadSetting(m->settings, &v) ) {
        
        if( IS_KEY(S_NAME, v) ) {
            GET_STRING(v, dbd->name)     
        }
        else if( IS_KEY(S_PATH, v) ) {
            GET_STRING(v, dbd->path)
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
        else if( IS_KEY(S_HOTKEY, v) ) {
            GET_STRING(v, dbd->hotKey)
        }
    }    

    if( dbd->diskObj = GetDiskObjectNew(dbd->path) ) {
        if( screen = LockPubScreen(NULL) ) {

            LayoutIconA(dbd->diskObj, screen, NULL);

            UnlockPubScreen(NULL, screen);
        }
    }
}

VOID write_button_settings(Class *c, Object *o, Msg msg)
{
}

VOID dispose_button_data(struct DockButtonData *dbd) 
{
    FREE_STRING(dbd->name);
    FREE_STRING(dbd->path);
    FREE_STRING(dbd->args);
    FREE_STRING(dbd->con);    
    FREE_STRING(dbd->hotKey);

    FREE_STRING(dbd->origName);
    FREE_STRING(dbd->origArgs);
    FREE_STRING(dbd->origCon);    
    FREE_STRING(dbd->origHotKey);

    if( dbd->diskObj ) {
        FreeDiskObject(dbd->diskObj);
    }
}

VOID dock_button_store(struct DockButtonData *dbd)
{
    FREE_STRING(dbd->origName);
    dbd->origName = dbd->name;

    FREE_STRING(dbd->origArgs);
    dbd->origArgs = dbd->args;

    FREE_STRING(dbd->origCon);
    dbd->origCon = dbd->con;
    
    FREE_STRING(dbd->origHotKey);
    dbd->origHotKey = dbd->hotKey;

    dbd->origStartType = dbd->startType;
}

STRPTR startTypes[] = { "Workbench", "Shell", NULL };

enum {
    OBJ_STR_PATH,
    OBJ_STR_NAME,
    OBJ_STR_ARGS,
    OBJ_CYC_START,
    OBJ_STR_CON
};

ULONG dock_button_get_editor(Class *c, Object *o)
{
    struct DockButtonData *dbd = INST_DATA(c, o);

    dock_button_store(dbd);
 
    return (ULONG)make_tag_list(   
        VertGroupA,
            Space,
            ColumnArray,
                Space,
                BeginColumn,
                    Space,
                    TextN("Path"),
                    Space,
                    TextN("Name"),
                    Space,
                    TextN("Arguments"),
                    Space,
                    TextN("Start Type"),
                    Space,
                    TextN("Console"),
                    Space,
                EndColumn,
                Space,
                BeginColumn,
                    Space,
                    StringGadget(dbd->path, OBJ_STR_PATH),
                    Space,
                    StringGadget(dbd->name, OBJ_STR_NAME),
                    Space,
                    StringGadget(dbd->args, OBJ_STR_ARGS),
                    Space,
                    CycleGadget(startTypes, dbd->startType, OBJ_CYC_START),
                    Space,  
                    StringGadget(dbd->con, OBJ_STR_CON),
                    Space,
                EndColumn,
                Space,
            EndArray,
            Space,
        EndGroup,
        TAG_END);
}

VOID dock_button_handle_event(Class *c, Object *o, Msg msg)
{
}

VOID dock_button_reset(Class *c, Object *o)
{
    struct DockButtonData *dbd = INST_DATA(c, o);

    FREE_STRING(dbd->name);
    dbd->name = dbd->origName;

    FREE_STRING(dbd->args);
    dbd->args = dbd->origArgs;

    FREE_STRING(dbd->con);
    dbd->con = dbd->origCon;
    
    FREE_STRING(dbd->hotKey);
    dbd->hotKey = dbd->origHotKey;

    dbd->startType = dbd->origStartType;
}

ULONG __saveds dock_button_dispatch(Class *c, Object *o, Msg msg)
{
    switch( msg->MethodID ) 
    {
        case OM_DISPOSE:
            dispose_button_data(INST_DATA(c,o));
            return DoSuperMethodA(c, o, msg);

        case DM_READ_CONFIG:
            read_button_settings(c, o, msg);
            break;

        case DM_WRITE_CONFIG:
            write_button_settings(c, o, msg);
            break;

        case DM_GET_NAME:
            return (ULONG)((struct DockButtonData *)INST_DATA(c, o))->name;

        case DM_CAN_EDIT:
            return TRUE;

        case DM_GET_EDITOR:
            return dock_button_get_editor(c, o);
    
        case DM_HANDLE_EVENT:
            dock_button_handle_event(c, o, msg);
            break;

        case DM_RESET:
            dock_button_reset(c, o);
            break;

        default:
            return DoSuperMethodA(c, o, msg);
    
    }

    return NULL;
}


Class *init_dock_button_class(VOID)
{
    ULONG HookEntry();
    Class *c;
    if( c = MakeClass(CLASS_NAME, DB_ROOT_PREFS_CLASS, NULL, sizeof(struct DockButtonData), 0) )
    {
        c->cl_Dispatcher.h_Entry = HookEntry;
        c->cl_Dispatcher.h_SubEntry = dock_button_dispatch;

        AddClass(c);
    }

    return c;
}

BOOL free_dock_button_class(Class *c)
{
    RemoveClass(c);
    
    return FreeClass(c);
}



