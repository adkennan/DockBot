/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <clib/commodities_protos.h>

BOOL create_cx_broker(struct DockWindow *dock)
{
    struct NewBroker nb = {
        NB_VERSION,
        APP_NAME,
        APP_NAME,
        0,
        NBU_UNIQUE, COF_SHOW_HIDE, 0, 0, 0
    };

    nb.nb_Descr = (STRPTR)MSG_CX_Description;

    if( dock->cxPort = CreateMsgPort() ) {

        nb.nb_Port = dock->cxPort;

        if( dock->cxBroker = CxBroker(&nb, NULL) ) {

            return TRUE;   
        }

    }

    return FALSE;
}

VOID free_cx_broker(struct DockWindow *dock) 
{
    DEBUG(printf("free_cx_broker\n"));

    if( dock->cxBroker ) {

        DeleteCxObjAll(dock->cxBroker);
        dock->cxBroker = NULL;
    }

    if( dock->cxPort ) {
        delete_port(dock->cxPort);
        dock->cxPort = NULL;
    }
}

BOOL init_cx_broker(struct DockWindow *dock)
{
    struct DgNode *curr;
    STRPTR hotKey;
    CxObj *filter, *sender, *translate;
    UWORD msgId;    
    
    DEBUG(printf("init_cx_broker\n"));

    if( dock->cxBroker ) {
        free_cx_broker(dock);
    } 
    
    if( create_cx_broker(dock) ) {

        msgId = 0;

        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            dock_gadget_get_hotkey(curr->dg, &hotKey);
            if( hotKey ) {
                if( filter = CxFilter(hotKey) ) {
                                    
                    AttachCxObj(dock->cxBroker, filter);

                    if( sender = CxSender(dock->cxPort, msgId) ) {

                        AttachCxObj(filter, sender);

                        if( translate = CxTranslate(NULL) ) {

                            AttachCxObj(filter, translate);
    
                            if( CxObjError(filter) ) {
                                return FALSE;
                            }
                        } else {
                            return FALSE;
                        }
                    } else {
                        return FALSE;
                    }

                } else {
                    return FALSE;
                }
            }
            msgId++;
        }

        ActivateCxObj(dock->cxBroker, 1L);

        return TRUE;
    }

    return FALSE;
}


VOID handle_cx_message(struct DockWindow *dock)
{
    CxMsg *msg;
    ULONG msgType, msgId, ix;
    struct DgNode *curr;

    while( msg = (CxMsg*)GetMsg(dock->cxPort) ) {

        msgType = CxMsgType(msg);
        msgId = CxMsgID(msg);

        ReplyMsg((struct Message *)msg);

        switch(msgType) {
            
            case CXM_COMMAND:
                switch( msgId ) {
                    case CXCMD_DISABLE:
                        if( dock->cxBroker ) {
                            ActivateCxObj(dock->cxBroker, 0L);
                        }
                        break;
            
                    case CXCMD_ENABLE:
                        if( dock->cxBroker ) {
                            ActivateCxObj(dock->cxBroker, 1L);    
                        }
                        break;

                    case CXCMD_KILL:
                        dock->runState = RS_QUITTING;
                        break;

                    case CXCMD_APPEAR:
                        if( ! dock->win ) {
                            dock->runState = RS_UNICONIFYING;
                        }
                        break;

                    case CXCMD_DISAPPEAR:
                        if( dock->win ) {
                            dock->runState = RS_ICONIFYING;
                        }
                        break;
                }
                break;

            case CXM_IEVENT:
                ix = 0;

                FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

                    if( ix == msgId ) {
                        dock_gadget_hotkey(curr->dg);
                        break;
                    }
                    ix++;
                }
                break;
        }
    }
}
