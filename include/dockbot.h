/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCKBOT_H__
#define __DOCKBOT_H__

#include <intuition/intuition.h>

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

struct Rect
{
	UWORD x,y,w,h;
};

#define DB_ROOT_CLASS "dockgadget"

#define DEFAULT_SIZE 48

#define MAX_PATH_LENGTH 2048

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

    // Event Handlers.
	DM_TICK 		    = 1302,
	DM_CLICK 		    = 1303,
	DM_DROP 		    = 1304,
	DM_HOTKEY			= 1305,

    // Rendering.
	DM_GETSIZE 		    = 1400,
	DM_DRAW			    = 1401,

    // Configuration.
	DM_READCONFIG	    = 1500,
	DM_WRITECONFIG		= 1501,
    DM_GETSETTINGS      = 1502,

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

struct DockSettingValue
{
	STRPTR Key;
	STRPTR Value;
	UWORD KeyLength;
	UWORD ValueLength;
};

#endif