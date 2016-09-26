
#ifndef __PREF_EDITOR_H__
#define __PREF_EDITOR_H__

#include "dockbot.h"
#include "dock_settings.h"

#include <exec/lists.h>
#include <intuition/classes.h>

struct DockGadgetInfo {
	struct MinNode n;
	Object *editor;
};

struct LibNode {
	struct Node n;
	struct Library *lib;
};

struct GadgetList {
	DockPosition pos;
	DockAlign align;
	struct MinList libs;
	struct MinList gadgets;
};

struct GadgetList* load_config(VOID);

//VOID use_config(struct GadgetList* config);

//VOID save_config(struct GadgetList* config);

VOID free_config(struct GadgetList* config);

typedef enum {
    DM_DISPLAYNAME  = 1300,
    DM_ABOUT        = 1301,
    DM_WRITECONFIG  = 1302
} DockPrefsMsg;

struct DockPrefMsgDisplayName {
    ULONG MethodID;
    STRPTR displayName;
};

struct DockPrefMsgAbout {
    ULONG MethodID;
    STRPTR name;
    STRPTR gadgetName;
    STRPTR description;
    STRPTR version;
    STRPTR copyright;
};

struct DockPrefMsgConfig {
    ULONG MethodID;
    struct DockSettings *settings;
};


#endif
