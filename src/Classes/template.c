
#include <utility/hooks.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>

#include <proto/exec.h>
#include <proto/intuition.h>

#include "dockbot.h"

#include "class_def.h"


/**** Main Library Structure ****/

struct ClassLibrary {
    struct Library          cl_Lib;
    UWORD                   cl_Pad;
    BPTR                    cl_SegList;
    struct Library          *cl_ExecBase;
    struct Library          *cl_IntuitionBase;
    struct Library          *cl_DockBotBase;
    Class                   *cl_GadgetClass;
#ifdef GADGET_LIB_DATA
    struct GADGET_LIB_DATA  cl_Data;
#endif
};

/**** Initialization Structs ****/
struct InitTable;
struct InitDataTable;


/**** Library Init ****/
struct ClassLibrary * __saveds __asm InitLib(
    register __a6 struct Library *sysBase,
    register __a0 BPTR segList,
    register __d0 struct ClassLibrary *cb);


/**** Library Functions ****/
struct ClassLibrary * __saveds __asm _LibOpen(
    register __a6 struct ClassLibrary *ClassLibraryBase);
    
BPTR __saveds __asm _LibClose(
    register __a6 struct ClassLibrary *ClassLibraryBase);

BPTR __saveds __asm _LibExpunge(
    register __a6 struct ClassLibrary *ClassLibraryBase);

ULONG __saveds __asm ExtFuncLib(
    register __a6 struct ClassLibrary *ClassLibraryBase);

Class * __saveds __asm _GetEngine(
    register __a6 struct ClassLibrary *ClassLibraryBase);


/**** Utility Functions ****/

VOID __saveds CloseLibs(VOID);

VOID __saveds FreeLib(struct ClassLibrary *cb);

Class* __saveds InitClass(VOID);

VOID __saveds CleanUpClass(struct ClassLibrary *cb);

/**** Class library constructor/destructor ****/
#ifdef GADGET_LIB_INIT
ULONG GADGET_LIB_INIT(struct GADGET_LIB_DATA *gld);
#endif

#ifdef GADGET_LIB_EXPUNGE
ULONG GADGET_LIB_EXPUNGE(struct GADGET_LIB_DATA *gld);
#endif


/**** Prevent Execution ****/

LONG __asm LibStart(VOID)
{
    return -1;
} 

/**** Variables *****/

extern APTR FuncTable[];
extern struct InitDataTable DataTable;
extern struct InitTable InitTab;
extern APTR EndResident;
extern char ClassLibName[];
extern char ClassLibId[];


struct Resident __aligned _LibRomTag = {
    RTC_MATCHWORD,
    &_LibRomTag,
    &EndResident,
    RTF_AUTOINIT,
    CLASS_VERSION,
    NT_LIBRARY,
    0,
    &ClassLibName[0],
    &ClassLibId[0],
    &InitTab
};

APTR EndResident;


char __aligned ClassLibName[]   = CLASS_NAME ".class";
char __aligned ClassLibId[]     = CLASS_NAME " " CLASS_VER_STR; 


struct InitDataTable {

    UWORD ln_Type_i;        UWORD ln_Type_o;        UWORD ln_Type_c;
    UBYTE ln_Name_i;        UBYTE ln_Name_o;        ULONG ln_Name_c;
    UWORD lib_Flags_i;      UWORD lib_Flags_o;      UWORD lib_Flags_c;
    UWORD lib_Version_i;    UWORD lib_Version_o;    UWORD lib_Version_c;
    UWORD lib_Revision_i;   UWORD lib_Revision_o;   UWORD lib_Revision_c;
    UWORD lib_IdString_i;   UBYTE lib_IdString_o;   ULONG lib_IdString_c;
    ULONG ENDMARK;
    
} DataTable = {
    INITBYTE(OFFSET(Node,   ln_Type),               NT_LIBRARY),
    0x80, OFFSET(Node,      ln_Name),               (ULONG)&ClassLibName[0],
    INITBYTE(OFFSET(Library, lib_Flags),            LIBF_SUMUSED | LIBF_CHANGED),
    INITWORD(OFFSET(Library, lib_Version),          CLASS_VERSION),
    INITWORD(OFFSET(Library, lib_Revision),         CLASS_REVISION),
    0x80, OFFSET(Library,   lib_IdString),          (ULONG)&ClassLibId[0],
    (ULONG)0
};

