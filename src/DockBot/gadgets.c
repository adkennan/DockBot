/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include "dock.h"

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/layers_protos.h>
#include <clib/graphics_protos.h>

#include "dock_gadget.h"
#include "dock_handle.h"
#include "dock_button.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

BOOL create_dock_handle(struct DockWindow *dock)
{
    Object *gad;
    
    if( gad = NewObjectA(dock->handleClass, NULL, TAG_DONE) ) {

        add_dock_gadget(dock, gad);

        return TRUE;
    }

    return FALSE;
}

BOOL init_gadget_classes(struct DockWindow *dock)
{
    if( dock->gadgetPort = CreateMsgPort() ) {
        if( dock->gadgetClass = init_dock_gadget_class() ) {
            if( dock->handleClass = init_dock_handle_class() ) {
                if( dock->buttonClass = init_dock_button_class() ) {
                    return TRUE;
                }
            }
        }   
    }
    return FALSE;
}

BOOL init_gadgets(struct DockWindow *dock)
{
    NewList((struct List *)&(dock->gadgets));
    NewList((struct List *)&(dock->libs));

    if( create_dock_handle(dock) ) {
        if( load_config(dock) ) {
            return TRUE;
        }
    }
    return FALSE; 
}


VOID remove_dock_gadgets(struct DockWindow *dock)
{
    struct DgNode *dg;

    disable_layout(dock);

    while( ! IsListEmpty((struct List *)&dock->gadgets) ) {
        if( dg = (struct DgNode *)RemTail((struct List *)&dock->gadgets) ) {
            dock_gadget_removed(dg->dg);
            DisposeObject(dg->dg);
            DB_FreeMem(dg, sizeof(struct DgNode));
        }
    }
}


VOID draw_gadgets(struct DockWindow *dock)
{
    struct Window *win = dock->win;
    struct RastPort *rp = win->RPort;
    struct DgNode *curr;

    LockLayer(NULL, win->WLayer);
    
    SetAPen(rp, 0);
    RectFill(rp, 0, 0, win->Width, win->Height);

    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                curr->n.mln_Succ; 
                curr = (struct DgNode *)curr->n.mln_Succ ) {

        dock_gadget_draw(curr->dg, rp);
    }

    UnlockLayer(win->WLayer);
}


VOID draw_gadget(struct DockWindow *dock, Object *gadget)
{
    struct Window *win = dock->win;
    struct RastPort *rp = win->RPort;
    struct Rect gb;

    DB_GetDockGadgetBounds(gadget, &gb);

    LockLayer(NULL, win->WLayer);

    SetAPen(rp, 0);
    RectFill(rp, gb.x, gb.y, gb.w, gb.h);
    
    dock_gadget_draw(gadget, rp);

    UnlockLayer(win->WLayer);
}


VOID add_dock_gadget(struct DockWindow *dock, Object *dg)
{
    struct DgNode *n;
    if( n = DB_AllocMem(sizeof(struct DgNode), MEMF_CLEAR) ) {
        n->dg = dg;
        AddTail((struct List *)&(dock->gadgets), (struct Node *)n);

        dock_gadget_added(dg, dock->gadgetPort);
    }

    layout_gadgets(dock);
}

VOID remove_dock_gadget(struct DockWindow *dock, Object *dg)
{
    struct DgNode *curr;
    for( curr = (struct DgNode *)dock->gadgets.mlh_Head; 
                curr->n.mln_Succ; 
                curr = (struct DgNode *)curr->n.mln_Succ ) {
        if( curr->dg == dg ) {
            dock_gadget_removed(curr->dg);
            DisposeObject(curr->dg);
            
            Remove((struct Node *)curr);
            DB_FreeMem(curr, sizeof(struct DgNode));
            break;
        }     
    } 

    layout_gadgets(dock);
}

Object *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y)
{
    struct DgNode *curr;

    for( curr = (struct DgNode *)dock->gadgets.mlh_Head;
         curr->n.mln_Succ;
         curr = (struct DgNode *)curr->n.mln_Succ ) {
                        
        if( dock_gadget_hit_test(curr->dg, x, y) ) {

            return curr->dg;
        }
    }
    return NULL;    
}

VOID close_class_libs(struct DockWindow *dock)
{
    struct LibNode *ln;

    while( ! IsListEmpty((struct List *)&dock->libs) ) {
        if( ln = (struct LibNode *)RemTail((struct List *)&dock->libs) ) {
            CloseLibrary(ln->lib);
            DB_FreeMem(ln, sizeof(struct LibNode));
        }
    }
}

Object *create_dock_gadget(struct DockWindow *dock, STRPTR name)
{
    Object *o;
    char libName[50];
    struct Library *lib;
    UWORD len;
    struct LibNode *ln = NULL;
    STRPTR suffix = ".class";

    if( ! (o = NewObjectA(NULL, name, TAG_DONE) ) ) {

        len = strlen(name);
        CopyMem(name, &libName, len);
        CopyMem(suffix, &libName[len], strlen(suffix));

        if( lib = OpenLibrary(libName, 1) ) {

            if( ln = (struct LibNode *)DB_AllocMem(sizeof(struct LibNode), MEMF_CLEAR) ) {

                if( o = NewObjectA(NULL, name, TAG_DONE) ) {

                    ln->lib = lib;
                    AddTail((struct List*)&(dock->libs), (struct Node *)ln);

                    return o;
                }           
            }
        }
        if( o ) {
            DisposeObject(o);
        }
        if( lib ) {
            CloseLibrary(lib);
        }
        if( ln ) {
            DB_FreeMem(ln, sizeof(struct LibNode));
        }
    }

    return o;
}

