

#include <exec/types.h>
#include <intuition/classes.h>
#include <libraries/mui.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>

#include "pref_editor.h"
#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

extern struct Library *MUIMasterBase;

struct EditorBase {
    STRPTR name;
    STRPTR gadgetName;
    STRPTR description;
    STRPTR version;
    STRPTR copyright;
};

STRPTR tabNames[] = { "Details", "About", NULL };

ULONG editor_base_new(Class *c, Object *obj, struct opSet *msg)
{
    if( obj = (Object *)DoSuperMethod(c, obj, OM_NEW, 
        RegisterGroup(tabNames),
            Child, HGroup,
                    Child, Label2("Placeholder"),
                End,
            End,
            Child, HGroup,
                Child, VGroup,
                    Child, Label2("Name"),
                    Child, Label2("Version"),
                    Child, Label2("Description"),
                    Child, Label2("Copyright"),
                End,
                Child, VGroup,
                    Child, Label2(""),
                    Child, Label2(""),
                    Child, Label2(""),
                    Child, Label2(""),
                End,
            End, NULL
        ))
    {



        return (ULONG)obj;
    }

    return 0;
}

VOID editor_base_dispose(Class *c, Object *obj, Msg msg)
{
    struct EditorBase *dgi = INST_DATA(c, obj);

    FREE_STRING(dgi->name);
    FREE_STRING(dgi->gadgetName);
    FREE_STRING(dgi->description);
    FREE_STRING(dgi->version);
    FREE_STRING(dgi->copyright);
}

ULONG editor_base_display_name(Class *c, Object *obj, Msg msg)
{
    struct EditorBase *dgi = INST_DATA(c, obj);

    return (ULONG)dgi->gadgetName;
}

ULONG __saveds editor_base_dispatch(Class *c, Object *obj, Msg msg)
{
    switch( msg->MethodID ) {
        case OM_NEW:
            return editor_base_new(c, obj, (struct opSet *)msg);

        case OM_DISPOSE:
            editor_base_dispose(c, obj, msg);
            return DoSuperMethodA(c, obj, msg);

        case DM_DISPLAYNAME:
            return editor_base_display_name(c, obj, msg);

        case DM_ABOUT:
        case DM_READCONFIG:
        case DM_WRITECONFIG:
            return NULL;
        

        default:
            return DoSuperMethodA(c, obj, msg);
    
    }
}

struct MUI_CustomClass *editor_base_init(VOID) {

    return MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct EditorBase), editor_base_dispatch);
}

VOID editor_base_free(MUI_CustomClass *c)
{
    MUI_DeleteCustomClass(c);
}
