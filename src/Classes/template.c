/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

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

#include <libraries/locale.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/locale.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

#include <libraries/triton.h>

/**** Main Library Structure ****/

struct ClassLibrary {
    struct Library          cl_Lib;
    UWORD                   cl_Pad;
    BPTR                    cl_SegList;
    Class                   *cl_GadgetClass;
    struct Library          *cl_ExecBase;
    struct Library          *cl_IntuitionBase;
    struct Library          *cl_DockBotBase;
    struct Library          *cl_TritonBase;
    struct Library          *cl_LocaleBase;
    struct Catalog          *cl_Catalog;
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

Class * __saveds __asm GetEngine(
    register __a6 struct ClassLibrary *ClassLibraryBase);

VOID __saveds __asm SetCatalog(
    register __a6 struct ClassLibrary *ClassLibraryBase,
    register __a0 struct Catalog *catalog);

/**** Utility Functions ****/

VOID __saveds CloseLibs(VOID);

VOID __saveds FreeLib(struct ClassLibrary *cb);

Class* __saveds InitClass(VOID);

VOID __saveds CleanUpClass(struct ClassLibrary *cb);

VOID InitCatalog(struct Catalog *catalog);

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

    GetEngine,
    (APTR)-1
};

struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
struct Library *DockBotBase;
extern struct Library *TritonBase;
struct ClassLibrary *ClassLib;
struct Library *LocaleBase;

