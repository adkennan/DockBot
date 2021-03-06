/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  � 2019 Andrew Kennan
**
************************************/

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

#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>

#include "lib.h"
#include "dockbot.h"
#include "dockbot_protos.h"

extern struct IntuitionBase *IntuitionBase;

struct DockGadgetData {
    struct MsgPort *dockPort;
    struct GadgetEnvironment env;
};


VOID draw_default_image(Class *c, Object *o, struct RastPort *rp, struct DockGadgetData *dgd)
{
    DB_DrawOutsetFrame(rp, &dgd->env.gadgetBounds);
}

VOID read_settings(Msg msg)
{
    struct DockMessageConfig *m = (struct DockMessageConfig *)msg;
    struct DockSettingValue v;

    while( DB_ReadSetting(m->settings, &v) ) {
        // Do nothing
    }    
}

VOID send_message_to_dock(Class *c, Object *o, GadgetMessageType gm, struct GadgetMessage *msg)
{
	struct DockGadgetData *dgd = INST_DATA(c,o);

    msg->m.mn_Node.ln_Type = NT_MESSAGE;
    msg->m.mn_ReplyPort = NULL;
    msg->messageType = gm;
    msg->sender = o;

    PutMsg(dgd->dockPort, (struct Message *)msg);
}

VOID send_message_no_params(Class *c, Object *o, GadgetMessageType gm)
{
	struct DockGadgetData *dgd = INST_DATA(c,o);
    struct GadgetMessage *msg;

    if( dgd->dockPort ) {
        if( msg = DB_AllocMem(sizeof(struct GadgetMessage), MEMF_CLEAR) ) {

            msg->m.mn_Length = sizeof(struct GadgetMessage);

            send_message_to_dock(c, o, gm, msg);
        }
    }
}

VOID send_launch_msg_to_dock(Class *c, Object *o, struct DockMessageLaunch *dml)
{
	struct DockGadgetData *dgd = INST_DATA(c,o);
    struct GadgetMessageLaunch *msg;

    if( dgd->dockPort ) {
        if( msg = DB_AllocMem(sizeof(struct GadgetMessageLaunch), MEMF_CLEAR) ) {

            msg->m.m.mn_Length = sizeof(struct GadgetMessageLaunch);

            msg->path = dml->path;
            msg->args = dml->args;
            msg->console = dml->console;
            msg->wb = dml->wb;

            send_message_to_dock(c, o, GM_LAUNCH, (struct GadgetMessage *)msg);
        }
    }
}

VOID send_port_msg_to_dock(Class *c, Object *o, struct DockMessagePort *dmp, ULONG msgID)
{
	struct DockGadgetData *dgd = INST_DATA(c,o);
    struct GadgetMessagePort *msg;

    if( dgd->dockPort ) {
        if( msg = DB_AllocMem(sizeof(struct GadgetMessagePort), MEMF_CLEAR) ) {

            msg->m.m.mn_Length = sizeof(struct GadgetMessagePort);

            msg->port = dmp->port;

            send_message_to_dock(c, o, msgID, (struct GadgetMessage *)msg);
        }
    }
}

VOID send_register_port_msg_to_dock(Class *c, Object *o, struct DockMessagePort *dmp)
{
    send_port_msg_to_dock(c, o, dmp, GM_REGISTER_PORT);
}

VOID send_unregister_port_msg_to_dock(Class *c, Object *o, struct DockMessagePort *dmp)
{
    send_port_msg_to_dock(c, o, dmp, GM_UNREGISTER_PORT);
}

