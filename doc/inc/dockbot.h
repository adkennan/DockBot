@DATABASE "dockbot.h"
@MASTER   "Stuff:Andrew/DockBot/include/dockbot.h"
@REMARK   This file was created by ADtoHT 2.1 on 05-May-20 21:03:26
@REMARK   Do not edit
@REMARK   ADtoHT is © 1993-1995 Christian Stieber

@NODE MAIN "dockbot.h"

@{"dockbot.h" LINK File}


@{b}Structures@{ub}

@{"DgNode" LINK "dockbot.h/File" 75}                   @{"DockConfig" LINK "dockbot.h/File" 65}            @{"DockMessageAdded" LINK "dockbot.h/File" 169}
@{"DockMessageCanEdit" LINK "dockbot.h/File" 241}       @{"DockMessageClick" LINK "dockbot.h/File" 175}      @{"DockMessageConfig" LINK "dockbot.h/File" 226}
@{"DockMessageDraw" LINK "dockbot.h/File" 202}          @{"DockMessageDrop" LINK "dockbot.h/File" 181}       @{"DockMessageEditorEvent" LINK "dockbot.h/File" 279}
@{"DockMessageEditorUpdate" LINK "dockbot.h/File" 285}  @{"DockMessageGetEditor" LINK "dockbot.h/File" 252}  @{"DockMessageGetEnvironment" LINK "dockbot.h/File" 213}
@{"DockMessageGetHotKey" LINK "dockbot.h/File" 231}     @{"DockMessageGetInfo" LINK "dockbot.h/File" 218}    @{"DockMessageGetLabel" LINK "dockbot.h/File" 236}
@{"DockMessageGetSize" LINK "dockbot.h/File" 194}       @{"DockMessageHitTest" LINK "dockbot.h/File" 208}    @{"DockMessageInitButton" LINK "dockbot.h/File" 246}
@{"DockMessageLaunch" LINK "dockbot.h/File" 257}        @{"DockMessagePort" LINK "dockbot.h/File" 265}       @{"DockMessageSetEnvironment" LINK "dockbot.h/File" 188}
@{"DockSettingValue" LINK "dockbot.h/File" 369}         @{"GadgetEnvironment" LINK "dockbot.h/File" 159}     @{"GadgetMessage" LINK "dockbot.h/File" 301}
@{"GadgetMessageLaunch" LINK "dockbot.h/File" 307}      @{"GadgetMessagePort" LINK "dockbot.h/File" 316}     @{"PortReg" LINK "dockbot.h/File" 89}
@{"Rect" LINK "dockbot.h/File" 96}                     @{"Values" LINK "dockbot.h/File" 323}                


@{b}Typedefs@{ub}

@{"BrushFlags" LINK "dockbot.h/File" 58}  @{"DockAlign" LINK "dockbot.h/File" 43}  @{"DockMessage" LINK "dockbot.h/File" 109}  @{"DockPosition" LINK "dockbot.h/File" 50}  @{"GadgetMessageType" LINK "dockbot.h/File" 292}


@{b}#defines@{ub}

@{"CONFIG_FILE" LINK "dockbot.h/File" 359}    @{"CONFIG_FILE_PERM" LINK "dockbot.h/File" 360}   @{"DB_BUTTON_CLASS" LINK "dockbot.h/File" 41}
@{"DB_ROOT_CLASS" LINK "dockbot.h/File" 101}  @{"DEBUG()" LINK "dockbot.h/File" 33}            @{"DEFAULT_SIZE" LINK "dockbot.h/File" 103}
@{"FOR_EACH()" LINK "dockbot.h/File" 82}     @{"FOR_EACH_GADGET()" LINK "dockbot.h/File" 87}  @{"FOR_EACH_PORTREG()" LINK "dockbot.h/File" 94}
@{"FREE_STRING()" LINK "dockbot.h/File" 352}  @{"GET_INTEGER()" LINK "dockbot.h/File" 338}      @{"GET_STRING()" LINK "dockbot.h/File" 331}
@{"GET_VALUE()" LINK "dockbot.h/File" 341}    @{"IS_KEY()" LINK "dockbot.h/File" 329}           @{"MAX_PATH_LENGTH" LINK "dockbot.h/File" 105}
@{"REVISION" LINK "dockbot.h/File" 25}       @{"S_ALIGN" LINK "dockbot.h/File" 362}            @{"S_BACKGROUND" LINK "dockbot.h/File" 366}
@{"S_BORDERS" LINK "dockbot.h/File" 365}      @{"S_GADGET" LINK "dockbot.h/File" 367}           @{"S_LABELS" LINK "dockbot.h/File" 364}
@{"S_POSITION" LINK "dockbot.h/File" 363}     @{"VERSION" LINK "dockbot.h/File" 24}            @{"VERSION_STR" LINK "dockbot.h/File" 27}

