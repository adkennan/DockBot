/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>
#include <clib/utility_protos.h>

#include <libraries/triton.h>
#include <proto/triton.h>


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

    Object *icon;
};

struct Values StartValues[] = {
    { "wb", ST_WB },
    { "sh", ST_SH },
    { NULL, 0 }
};


#define TROB_Icon TRTG_PRVCLS(1)

#define TRAT_Icon_DiskObj TRTG_PRVOAT(1)


STRPTR startTypes[] = { "Workbench", "Shell", NULL };

enum {
    OBJ_STR_NAME = 1001,
    OBJ_STR_ARGS,
    OBJ_CYC_START,
    OBJ_STR_CON,
    OBJ_STR_HOTKEY,
    OBJ_ICON
};


struct TROD_Icon {
    struct TROD_DisplayObject DO;
    struct DiskObject *DiskObj;
    UWORD IconWidth;
    UWORD IconHeight;
};

TR_CONSTRUCTOR(Icon) 
{
    struct TROM_SetAttributeData saDat;

    Self.DiskObj = NULL;

    if(! TR_SUPERMETHOD) {
        return NULL;
    }

    Self.DO.MinWidth = 16;
    Self.DO.MinHeight = 16;

    if( data->parseargs ) {
        if( data->itemdata ) {
            saDat.attribute = TRAT_Icon_DiskObj;
            saDat.value = data->itemdata;

            TR_DoMethod((struct TROD_Object *)&Self, TROM_SETATTRIBUTE, (APTR)&saDat);

            if( Self.DiskObj ) {
                Self.DO.MinWidth = Self.IconWidth;
                Self.DO.MinHeight = Self.IconHeight;
            }
        }
    }
    Self.DO.XResize = TRUE;
    Self.DO.YResize = TRUE;

    return object;
}

TR_SIMPLEMETHOD(Icon, INSTALL_REFRESH)
{
    struct TR_Project *project;
    ULONG l, t, w, h;
    struct Rect bounds;

    TR_SUPERMETHOD;

    project = Self.DO.O.Project;
    l = Self.DO.Left;
    t = Self.DO.Top;
    w = Self.DO.Width;
    h = Self.DO.Height;

    if( Self.DiskObj ) {

        bounds.x = l + (w - Self.IconWidth) / 2,
        bounds.y = t + (h - Self.IconHeight) / 2,
        bounds.w = Self.IconWidth;
        bounds.h = Self.IconHeight;

        DrawIconStateA(project->trp_Window->RPort, Self.DiskObj,
                    NULL,
                    bounds.x,
                    bounds.y,
                    0, NULL);

        DB_DrawOutsetFrame(project->trp_Window->RPort, &bounds);
    }

    return 1;
}

TR_METHOD(Icon, SETATTRIBUTE, SetAttributeData)
{
    struct Rectangle r;
    switch(data->attribute)
    {
        case TRAT_Icon_DiskObj:
            Self.DiskObj = (struct DiskObject *)data->value;
            if( Self.DiskObj && GetIconRectangleA(NULL, Self.DiskObj, NULL, &r, NULL) ) {
                Self.IconWidth = (r.MaxX - r.MinX) + 1;
                Self.IconHeight = (r.MaxY - r.MinY) + 1;
            }
            break;

        default:
            return TR_SUPERMETHOD;
    }

    if( Self.DO.Installed ) {
        return TR_DoMethod(&Self.DO.O, TROM_REFRESH, NULL);
    }
    return 1;
}

TR_SIMPLEMETHOD(Icon, GETATTRIBUTE)
{
    switch((ULONG)data) 
    {
        case TRAT_Icon_DiskObj:
            return (ULONG)Self.DiskObj;

        default:
            return TR_SUPERMETHOD;
    }
}

TR_METHOD(Icon, EVENT, EventData)
{

    return TROM_EVENT_CONTINUE;
}