struct ClassLibrary * __saveds __asm InitLib(
    register __a6 struct Library *sysBase,
    register __a0 BPTR segList,
    register __d0 struct ClassLibrary *cb)
{
    SysBase = (struct ExecBase *)sysBase;

    cb->cl_ExecBase = sysBase;
    cb->cl_SegList = segList;

    if( IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39) ) {
        if( LocaleBase = OpenLibrary("locale.library", 37) ) {
            if( DockBotBase = OpenLibrary("dockbot.library", 1) ) {
                if( cb->cl_GadgetClass = InitClass() ) {

                    if( cb->cl_Catalog = OpenCatalog(NULL, CLASS_NAME ".catalog"
                                            , OC_BuiltInLanguage, "english"
              	    					    , OC_Version, 0
								            , TAG_DONE) ) {

                        InitCatalog(cb->cl_Catalog);            
                    }

#ifdef GADGET_LIB_INIT
                    if( GADGET_LIB_INIT(&cb->cl_Data) ) {
#endif
                        cb->cl_IntuitionBase = (struct Library *)IntuitionBase;
                        cb->cl_DockBotBase = DockBotBase;
                        cb->cl_TritonBase = NULL;
                        cb->cl_LocaleBase = LocaleBase;
                        TritonBase = NULL;
        
                        ClassLib = cb;

                        return cb;
                        
#ifdef GADGET_LIB_INIT
                    }
#endif
                }
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


Class* __saveds __asm GetEngine(
    register __a6 struct ClassLibrary *ClassLibraryBase) 
{
    return ClassLibraryBase->cl_GadgetClass;
}

#define METHOD_DEF(NAME) ULONG __saveds METHOD_ ## NAME (Class *c, Object *o, Msg msg);

#ifdef METHOD_NEW
    METHOD_DEF(NEW)
#endif

#ifdef METHOD_DISPOSE
    METHOD_DEF(DISPOSE)
#endif

#ifdef METHOD_DRAW
    METHOD_DEF(DRAW)
#endif

#ifdef METHOD_CLICK
    METHOD_DEF(CLICK)
#endif

#ifdef METHOD_DROP
    METHOD_DEF(DROP)
#endif

#ifdef METHOD_TICK
    METHOD_DEF(TICK)
#endif

#ifdef METHOD_LAUNCHED
    METHOD_DEF(LAUNCHED)
#endif    

#ifdef METHOD_GETSIZE
    METHOD_DEF(GETSIZE)
#endif

#ifdef METHOD_READCONFIG
    METHOD_DEF(READCONFIG)
#endif

#ifdef METHOD_WRITECONFIG
    METHOD_DEF(WRITECONFIG)
#endif

#ifdef METHOD_GETEDITOR
    METHOD_DEF(GETEDITOR)
#endif

#ifdef METHOD_EDITOREVENT
    METHOD_DEF(EDITOREVENT)
#endif

#ifdef METHOD_EDITORUPDATE
    METHOD_DEF(EDITORUPDATE)
#endif

#ifdef METHOD_GETHOTKEY
    METHOD_DEF(GETHOTKEY)
#endif

#ifdef METHOD_HOTKEY
    METHOD_DEF(HOTKEY)
#endif

#ifdef METHOD_INITBUTTON
    METHOD_DEF(INITBUTTON)
#endif

#ifdef METHOD_REMAP
    METHOD_DEF(REMAP)
#endif

#ifdef METHOD_MESSAGE
    METHOD_DEF(MESSAGE)
#endif

#ifdef METHOD_GETLABEL
    METHOD_DEF(GETLABEL)
#else

ULONG __saveds GetGadgetLabel (Class *c, Object *o, Msg msg)
{
    struct DockMessageGetLabel *dmgl = (struct DockMessageGetLabel *)msg;
    dmgl->label = CLASS_NAME;

    return TRUE;
}
    
#endif

#ifdef METHOD_CANEDIT
    METHOD_DEF(CANEDIT)
#else

ULONG __saveds CanEdit(Class *c, Object *o, Msg msg)
{
    struct DockMessageCanEdit *dmce = (struct DockMessageCanEdit *)msg;
    dmce->canEdit = FALSE;

    return TRUE;
}

#endif

ULONG __saveds GetGadgetInfo(Class *c, Object *o, Msg msg)
{
    struct DockMessageGetInfo *i = (struct DockMessageGetInfo *)msg;
    
    i->name = CLASS_NAME;
    i->description = (STRPTR)MSG_Description;
    i->version = CLASS_VER_STR;
    i->copyright = CLASS_COPYRIGHT;

    return TRUE;
}


#ifdef METHOD_INITEDIT
    METHOD_DEF(INITEDIT)
#endif

ULONG __saveds InitEditor(Class *c, Object *o, Msg msg)
{
    BOOL res = TRUE;

    if( !ClassLib->cl_TritonBase ) {
        if( ClassLib->cl_TritonBase = OpenLibrary(TRITONNAME, TRITONVERSION) ) {
            TritonBase = ClassLib->cl_TritonBase;
        } else {
            res = FALSE;
        }
    }

#ifdef METHOD_INITEDIT
    if( res ) {
        METHOD_INITEDIT(c, o, msg);
    }
#endif

    return res;
}

#undef METHOD_DEF

#define METHOD_DIS(NAME) case DM_ ## NAME: return METHOD_ ## NAME (c, o, msg);

ULONG __saveds GadgetDispatch(Class *c, Object *o, Msg msg)
{
#ifdef METHOD_NEW
    Object *newObj;
#endif

    geta4();

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
        METHOD_DIS(ADDED)
#endif

#ifdef METHOD_REMOVED
        METHOD_DIS(REMOVED)
#endif

#ifdef METHOD_DRAW
        METHOD_DIS(DRAW)
#endif
            
#ifdef METHOD_TICK
        METHOD_DIS(TICK)
#endif

#ifdef METHOD_CLICK
        METHOD_DIS(CLICK)
#endif

#ifdef METHOD_DROP
        METHOD_DIS(DROP)
#endif

#ifdef METHOD_LAUNCHED
        METHOD_DIS(LAUNCHED)
#endif

#ifdef METHOD_GETSIZE
        METHOD_DIS(GETSIZE)
#endif

#ifdef METHOD_READCONFIG
        METHOD_DIS(READCONFIG)
#endif

#ifdef METHOD_WRITECONFIG
        METHOD_DIS(WRITECONFIG)
#endif

#ifdef METHOD_GETHOTKEY
        METHOD_DIS(GETHOTKEY)
#endif

#ifdef METHOD_HOTKEY
        METHOD_DIS(HOTKEY)
#endif

        case DM_GETLABEL:
#ifdef METHOD_GETLABEL
            return METHOD_GETLABEL (c, o, msg);
#else
            return GetGadgetLabel(c, o, msg);
#endif

        case DM_CANEDIT:
#ifdef METHOD_CANEDIT
            return METHOD_CANEDIT (c, o, msg);
#else
            return CanEdit (c, o, msg);
#endif

#ifdef METHOD_GETEDITOR
        METHOD_DIS(GETEDITOR)
#endif

#ifdef METHOD_EDITOREVENT
        METHOD_DIS(EDITOREVENT)
#endif

#ifdef METHOD_EDITORUPDATE
        METHOD_DIS(EDITORUPDATE)
#endif

#ifdef METHOD_INITBUTTON
        METHOD_DIS(INITBUTTON)
#endif

#ifdef METHOD_REMAP
        METHOD_DIS(REMAP)
#endif

#ifdef METHOD_MESSAGE
        METHOD_DIS(MESSAGE)
#endif

        case DM_INITEDIT:
            return InitEditor(c, o, msg);

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

    if( cb->cl_Catalog ) {
        CloseCatalog(cb->cl_Catalog);   
    }
}


VOID __saveds CloseLibs(VOID)
{
    if( TritonBase ) {
        CloseLibrary(TritonBase);
    }

    if( DockBotBase ) {
        CloseLibrary(DockBotBase);
    }

    if( IntuitionBase ) {
        CloseLibrary((struct Library *)IntuitionBase);
    }

    if( LocaleBase ) {
        CloseLibrary(LocaleBase);
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

#ifdef DEBUG_BUILD

BOOL __DebugEnabled = TRUE;

VOID DB_Printf(STRPTR fmt, ...)
{
    DB_DebugLog(fmt, (LONG *)(&fmt + 1));
}

#endif

VOID *_XCEXIT = NULL;