struct InitTable {
    ULONG   LibBaseSize;
    APTR   *FuncTable;
    struct  InitDataTable *DataTable;
    APTR    InitLibTable;
} InitTab = {
    sizeof(struct ClassLibrary),
    (APTR*)&FuncTable[0],
    (struct InitDataTable *)&DataTable,
    (APTR)InitLib
};


APTR FuncTable[] = {
    _LibOpen,
    _LibClose,
    _LibExpunge,
    ExtFuncLib,

    _GetEngine,
    (APTR)-1
};

struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
struct Library *DockBotBase;

struct ClassLibrary * __saveds __asm InitLib(
    register __a6 struct Library *sysBase,
    register __a0 BPTR segList,
    register __d0 struct ClassLibrary *cb)
{
    SysBase = (struct ExecBase *)sysBase;

    cb->cl_ExecBase = sysBase;
    cb->cl_SegList = segList;

    if( IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39) ) {
        if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {
            if( cb->cl_GadgetClass = InitClass() ) {

#ifdef GADGET_LIB_INIT
                if( GADGET_LIB_INIT(&cb->cl_Data) ) {
#endif
                    cb->cl_IntuitionBase = (struct Library *)IntuitionBase;
                    cb->cl_DockBotBase = DockBotBase;

                    return cb;

#ifdef GADGET_LIB_INIT
                }
#endif
            }
        }
    }

    CleanUpClass(cb);   
    CloseLibs();
    FreeLib(cb);
    return NULL;
}

struct ClassLibrary * __saveds __asm _LibOpen(
    register __a6 struct ClassLibrary *ClassLibraryBase)
{
    ClassLibraryBase->cl_Lib.lib_OpenCnt++;
    ClassLibraryBase->cl_Lib.lib_Flags &= ~LIBF_DELEXP;

    return ClassLibraryBase;
}
    
BPTR __saveds __asm _LibClose(
    register __a6 struct ClassLibrary *ClassLibraryBase)
{
    ClassLibraryBase->cl_Lib.lib_OpenCnt--;
    
    if( ClassLibraryBase->cl_Lib.lib_OpenCnt == 0 ) {
        if( ClassLibraryBase->cl_Lib.lib_Flags & LIBF_DELEXP ) {
            return _LibExpunge(ClassLibraryBase);
        }
    }
    return NULL;
}

BPTR __saveds __asm _LibExpunge(
    register __a6 struct ClassLibrary *ClassLibraryBase)
{
    BPTR segList;

    if( ClassLibraryBase->cl_Lib.lib_OpenCnt == 0 ) {
        
#ifdef GADGET_LIB_EXPUNGE
        GADGET_LIB_EXPUNGE(&ClassLibraryBase->cl_Data);
#endif

        segList = ClassLibraryBase->cl_SegList;

        Remove((struct Node*)ClassLibraryBase);

        CleanUpClass(ClassLibraryBase);
        CloseLibs();
        FreeLib(ClassLibraryBase);

        return segList;
    }

    ClassLibraryBase->cl_Lib.lib_Flags |= LIBF_DELEXP;

    return NULL;
}

ULONG __saveds __asm ExtFuncLib(
    register __a6 struct ClassLibrary *ClassLibraryBase)
{
    return NULL;
}


Class* __saveds __asm _GetEngine(
    register __a6 struct ClassLibrary *ClassLibraryBase) 
{
    return ClassLibraryBase->cl_GadgetClass;
}