BOOL init_icon_class(VOID)
{
    return TR_AddClassTags(Application, 
        TROB_Icon, TROB_DisplayObject, 
        NULL, sizeof(struct TROD_Icon),

        TROM_NEW,           TRDP_Icon_NEW,
        TROM_INSTALL,       TRDP_Icon_INSTALL_REFRESH,
        TROM_REFRESH,       TRDP_Icon_INSTALL_REFRESH,
        TROM_SETATTRIBUTE,  TRDP_Icon_SETATTRIBUTE,
        TROM_GETATTRIBUTE,  TRDP_Icon_GETATTRIBUTE,
        TROM_EVENT,         TRDP_Icon_EVENT,
        TAG_END);   
}




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

    if( dbd->diskObj ) {
        FreeDiskObject(dbd->diskObj);
    }

    if( dbd->icon ) {
        DisposeObject(dbd->icon);
    }   
}

VOID dock_button_update(Class *c, Object *o, Msg msg)
{
    STRPTR str;
    UWORD len;
    struct DockButtonData *dbd = INST_DATA(c, o);
    struct TR_Project *proj = ((struct DockMessageUpdate *)msg)->project;

    FREE_STRING(dbd->name)
    FREE_STRING(dbd->args)
    FREE_STRING(dbd->con)
    FREE_STRING(dbd->hotKey)

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_NAME, 0);
    if( str && (len = strlen(str)) ) {        
        dbd->name = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, dbd->name, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_HOTKEY, 0);
    if( str && (len = strlen(str)) ) {        
        dbd->hotKey = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, dbd->hotKey, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_ARGS, 0);
    if( str && (len = strlen(str)) ) {        
        dbd->args = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, dbd->args, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_CON, 0);
    if( str && (len = strlen(str)) ) {        
        dbd->con = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, dbd->con, len + 1);
    }
    
    dbd->startType = (UWORD)TR_GetAttribute(proj, OBJ_CYC_START, TRAT_Value);
}


ULONG dock_button_get_editor(Class *c, Object *o)
{
    struct DockButtonData *dbd = INST_DATA(c, o);

    return (ULONG)make_tag_list(   
        VertGroupA,
            Space,
            HorizGroupC,
                Space,
                TROB_Icon, dbd->diskObj, ID(OBJ_ICON),
                Space,
            EndGroup,
            Space,
            HorizGroupSC,
                Space,
                TextT(dbd->path),
                Space,
            EndGroup,
            Space,
            ColumnArray,
                Space,
                BeginColumn,
                    Space,
                    TextN("Name"),
                    Space,
                    TextN("Arguments"),
                    Space,
                    TextN("Key"),
                    Space,
                    TextN("Start Type"),
                    Space,
                    TextN("Console"),
                    Space,
                EndColumn,
                Space,
                BeginColumn,
                    Space,
                    StringGadget(dbd->name, OBJ_STR_NAME),
                    Space,
                    StringGadget(dbd->args, OBJ_STR_ARGS),
                    Space,
                    StringGadget(dbd->hotKey, OBJ_STR_HOTKEY),
                    Space,
                    CycleGadget(startTypes, dbd->startType, OBJ_CYC_START),
                    Space,  
                    StringGadget(dbd->con, OBJ_STR_CON),
                        TRAT_Disabled, (dbd->startType == ST_WB),
                    Space,
                EndColumn,
                Space,
            EndArray,
        EndGroup,
        TAG_END);
}

VOID dock_button_handle_event(Class *c, Object *o, Msg msg)
{
    struct TR_Message *m = ((struct DockMessageHandleEvent *)msg)->msg;

    switch( m->trm_Class ) {
        case TRMS_NEWVALUE:
            switch( m->trm_ID ) {
                case OBJ_CYC_START:
                    TR_SetAttribute(m->trm_Project, OBJ_STR_CON, TRAT_Disabled, m->trm_Data == ST_WB);
                    break;

                default:
                    break;
            }

        default:
            break;
        
    }
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

        case DM_UPDATE:
            dock_button_update(c, o, msg);
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

