/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "dock.h"

#include <exec/memory.h>
#include <exec/ports.h>
#include <dos/dostags.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/layers_protos.h>
#include <clib/graphics_protos.h>
#include <devices/timer.h>

#include "dock_handle.h"

VOID show_about(struct DockWindow *dock)
{
    STRPTR msg;
    STRPTR p;
    STRPTR name;
    STRPTR version;
    STRPTR desc;
    STRPTR copy;
    ULONG msgLen, l;
    struct DgNode *curr;
    ClassID lastClass = 0, currClass;
    struct EasyStruct es = {
            sizeof(struct EasyStruct),
            0,
            0 /* MSG_ABOUT_Title */,
            0 /* MSG_ABOUT_Text */,
            0 /* MSG_OK */
    };

    es.es_Title = (STRPTR)MSG_ABOUT_Title;
    es.es_TextFormat = (STRPTR)MSG_ABOUT_Text;
    es.es_GadgetFormat = (STRPTR)MSG_OK;    
 
    msgLen = 0;
    FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

        if( ! dock_gadget_builtin(curr->dg) ) {

            currClass = OCLASS(curr->dg)->cl_ID;
            if( currClass == lastClass ) {
                continue;
            }
            lastClass = currClass;

            dock_gadget_get_info(curr->dg, &name, &version, &desc, &copy);

            msgLen += strlen(name) + strlen(version) + strlen(desc) + strlen(copy) + 8;
        }
    } 

    if( msg = (STRPTR)DB_AllocMem(msgLen, MEMF_CLEAR) ) {

        p = msg;
        lastClass = 0;
    
        FOR_EACH_GADGET(&dock->cfg.gadgets, curr) {

            if( ! dock_gadget_builtin(curr->dg) ) {

                currClass = OCLASS(curr->dg)->cl_ID;
                if( currClass == lastClass ) {
                    continue;
                }
                lastClass = currClass;


                dock_gadget_get_info(curr->dg, &name, &version, &desc, &copy);

                l = strlen(name);
                CopyMem(name, p, l);
                p += l;
                *(p++) = ' ';
                l = strlen(version);
                CopyMem(version, p, l);
                p += l;
                *(p++) = '\n';
                l = strlen(desc);
                CopyMem(desc, p, l);
                p += l;
                *(p++) = '\n';
                l = strlen(copy);
                CopyMem(copy, p, l);
                p += l;
                *(p++) = '\n';
                *(p++) = '\n';
            }
        } 
        *p = '\0';
    
        EasyRequest(NULL, &es, NULL, VERSION_STR, msg);


        DB_FreeMem(msg, msgLen);
    }
}

VOID delete_port(struct MsgPort *port) {
    struct Message *msg;

    if( port ) {
        while( msg = GetMsg(port) ) {
            ReplyMsg(msg);
        }

        if( port->mp_Node.ln_Name ) {
            DeletePort(port);
        } else {
            DeleteMsgPort(port);
        }
    }
}


BOOL get_prog_path(struct DockWindow *dock)
{
    BPTR lock;
    BOOL result = FALSE;
            
    if( lock = Lock("PROGDIR:", ACCESS_READ) ) {

        result = NameFromLock(lock, (STRPTR)&dock->progPath, 2048);
    
        UnLock(lock);

    }

    return result;
}

struct DockWindow* create_dock(VOID)
{
	struct DockWindow *dock;

    DEBUG(printf("create_dock\n"));

    if( dock = (struct DockWindow *)DB_AllocMem(sizeof(struct DockWindow), MEMF_CLEAR) ) {

        dock->runState = RS_STARTING;
        dock->disableLayout = TRUE;
        dock->cfg.align = DA_CENTER;
        dock->cfg.pos = DP_RIGHT;
        dock->cfg.showGadgetLabels = TRUE;
        dock->cfg.showGadgetBorders = TRUE;

        InitRastPort(&dock->renderRP);

        if( dock->pubPort = CreatePort(APP_NAME, 0L) ) {

            if( init_gadget_classes(dock) ) {

                if( init_gadgets(dock) ) {    

                    if( init_config_notification(dock) ) {
                        
                        if( init_timer_notification(dock) ) {

                            if( init_screennotify(dock) ) {

                                if( get_prog_path(dock) ) {
    
                                    return dock;
                                }
                            }
                        }        
                    }
                }
            }
        }    

        // Uh oh!    
        free_dock(dock);
    }

    return NULL;
}

VOID free_dock(struct DockWindow* dock)
{
    DEBUG(printf("free_dock\n"));

    free_screennotify(dock);

    free_cx_broker(dock);

    hide_dock_window(dock);

    remove_dock_gadgets(dock);

    free_config_notification(dock);

    free_gadget_classes(dock);

    free_timer_notification(dock);

    free_app_icon(dock);

    delete_port(dock->pubPort);

    free_icon_brushes(dock);

    free_config(dock);

    free_render_bitmap(dock);

	DB_FreeMem(dock, sizeof(struct DockWindow));

    DEBUG(printf("Done\n"));
}


VOID execute_external(struct DockWindow* dock, STRPTR path, STRPTR args, STRPTR console, BOOL wb)
{
    STRPTR cmd;
    STRPTR pos;
    STRPTR con;
    BPTR fhIn;
    BPTR fhOut;
    UWORD len = 0, l;

    struct TagItem shellTags[] = {
        { SYS_UserShell, TRUE },
        { SYS_Asynch, TRUE },
        { SYS_Input, NULL },
        { SYS_Output, NULL },
        { TAG_DONE, 0 }
    };


    if( wb ) {
        len = strlen((STRPTR)&dock->progPath) + 7;
    }   
    
    len += strlen(path) + 1;
    if( args ) {
        len += strlen(args) + 1;
    }

    if( cmd = (STRPTR)DB_AllocMem(len + 1, MEMF_CLEAR) ) {
        
        pos = cmd;
        if( wb ) {
            COPY_STRING((STRPTR)&dock->progPath, pos);
            pos--;
            COPY_STRING("/WBRUN ", pos);
            pos--;
        }
        
        COPY_STRING(path, pos);
                
        if( args ) {
            COPY_STRING(args, pos);
        }

        pos--;
        *pos = '\0';

        con = console;
        if( con == NULL ) {
            con = DEFAULT_CONSOLE;
        }

        if( fhOut = Open(con, MODE_OLDFILE) ) {
            if( fhIn = Open(DEFAULT_CONSOLE, MODE_OLDFILE) ) {

                shellTags[2].ti_Data = fhIn;
                shellTags[3].ti_Data = fhOut;
                
                DEBUG(printf(__FUNC__ ": cmd = >%s<\n", cmd));             

                if( SystemTagList(cmd, (struct TagItem*)&shellTags) == -1 ) {
                    Close(fhIn);
                    Close(fhOut);
                }

            } else {
                Close(fhOut);
            }
        }
        DB_FreeMem(cmd, len + 1);
    }
}

VOID open_help(struct DockWindow *dock)
{
    UWORD l;
    UBYTE arg[256];
    STRPTR pos = (STRPTR)&arg;

    COPY_STRING((STRPTR)dock->progPath, pos);
    pos--;
    COPY_STRING("/doc/DockBot.guide", pos);
    pos--;
    *pos = '\0';

    execute_external(dock, "SYS:Utilities/Multiview", (STRPTR)&arg, NULL, TRUE);
}
