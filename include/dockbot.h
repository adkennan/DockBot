
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

typedef enum {
	DM_TICK 		= 0x1200,
	DM_CLICK 		= 0x1201,
	DM_DROP 		= 0x1202,
	DM_SETBOUNDS 	= 0x1203,
	DM_GETSIZE 		= 0x1204,
	DM_DRAW			= 0x1205,
	DM_HITTEST		= 0x1206,
    DM_GETBOUNDS    = 0x1207,
	DM_READCONFIG	= 0x1208
} DockMessage;

struct DockMessageClick
{
	ULONG MethodID;
	UWORD x,y;
};

struct DockMessageDrop
{
	ULONG MethodID;
	STRPTR path;
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

struct DockWindow;

extern struct Library *DockBotBase;

struct DockSettings;

struct DockSettingValue
{
	STRPTR Key;
	STRPTR Value;
	UWORD KeyLength;
	UWORD ValueLength;
};

struct DockMessageReadSettings {
	ULONG MethodID;
	struct DockSettings *settings;
};

#endif