ULONG __saveds dock_gadget_dispatch(Class *c, Object *o, Msg msg)
{
	struct DockMessageGetSize* gs;
	struct DockMessageSetEnvironment* senv;
    struct DockMessageGetEnvironment* genv;
    struct DockMessageDraw* dm;
    struct DockMessageHitTest* ht;
    struct DockMessageAdded *am;
    struct DockMessageLaunch *dml;
	struct DockGadgetData *dgd;
    
	Object *no;

	switch( msg->MethodID ) 
	{
		case OM_NEW:	
			no = (Object *)DoSuperMethodA(c, o, msg);
			return (ULONG)no;

        case OM_DISPOSE:
            return DoSuperMethodA(c, o, msg);

		case DM_DRAW:
            dm = (struct DockMessageDraw *)msg;
			draw_default_image(c, o, dm->rp, INST_DATA(c, o));
			break;

        case DM_BUILTIN:
            return (ULONG)FALSE;
    
		case DM_SETENV:
			senv = (struct DockMessageSetEnvironment *)msg;			
			dgd = INST_DATA(c, o);
            dgd->env = *senv->env;
			break;

        case DM_GETENV:
            genv = (struct DockMessageGetEnvironment *)msg;
            dgd = INST_DATA(c, o);
            *genv->env = dgd->env;
            break;

		case DM_GETSIZE:
			gs = (struct DockMessageGetSize *)msg;
			gs->w = DEFAULT_SIZE;
			gs->h = DEFAULT_SIZE;
			break;

        case DM_HITTEST:
            ht = (struct DockMessageHitTest *)msg;
            dgd = INST_DATA(c, o);
            if( dgd->env.gadgetBounds.x <= ht->x && 
                ht->x < (dgd->env.gadgetBounds.x + dgd->env.gadgetBounds.w) &&
                dgd->env.gadgetBounds.y <= ht->y && 
                ht->y < (dgd->env.gadgetBounds.y + dgd->env.gadgetBounds.h) ) {
                return 1;
            }
            break;

        case DM_READCONFIG:
            read_settings(msg);
            break;

        case DM_ADDED:
            dgd = INST_DATA(c,o);
            am = (struct DockMessageAdded*)msg;
            dgd->dockPort = am->dockPort;
            break;

        case DM_REMOVED:
            dgd = INST_DATA(c,o);
            dgd->dockPort = NULL;
            break;

        case DM_REQ_QUIT:
            send_message_no_params(c, o, GM_QUIT);
            break;

        case DM_REQ_DRAW:
            send_message_no_params(c, o, GM_DRAW);
            break;         

        case DM_REQ_LAUNCH:
            dml = (struct DockMessageLaunch *)msg;
            send_launch_msg_to_dock(c, o, dml);
            break;

        case DM_CANEDIT:
            return FALSE;

        case DM_REG_PORT:
            send_register_port_msg_to_dock(c, o, (struct DockMessagePort *)msg);
            break;

        case DM_UNREG_PORT:
            send_unregister_port_msg_to_dock(c, o, (struct DockMessagePort *)msg);
            break;

        case DM_TICK:
        case DM_CLICK:
        case DM_DROP:
        case DM_HOTKEY:
        
        case DM_WRITECONFIG:
        case DM_INITBUTTON:
        
        case DM_GETEDITOR:
        case DM_EDITOREVENT:
        case DM_EDITORUPDATE:

        case DM_GETHOTKEY:
        case DM_GETLABEL:

        case DM_MESSAGE:
            break;

		default:
			return DoSuperMethodA(c, o, msg);
			
	}
	return NULL;
}

BOOL InitGadgetClass(struct DockBotLibrary *lib)
{
	ULONG HookEntry();
	Class *c;
	if( c = MakeClass(DB_ROOT_CLASS, ROOTCLASS, NULL, sizeof(struct DockGadgetData), 0) )
	{
        c->cl_Dispatcher.h_Entry = HookEntry;
		c->cl_Dispatcher.h_SubEntry = dock_gadget_dispatch;

        AddClass(c);

        lib->l_GadgetClass = c;
        return TRUE;
	}

	return FALSE;
}

VOID FreeGadgetClass(struct DockBotLibrary *lib)
{
    RemoveClass(lib->l_GadgetClass);

    FreeClass(lib->l_GadgetClass);
}