@ENDNODE
@NODE File "dockbot.h"
/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __DOCKBOT_H__
#define __DOCKBOT_H__


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <clib/exec_protos.h>

#include <string.h>

#include <intuition/intuition.h>
#include <intuition/classes.h>


#define VERSION 1
#define REVISION 3

#define VERSION_STR BUILDVER " (" DS ")"

#ifdef DEBUG_BUILD

extern BOOL __DebugEnabled;

#define @{"DEBUG" LINK "File" 37}(x) if( __DebugEnabled ) { x; }

#else

#define @{"DEBUG" LINK "File" 33}(x)

#endif

#define DB_BUTTON_CLASS "DockButton"

typedef enum
{
        DA_LEFT,
        DA_CENTER,
        DA_RIGHT
} DockAlign;

typedef enum
{
        DP_LEFT,
        DP_RIGHT,
        DP_TOP,
        DP_BOTTOM
} DockPosition;

typedef enum
{
    BF_NONE        = 0,
    BF_CREATE_MASK = 2 << 0,
    BF_TRIM        = 2 << 1
} BrushFlags;

struct DockConfig
{
        @{"DockPosition" LINK File 50} pos;
        @{"DockAlign" LINK File 43} align;
    BOOL showGadgetLabels;
        BOOL showGadgetBorders;
        struct List gadgets;
    STRPTR bgBrushPath;
};

struct DgNode
{
    struct Node n;
    Object *dg;
        struct MinList ports;
};

#define FOR_EACH(type,list,curr) for( curr = (type *)((struct List *)(list))->lh_Head; \\
                                      ((struct Node *)curr)->ln_Succ; \\
                                      curr = (type *)((struct Node *)curr)->ln_Succ )


#define FOR_EACH_GADGET(list, curr) @{"FOR_EACH" LINK File 82}(@{"struct DgNode" LINK File 75}, list, curr)

struct PortReg {
    struct MinNode n;
    struct MsgPort *port;
};

#define FOR_EACH_PORTREG(list, curr) @{"FOR_EACH" LINK File 82}(@{"struct PortReg" LINK File 89}, list, curr)

struct Rect
{
        UWORD x,y,w,h;
};

#define DB_ROOT_CLASS "dockgadget"

#define DEFAULT_SIZE 48

#define MAX_PATH_LENGTH 2048

// Object messages

typedef enum {

    // Internal - Don't override.
    DM_ADDED            = 1201,
    DM_REMOVED          = 1202,
    DM_REQ_QUIT         = 1203,
    DM_REQ_DRAW         = 1204,
    DM_BUILTIN          = 1205,
        DM_SETENV                   = 1206,
    DM_GETENV           = 1207,
        DM_HITTEST                  = 1208,
    DM_REQ_LAUNCH       = 1209,
    DM_REG_PORT         = 1210,
    DM_UNREG_PORT       = 1211,

    // Event Handlers.
        DM_TICK                     = 1302,
        DM_CLICK                    = 1303,
        DM_DROP                     = 1304,
        DM_HOTKEY                       = 1305,
    DM_LAUNCHED         = 1306,
    DM_MESSAGE          = 1307,

    // Rendering.
        DM_GETSIZE                  = 1400,
        DM_DRAW                     = 1401,
    DM_REMAP            = 1402,

    // Configuration.
        DM_READCONFIG       = 1500,
        DM_WRITECONFIG          = 1501,
        DM_CANEDIT                      = 1502,
        DM_INITBUTTON           = 1503,
        DM_GETEDITOR            = 1504,
        DM_EDITOREVENT          = 1505,
        DM_EDITORUPDATE         = 1506,
    DM_INITEDIT         = 1507,

    // Getters.
    DM_GETINFO          = 1601,
        DM_GETHOTKEY            = 1602,
        DM_GETLABEL                     = 1603

} DockMessage;

struct DockSettings;
struct DockWindow;

extern struct Library *DockBotBase;

struct GadgetEnvironment {
        @{"struct Rect" LINK File 96} gadgetBounds;
    @{"struct Rect" LINK File 96} windowBounds;
        UWORD align;
        UWORD pos;
    UWORD index;
        BOOL showBorders;
    BOOL isLast;
};

struct DockMessageAdded
{
    ULONG MethodID;
    struct MsgPort *dockPort;
};

struct DockMessageClick
{
        ULONG MethodID;
        UWORD x,y;
};

struct DockMessageDrop
{
        ULONG MethodID;
        STRPTR* paths;
        UWORD pathCount;
};

