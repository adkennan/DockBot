
#include "prefs.h"

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>


#include "dock_settings.h"

#include "dockbot_protos.h" 
#include "dockbot_pragmas.h"


struct PrefsBaseData
{
    STRPTR name;
};


VOID prefs_base_read_config(Msg msg)
{
    struct DockSettingValue v;
    struct DockMessageSettings *dms = (struct DockMessageSettings *)msg;

    while( DB_ReadSetting(dms->settings, &v) ) {
        // Read to the end...
    }
}

VOID prefs_base_set_name(Class *c, Object *o, Msg msg)
{
    struct DockMessageSetName *dmsn = (struct DockMessageSetName *)msg;
    struct PrefsBaseData *pbd = INST_DATA(c, o);
    UWORD l = strlen(dmsn->name);

    if( pbd->name = DB_AllocMem(l + 1, MEMF_ANY) ) {
        strncpy(pbd->name, dmsn->name, l + 1);
    }
}

VOID prefs_base_dispose(Class *c, Object *o)
{
    struct PrefsBaseData *pbd = INST_DATA(c, o);

    FREE_STRING(pbd->name);
}

ULONG __saveds prefs_base_dispatch(Class *c, Object *o, Msg msg)
{
    switch( msg->MethodID ) 
    {
        case OM_DISPOSE:
            prefs_base_dispose(c, o);
            return DoSuperMethodA(c, o, msg);

        case DM_GET_NAME:
            return (ULONG)(((struct PrefsBaseData *)INST_DATA(c,o))->name);

        case DM_SET_NAME:
            prefs_base_set_name(c, o, msg);
            break;

        case DM_GET_INFO:
            break;

        case DM_CAN_EDIT:
            return (ULONG)FALSE;
            
        case DM_GET_EDITOR:
            return (ULONG)NULL;
    
        case DM_HANDLE_EVENT:
            break;

        case DM_READ_CONFIG:
            prefs_base_read_config(msg);
            break;

        case DM_WRITE_CONFIG:
            break;

        case DM_RESET:
            break;

        default:
            return DoSuperMethodA(c, o, msg);
            
    }

    return NULL;
}


Class *init_prefs_base_class(VOID)
{
    ULONG HookEntry();
    Class *c;
    if( c = MakeClass(DB_ROOT_PREFS_CLASS, ROOTCLASS, NULL, sizeof(struct PrefsBaseData), 0) )
    {
        c->cl_Dispatcher.h_Entry = HookEntry;
        c->cl_Dispatcher.h_SubEntry = prefs_base_dispatch;

        AddClass(c);
    }

    return c;
}

BOOL free_prefs_base_class(Class *c)
{
    return FreeClass(c);
}
