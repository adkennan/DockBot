@DATABASE "dockbot.h"
@MASTER   "Stuff:Andrew/DockBot/include/dockbot.h"
@REMARK   This file was created by ADtoHT 2.1 on 17-Aug-19 16:16:01
@REMARK   Do not edit
@REMARK   ADtoHT is © 1993-1995 Christian Stieber

@NODE MAIN "dockbot.h"

@{"dockbot.h" LINK File}


@{b}Structures@{ub}

@{"DgNode" LINK "dockbot.h/File" 53}                   @{"DockConfig" LINK "dockbot.h/File" 45}            @{"DockMessageAdded" LINK "dockbot.h/File" 123}
@{"DockMessageCanEdit" LINK "dockbot.h/File" 195}       @{"DockMessageClick" LINK "dockbot.h/File" 129}      @{"DockMessageConfig" LINK "dockbot.h/File" 180}
@{"DockMessageDraw" LINK "dockbot.h/File" 156}          @{"DockMessageDrop" LINK "dockbot.h/File" 135}       @{"DockMessageEditorEvent" LINK "dockbot.h/File" 229}
@{"DockMessageEditorUpdate" LINK "dockbot.h/File" 235}  @{"DockMessageGetBounds" LINK "dockbot.h/File" 167}  @{"DockMessageGetEditor" LINK "dockbot.h/File" 206}
@{"DockMessageGetHotKey" LINK "dockbot.h/File" 185}     @{"DockMessageGetInfo" LINK "dockbot.h/File" 172}    @{"DockMessageGetLabel" LINK "dockbot.h/File" 190}
@{"DockMessageGetSize" LINK "dockbot.h/File" 148}       @{"DockMessageHitTest" LINK "dockbot.h/File" 162}    @{"DockMessageInitButton" LINK "dockbot.h/File" 200}
@{"DockMessageLaunch" LINK "dockbot.h/File" 211}        @{"DockMessageSetBounds" LINK "dockbot.h/File" 142}  @{"DockSettingValue" LINK "dockbot.h/File" 305}
@{"GadgetMessage" LINK "dockbot.h/File" 248}            @{"GadgetMessageLaunch" LINK "dockbot.h/File" 255}   @{"Rect" LINK "dockbot.h/File" 63}
@{"Values" LINK "dockbot.h/File" 266}                   


@{b}Typedefs@{ub}

@{"DockAlign" LINK "dockbot.h/File" 30}  @{"DockMessage" LINK "dockbot.h/File" 76}  @{"DockPosition" LINK "dockbot.h/File" 37}  @{"GadgetMessageType" LINK "dockbot.h/File" 242}


@{b}#defines@{ub}

@{"CONFIG_FILE" LINK "dockbot.h/File" 297}        @{"CONFIG_FILE_PERM" LINK "dockbot.h/File" 298}  @{"DB_ROOT_CLASS" LINK "dockbot.h/File" 68}  @{"DEFAULT_SIZE" LINK "dockbot.h/File" 70}
@{"FOR_EACH_GADGET()" LINK "dockbot.h/File" 59}  @{"FREE_STRING()" LINK "dockbot.h/File" 290}     @{"GET_STRING()" LINK "dockbot.h/File" 274}   @{"GET_VALUE()" LINK "dockbot.h/File" 279}
@{"IS_KEY()" LINK "dockbot.h/File" 272}           @{"MAX_PATH_LENGTH" LINK "dockbot.h/File" 72}   @{"REVISION" LINK "dockbot.h/File" 25}       @{"S_ALIGN" LINK "dockbot.h/File" 300}
@{"S_GADGET" LINK "dockbot.h/File" 303}           @{"S_LABELS" LINK "dockbot.h/File" 302}          @{"S_POSITION" LINK "dockbot.h/File" 301}     @{"VERSION" LINK "dockbot.h/File" 24}
@{"VERSION_STR" LINK "dockbot.h/File" 27}        

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
#define REVISION 0

#define VERSION_STR "1.0 (03/06/2019)"


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

struct DockConfig
{
        @{"DockPosition" LINK File 37} pos;
        @{"DockAlign" LINK File 30} align;
    BOOL showGadgetLabels;
        struct List gadgets;
};

struct DgNode
{
    struct Node n;
    Object *dg;
};

#define FOR_EACH_GADGET(list, curr) for( curr = (@{"struct DgNode" LINK File 53} *)(list)->lh_Head; \\
                                         curr->n.ln_Succ; \\
                                         curr = (@{"struct DgNode" LINK File 53} *)curr->n.ln_Succ )

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
        DM_SETBOUNDS        = 1206,
    DM_GETBOUNDS        = 1207,
        DM_HITTEST                  = 1208,
    DM_REQ_LAUNCH       = 1209,

    // Event Handlers.
        DM_TICK                     = 1302,
        DM_CLICK                    = 1303,
        DM_DROP                     = 1304,
        DM_HOTKEY                       = 1305,
    DM_LAUNCHED         = 1306,

    // Rendering.
        DM_GETSIZE                  = 1400,
        DM_DRAW                     = 1401,

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

struct DockMessageSetBounds
{
        ULONG MethodID;
        @{"struct Rect" LINK File 63} *b;
};

struct DockMessageGetSize
{
        ULONG MethodID;
        @{"DockPosition" LINK File 37} position;
        @{"DockAlign" LINK File 30} align;
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

struct DockMessageGetBounds {
    ULONG MethodID;
    @{"struct Rect" LINK File 63} *b;
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
    GM_DRAW         = 1700,
    GM_QUIT         = 1701,
    GM_LAUNCH       = 1702
} GadgetMessageType;

struct GadgetMessage {
    struct Message m;
    @{"GadgetMessageType" LINK File 242} messageType;
    Object* sender;
};


struct GadgetMessageLaunch {
    @{"struct GadgetMessage" LINK File 248} m;
    STRPTR path;
    STRPTR args;
    STRPTR console;
    BOOL wb;
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
        DB_FreeMem(str, strlen(str) + 1); \\
        str = NULL; \\
    }


#define CONFIG_FILE "ENV:DockBot.prefs"
#define CONFIG_FILE_PERM "ENVARC:DockBot.prefs"

#define S_ALIGN "align"
#define S_POSITION "position"
#define S_LABELS "labels"
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