struct DockMessageSetEnvironment
{
        ULONG MethodID;
        @{"struct GadgetEnvironment" LINK File 159} *env;
};

struct DockMessageGetSize
{
        ULONG MethodID;
        @{"DockPosition" LINK File 50} position;
        @{"DockAlign" LINK File 43} align;
        UWORD w, h;
};

struct DockMessageDraw
{
        ULONG MethodID;
        struct RastPort *rp;
};

struct DockMessageHitTest {
        ULONG MethodID;
        UWORD x, y;
};

struct DockMessageGetEnvironment {
    ULONG MethodID;
        @{"struct GadgetEnvironment" LINK File 159} *env;
};

struct DockMessageGetInfo {
    ULONG MethodID;
    STRPTR name;
    STRPTR version;
    STRPTR description;
    STRPTR copyright;
};

struct DockMessageConfig {
        ULONG MethodID;
        struct DockSettings *settings;
};

struct DockMessageGetHotKey {
        ULONG MethodID;
        STRPTR hotKey;
};

struct DockMessageGetLabel {
        ULONG MethodID;
        STRPTR label;
};

struct DockMessageCanEdit {
        ULONG MethodID;
        BOOL canEdit;
};

struct DockMessageInitButton {
    ULONG MethodID;
    STRPTR name;
    STRPTR path;
};

struct DockMessageGetEditor {
        ULONG MethodID;
        struct TagItem *uiTags;
};

struct DockMessageLaunch {
        ULONG MethodID;
    STRPTR path;
    STRPTR args;
    STRPTR console;
    BOOL wb;
};

struct DockMessagePort {
    ULONG MethodID;
    struct MsgPort *port;
};

struct TR_App;
struct TR_Project;
struct TR_Message;

struct DockMessageEditorInit {
    ULONG MethodID;
    struct TR_App* app;
};

struct DockMessageEditorEvent {
        ULONG MethodID;
        struct TR_Project *window;
        struct TR_Message *message;
};

struct DockMessageEditorUpdate {
        ULONG MethodID;
        struct TR_Project *window;
};

// Messages from Gadget to Dock.

typedef enum {
    GM_DRAW             = 1700,
    GM_QUIT             = 1701,
    GM_LAUNCH           = 1702,
    GM_REGISTER_PORT    = 1703,
    GM_UNREGISTER_PORT  = 1704,
    GM_GET_DOCK_INFO    = 1705
} GadgetMessageType;

struct GadgetMessage {
    struct Message m;
    @{"GadgetMessageType" LINK File 292} messageType;
    Object* sender;
};

struct GadgetMessageLaunch {
    @{"struct GadgetMessage" LINK File 301} m;
    STRPTR path;
    STRPTR args;
    STRPTR console;
    BOOL wb;
};

// Used by GM_REGISTER_PORT and GM_UNREGISTER_PORT
struct GadgetMessagePort {
    @{"struct GadgetMessage" LINK File 301} m;
    struct MsgPort *port;
};

// Settings

struct Values
{
    STRPTR Name;
    ULONG Value;
};

#define IS_KEY(key, setting) (strncmp(key, setting.Key, strlen(key)) == 0)

#define GET_STRING(setting, dest) \\
    dest = (STRPTR)DB_AllocMem(setting.ValueLength + 1, MEMF_CLEAR); \\
    CopyMem(setting.Value, dest, setting.ValueLength); \\
    dest[setting.ValueLength] = '\\0';

int atoi(const char *);

#define GET_INTEGER(setting, dest) \\
    dest = atoi(setting.Value);

#define GET_VALUE(setting, values, curr, len, dest) \\
    curr = &values[0];\\
    while( curr->Name ) { \\
        len = strlen(curr->Name);\\
        if( len == setting.ValueLength && strncmp(setting.Value, curr->Name, len) == 0) { \\
            dest = curr->Value;\\
            break; \\
        }\\
        curr++; \\
    }

#define FREE_STRING(str) \\
    if( str ) { \\
        DB_FreeMem((VOID *)str, strlen((STRPTR)str) + 1); \\
        str = NULL; \\
    }


#define CONFIG_FILE "ENV:DockBot.prefs"
#define CONFIG_FILE_PERM "ENVARC:DockBot.prefs"

#define S_ALIGN "align"
#define S_POSITION "position"
#define S_LABELS "labels"
#define S_BORDERS "borders"
#define S_BACKGROUND "background"
#define S_GADGET "gadget"

struct DockSettingValue
{
        STRPTR Key;
        STRPTR Value;
        UWORD KeyLength;
        UWORD ValueLength;
};

#endif
@ENDNODE
