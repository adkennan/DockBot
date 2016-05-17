
#ifndef __PREF_EDITOR_H__
#define __PREF_EDITOR_H__

#include "dockbot.h"

#include <exec/lists.h>
#include <intuition/classes.h>

struct DockGadgetInfo {
	struct MinNode n;
	STRPTR gadgetName;
	STRPTR name;
	STRPTR description;
	STRPTR version;
	STRPTR copyright;
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

//Object *get_editor(UWORD ix);

#endif
