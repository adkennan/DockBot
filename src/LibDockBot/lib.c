/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <exec/types.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>

#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>


#include "dockbot_protos.h"

#include "lib.h"

/**** Initialization Structs ****/
struct InitTable;
struct InitDataTable;


/**** Library Init ****/
struct DockBotLibrary * __saveds __asm InitLib(
    register __a6 struct Library *sysBase,
    register __a0 BPTR segList,
    register __d0 struct DockBotLibrary *dockBotBase);


/**** Library Functions ****/
struct DockBotLibrary * __saveds __asm _LibOpen(
    register __a6 struct DockBotLibrary *dockBotBase);
    
BPTR __saveds __asm _LibClose(
    register __a6 struct DockBotLibrary *dockBotBase);

BPTR __saveds __asm _LibExpunge(
    register __a6 struct DockBotLibrary *dockBotBase);

ULONG __saveds __asm _ExtFuncLib(
    register __a6 struct DockBotLibrary *dockBotBase);

/**** Utility Functions ****/

VOID __saveds FreeLib(struct DockBotLibrary *lib);


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
extern char LibName[];
extern char LibId[];


struct Resident __aligned _LibRomTag = {
    RTC_MATCHWORD,
    &_LibRomTag,
    &EndResident,
    RTF_AUTOINIT,
    VERSION,
    NT_LIBRARY,
    0,
    &LibName[0],
    &LibId[0],
    &InitTab
};

APTR EndResident;


char __aligned LibName[]   = "dockbot.library";
char __aligned LibId[]     = "dockbot.library " VERSION_STR; 


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
    0x80, OFFSET(Node,      ln_Name),               (ULONG)&LibName[0],
    INITBYTE(OFFSET(Library, lib_Flags),            LIBF_SUMUSED | LIBF_CHANGED),
    INITWORD(OFFSET(Library, lib_Version),          VERSION),
    INITWORD(OFFSET(Library, lib_Revision),         REVISION),
    0x80, OFFSET(Library,   lib_IdString),          (ULONG)&LibId[0],
    (ULONG)0
};

struct InitTable {
    ULONG   LibBaseSize;
    APTR   *FuncTable;
    struct  InitDataTable *DataTable;
    APTR    InitLibTable;
} InitTab = {
    sizeof(struct DockBotLibrary),
    (APTR*)&FuncTable[0],
    (struct InitDataTable *)&DataTable,
    (APTR)InitLib
};


APTR FuncTable[] = {
    _LibOpen,
    _LibClose,
    _LibExpunge,
    _ExtFuncLib,

    DB_GetDockGadgetBounds,
    DB_RequestDockQuit,
    DB_RequestDockGadgetDraw,
    DB_RequestLaunch,

    DB_DrawOutsetFrame,
    DB_DrawInsetFrame,

    DB_OpenSettingsRead,
    DB_OpenSettingsWrite,
    DB_CloseSettings,
    DB_ReadBeginBlock,
    DB_ReadEndBlock,
    DB_ReadSetting,
    DB_WriteBeginBlock,
    DB_WriteEndBlock,
    DB_WriteSetting,
    DB_ReadConfig,
    DB_WriteConfig,

    DB_AllocMem,
    DB_FreeMem,
    DB_GetMemInfo,
    
    DB_CreateDockGadget,
    DB_ListClasses,

    DB_ShowError,

    (APTR)-1
};

struct Library *SysBase = NULL;
struct Library *DOSBase = NULL;
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
struct DockBotLibrary *DockBotBase = NULL;

struct DockBotLibrary * __saveds __asm InitLib(
    register __a6 struct Library *sysBase,
    register __a0 BPTR segList,
    register __d0 struct DockBotLibrary *dockBotBase)
{
    SysBase = sysBase;

    DockBotBase = dockBotBase;
    DockBotBase->l_ExecBase = sysBase;
    DockBotBase->l_SegList = segList;

    if( IntuitionBase = OpenLibrary("intuition.library", 39) ) {

        DockBotBase->l_IntuitionBase = IntuitionBase;

        if( DOSBase = OpenLibrary("dos.library", 39) ) {

            DockBotBase->l_DOSBase = DOSBase;

            if( GfxBase = OpenLibrary("graphics.library", 39) ) {

                DockBotBase->l_GfxBase = GfxBase;

                if( InitMem(DockBotBase) ) {

                    if( InitClassLibs(DockBotBase) ) {

                        if( InitGadgetClass(DockBotBase) ) {

                            return DockBotBase;
                        }
                        FreeClassLibs(DockBotBase);
                    }
                    CleanUpMem(DockBotBase);
                }
                CloseLibrary(GfxBase);
            }
            CloseLibrary(DOSBase);
        }
        CloseLibrary(IntuitionBase);
    }

    FreeLib(DockBotBase);
    return NULL;
}

struct DockBotLibrary * __saveds __asm _LibOpen(
    register __a6 struct DockBotLibrary *dockBotBase)
{
    dockBotBase->l_Lib.lib_OpenCnt++;
    dockBotBase->l_Lib.lib_Flags &= ~LIBF_DELEXP;

    return dockBotBase;
}
    
BPTR __saveds __asm _LibClose(
    register __a6 struct DockBotLibrary *dockBotBase)
{
    dockBotBase->l_Lib.lib_OpenCnt--;
    
    if( dockBotBase->l_Lib.lib_OpenCnt == ClassLibCount(dockBotBase) ) {

        FreeClassLibs(dockBotBase);

        if( dockBotBase->l_Lib.lib_Flags & LIBF_DELEXP ) {
            return _LibExpunge(dockBotBase);
        }
    }
    return NULL;
}

BPTR __saveds __asm _LibExpunge(
    register __a6 struct DockBotLibrary *dockBotBase)
{
    BPTR segList;

    if( dockBotBase->l_Lib.lib_OpenCnt == ClassLibCount(dockBotBase) ) {
        
        FreeGadgetClass(dockBotBase);
        FreeClassLibs(dockBotBase);
        CleanUpMem(dockBotBase);

        segList = dockBotBase->l_SegList;

        Remove((struct Node*)dockBotBase);

        FreeLib(dockBotBase);

        return segList;
    }

    dockBotBase->l_Lib.lib_Flags |= LIBF_DELEXP;

    return NULL;
}

ULONG __saveds __asm _ExtFuncLib(
    register __a6 struct DockBotLibrary *dockBotBase)
{
    return NULL;
}

VOID __saveds FreeLib(struct DockBotLibrary *lib)
{
    ULONG neg, pos, full;
    UBYTE* negPtr = (UBYTE*)lib;

    neg = lib->l_Lib.lib_NegSize;
    pos = lib->l_Lib.lib_PosSize;
    full = neg + pos;
    negPtr -= neg;

    FreeMem(negPtr, full);
}

VOID *_XCEXIT = NULL;

