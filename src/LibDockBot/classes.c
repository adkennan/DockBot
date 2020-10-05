/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/


#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <intuition/classes.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>

#include <dos/exall.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>

#include <stdio.h>

#include "dockbot.h"
#include "dockbot_protos.h"

#include "lib.h"

#define DIR_BUF_SIZE 512

#define GADGET_PATH "PROGDIR:Gadgets"
#define GADGET_EXT ".class"
#define GADGET_PAT "#?.CLASS"

extern struct Library *DOSBase;
extern struct Library *IntuitionBase;
extern struct DockBotLibrary *DockBotBaseFull;

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

        sprintf(libName, GADGET_PATH "/%s.class", name);

        if( lib = OpenLibrary(libName, 1) ) {

            if( ln = (struct LibNode *)DB_AllocMem(sizeof(struct LibNode), MEMF_CLEAR) ) {

                if( o = NewObjectA(NULL, name, TAG_DONE) ) {

                    ln->lib = lib;
                    AddTail((struct List*)&DockBotBaseFull->l_ClassLibs, (struct Node *)ln);

                    return o;
                } else {
                    DEBUG(DebugLog("Failed to construct class %s\n", name));
                }
            } else {
                DEBUG(DebugLog("Failed to alloc %ld bytes for lib node\n", sizeof(struct LibNode)));
            }
        } else {
            DEBUG(DebugLog("Failed to open library %s\n", libName));
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

VOID __asm __saveds DB_DisposeDockGadget(
    register __a0 Object *o)
{
    if( o ) {
        DisposeObject(o);
    }
}

BOOL __asm __saveds DB_ListClasses(
    register __a0 struct List *list)
{
    BPTR lock;
    struct ExAllData *buf, *ead;    
    struct ExAllControl *eac;
    BOOL more;
    struct Node *node;
    UWORD len;
    STRPTR name;
    BOOL result = FALSE;
    UBYTE *pattern;
    LONG parseLen = strlen(GADGET_PAT) * 3;

    if( pattern = DB_AllocMem(parseLen, MEMF_CLEAR) ) {

        if( ParsePatternNoCase(GADGET_PAT, pattern, parseLen) != -1 ) {

            if( lock = Lock(GADGET_PATH, SHARED_LOCK) ) {
    
                if( buf = (struct ExAllData *)DB_AllocMem(DIR_BUF_SIZE, MEMF_CLEAR) ) {
            
                    if( eac = AllocDosObject(DOS_EXALLCONTROL, NULL)) {
                
                        eac->eac_MatchString = pattern;
                        eac->eac_LastKey = 0;
    
                        do {

                            more = ExAll(lock, buf, DIR_BUF_SIZE, ED_NAME, eac);

                            if( !more && IoErr() != ERROR_NO_MORE_ENTRIES ) {
                                result = FALSE;
                                break;
                            } 
                    
                            result = TRUE;
  
                            if( eac->eac_Entries == 0 ) {
                                continue;
                            }

                            ead = buf;
                            do {

                                len = strlen(ead->ed_Name) - strlen(GADGET_EXT);
          
                                if( node = (struct Node *)DB_AllocMem(sizeof(struct Node) + len + 1, MEMF_CLEAR) ) {
                            
                                    name = (STRPTR)((UBYTE *)node + sizeof(struct Node));

                                    CopyMem(ead->ed_Name, name, len);
//                                    name[len + 1] = '\0';

                                    node->ln_Name = name;

                                    AddTail(list, node);
                                }
    
                                ead = ead->ed_Next;

                            } while(ead);
                    
                        } while( more );

                        FreeDosObject(DOS_EXALLCONTROL, eac);
                    }
                    DB_FreeMem(buf, DIR_BUF_SIZE);
                }
        
                UnLock(lock);
            }
        }
        DB_FreeMem(pattern, parseLen);
    }
    
    return result;
}


