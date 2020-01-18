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

#define DEBUG(x) if( __DebugEnabled ) { x; }

#else 

#define DEBUG(x) 

#endif

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
	DockPosition pos;
	DockAlign align;
    BOOL showGadgetLabels;
	struct List gadgets;
};

struct DgNode
{
    struct Node n;
    Object *dg;
	struct MinList ports;
};

#define FOR_EACH(type,list,curr) for( curr = (type *)((struct List *)(list))->lh_Head; \
                                      ((struct Node *)curr)->ln_Succ; \
                                      curr = (type *)((struct Node *)curr)->ln_Succ )


#define FOR_EACH_GADGET(list, curr) FOR_EACH(struct DgNode, list, curr)

struct PortReg {
    struct MinNode n;
    struct MsgPort *port;
};

#define FOR_EACH_PORTREG(list, curr) FOR_EACH(struct PortReg, list, curr)

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
	DM_SETBOUNDS 	    = 1206,
    DM_GETBOUNDS        = 1207,
	DM_HITTEST		    = 1208,
    DM_REQ_LAUNCH       = 1209,
    DM_REG_PORT         = 1210,
    DM_UNREG_PORT       = 1211,

    // Event Handlers.
	DM_TICK 		    = 1302,
	DM_CLICK 		    = 1303,
	DM_DROP 		    = 1304,
	DM_HOTKEY			= 1305,
    DM_LAUNCHED         = 1306,
    DM_MESSAGE          = 1307,

    // Rendering.
	DM_GETSIZE 		    = 1400,
	DM_DRAW			    = 1401,
    DM_REMAP            = 1402,

    // Configuration.
	DM_READCONFIG	    = 1500,
	DM_WRITECONFIG		= 1501,
	DM_CANEDIT			= 1502,
	DM_INITBUTTON		= 1503,
	DM_GETEDITOR		= 1504,
	DM_EDITOREVENT		= 1505,
	DM_EDITORUPDATE		= 1506,
    DM_INITEDIT         = 1507,

    // Getters.
    DM_GETINFO          = 1601,
	DM_GETHOTKEY		= 1602,
	DM_GETLABEL			= 1603

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
	struct Rect *b;
    UWORD windowX, windowY;
};

struct DockMessageGetSize
{
	ULONG MethodID;
	DockPosition position;
	DockAlign align;
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
    struct Rect *b;
    UWORD windowX, windowY;
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
    GadgetMessageType messageType;
    Object* sender;
};

struct GadgetMessageLaunch {
    struct GadgetMessage m;
    STRPTR path;
    STRPTR args;
    STRPTR console;
    BOOL wb;
};

// Used by GM_REGISTER_PORT and GM_UNREGISTER_PORT
struct GadgetMessagePort {
    struct GadgetMessage m;
    struct MsgPort *port;
};

// Settings

struct Values
{
    STRPTR Name;
    ULONG Value;
};

#define IS_KEY(key, setting) (strncmp(key, setting.Key, strlen(key)) == 0)

#define GET_STRING(setting, dest) \
    dest = (STRPTR)DB_AllocMem(setting.ValueLength + 1, MEMF_CLEAR); \ 
    CopyMem(setting.Value, dest, setting.ValueLength); \
    dest[setting.ValueLength] = '\0'; 

#define GET_VALUE(setting, values, curr, len, dest) \
    curr = &values[0];\
    while( curr->Name ) { \
        len = strlen(curr->Name);\
        if( len == setting.ValueLength && strncmp(setting.Value, curr->Name, len) == 0) { \
            dest = curr->Value;\
            break; \
        }\
        curr++; \
    }

#define FREE_STRING(str) \
    if( str ) { \
        DB_FreeMem(str, strlen(str) + 1); \
        str = NULL; \
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