#ifdef METHOD_NEW
    ULONG __saveds METHOD_NEW (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_DISPOSE
    ULONG __saveds METHOD_DISPOSE (Class *c, Object *o, Msg msg);
#endif


#ifdef METHOD_DRAW
    ULONG __saveds METHOD_DRAW (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_CLICK
    ULONG __saveds METHOD_CLICK (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_DROP
    ULONG __saveds METHOD_DROP (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_TICK
    ULONG __saveds METHOD_TICK (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_GETSIZE
    ULONG __saveds METHOD_GETSIZE (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_READCONFIG
    ULONG __saveds METHOD_READCONFIG (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_WRITECONFIG
    ULONG __saveds METHOD_WRITECONFIG (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_GETSETTINGS
    ULONG __saveds METHOD_GETSETTINGS (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_GETHOTKEY
    ULONG __saveds METHOD_GETHOTKEY (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_HOTKEY
    ULONG __saveds METHOD_HOTKEY (Class *c, Object *o, Msg msg);
#endif

#ifdef METHOD_GETLABEL
    ULONG __saveds METHOD_GETLABEL (Class *c, Object *o, Msg msg);
#endif


ULONG __saveds GetGadgetInfo(Class *c, Object *o, Msg msg)
{
    struct DockMessageGetInfo *i = (struct DockMessageGetInfo *)msg;
    
    i->name = CLASS_NAME;
    i->description = CLASS_DESC;
    i->version = CLASS_VER_STR;
    i->copyright = CLASS_COPYRIGHT;

    return TRUE;
}

ULONG __saveds GadgetDispatch(Class *c, Object *o, Msg msg)
{
#ifdef METHOD_NEW
    Object *newObj;
#endif

    switch( msg->MethodID )
    {

#ifdef METHOD_NEW
        case OM_NEW:
            if( newObj = (Object *)DoSuperMethodA(c, o, msg) ) {
                if( METHOD_NEW(c, newObj, msg) ) {
                    return (ULONG)newObj;
                }
            }
            break;
#endif

#ifdef METHOD_DISPOSE
        case OM_DISPOSE:
            if( METHOD_DISPOSE(c, o, msg) ) {
                return DoSuperMethodA(c, o, msg);
            }
            break;
#endif

#ifdef METHOD_ADDED
        case DM_ADDED:
            return METHOD_ADDED (c, o, msg);
#endif

#ifdef METHOD_REMOVED
        case DM_REMOVED:
            return METHOD_REMOVED (c, o, msg);
#endif

#ifdef METHOD_DRAW
        case DM_DRAW:
            return METHOD_DRAW (c, o, msg);
#endif
            
#ifdef METHOD_TICK
        case DM_TICK:
            return METHOD_TICK (c, o, msg);
#endif

#ifdef METHOD_CLICK
        case DM_CLICK:
            return METHOD_CLICK (c, o, msg);
#endif

#ifdef METHOD_DROP
        case DM_DROP:
            return METHOD_DROP (c, o, msg);
#endif

#ifdef METHOD_GETSIZE
        case DM_GETSIZE:
            return METHOD_GETSIZE (c, o, msg);
#endif

#ifdef METHOD_READCONFIG
        case DM_READCONFIG:
            return METHOD_READCONFIG (c, o, msg);
#endif

#ifdef METHOD_WRITECONFIG
        case DM_WRITECONFIG:
            return METHOD_WRITECONFIG (c, o, msg);
#endif

#ifdef METHOD_GETSETTINGS
        case DM_GETSETTINGS:
            return METHOD_GETSETTINGS (c, o, msg);
#endif

#ifdef METHOD_GETHOTKEY
        case DM_GETHOTKEY:
            return METHOD_GETHOTKEY (c, o, msg);
#endif

#ifdef METHOD_HOTKEY
        case DM_HOTKEY:
            return METHOD_HOTKEY (c, o, msg);
#endif

#ifdef METHOD_GETLABEL
        case DM_GETLABEL:
            return METHOD_GETLABEL (c, o, msg);
#endif

        case DM_GETINFO:
            return GetGadgetInfo(c, o, msg);

        default:
            return DoSuperMethodA(c, o, msg);
    }
    return NULL;
}


Class __saveds *InitClass(VOID)
{
    ULONG HookEntry();
    Class *c;

    if( c = MakeClass(CLASS_NAME, PARENT_CLASS, NULL, sizeof(struct PRIVATE_DATA), 0) )
    {
        c->cl_Dispatcher.h_Entry = HookEntry; //(ULONG (*) ())GadgetDispatch;
        c->cl_Dispatcher.h_SubEntry = GadgetDispatch;

        AddClass(c);
    }

    return c;
}

VOID __saveds CleanUpClass(struct ClassLibrary *cb)

{
    if( cb->cl_GadgetClass ) {
        RemoveClass(cb->cl_GadgetClass);
        FreeClass(cb->cl_GadgetClass);
    }
}


VOID __saveds CloseLibs(VOID)
{
    if( DockBotBase ) {
        CloseLibrary(DockBotBase);
    }

    if( IntuitionBase ) {
        CloseLibrary((struct Library *)IntuitionBase);
    }
}

VOID __saveds FreeLib(struct ClassLibrary *cb)
{
    ULONG neg, pos, full;
    UBYTE* negPtr = (UBYTE*)cb;

    neg = cb->cl_Lib.lib_NegSize;
    pos = cb->cl_Lib.lib_PosSize;
    full = neg + pos;
    negPtr -= neg;

    FreeMem(negPtr, full);
}


VOID *_XCEXIT = NULL;

