/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/


#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <intuition/classes.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>

#include <stdio.h>

#include "dockbot.h"
#include "dockbot_protos.h"

#include "lib.h"

extern struct Library *IntuitionBase;
extern struct DockBotLibrary *DockBotBase;

struct LibNode
{
	struct MinNode n;
	struct Library *lib;
};

BOOL InitClassLibs(struct DockBotLibrary *lib) {

    NewList((struct List *)&lib->l_ClassLibs);

    return TRUE;
}

VOID FreeClassLibs(struct DockBotLibrary *lib) {
    
    struct LibNode *ln;

    while( ! IsListEmpty((struct List *)&lib->l_ClassLibs) ) {
        if( ln = (struct LibNode *)RemTail((struct List *)&lib->l_ClassLibs) ) {
            CloseLibrary(ln->lib);
            Forbid();
            RemLibrary(ln->lib);
            Permit();
            DB_FreeMem(ln, sizeof(struct LibNode));
        }
    }
}

UWORD ClassLibCount(struct DockBotLibrary *lib)
{
    UWORD count = 0;
    struct LibNode *ln;

    for( ln = (struct LibNode *)lib->l_ClassLibs.mlh_Head;
         ln->n.mln_Succ;
         ln = (struct LibNode *)ln->n.mln_Succ ) {

        count++;
    }
    return count;
}

Object * __asm __saveds DB_CreateDockGadget(
                            register __a0 STRPTR name)
{
    Object *o;
    char libName[50];
    struct Library *lib;
    struct LibNode *ln = NULL;

    if( ! (o = NewObjectA(NULL, name, TAG_DONE) ) ) {

        sprintf(libName, "PROGDIR:Gadgets/%s.class", name);

        if( lib = OpenLibrary(libName, 1) ) {

            if( ln = (struct LibNode *)DB_AllocMem(sizeof(struct LibNode), MEMF_CLEAR) ) {

                if( o = NewObjectA(NULL, name, TAG_DONE) ) {

                    ln->lib = lib;
                    AddTail((struct List*)&DockBotBase->l_ClassLibs, (struct Node *)ln);

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



