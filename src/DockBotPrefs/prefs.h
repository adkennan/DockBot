
#ifndef __PREFS_H__
#define __PREFS_H__

#include <exec/types.h>
#include <exec/lists.h>
#include <intuition/classes.h>

#include "dockbot.h"

#include "dock_gadget.h"

#include "gadget_class.h"

#include <libraries/triton.h>

#define DB_BUTTON_CLASS "DockButton"

struct DockPrefs 
{
	struct DockConfig cfg;

    struct List gadLabels;
    struct List classes;

    struct DgNode *editGadget;
    struct TR_Project *editDialog;

	struct Hook getEditorHook;
	struct Hook editorEventHook;
	struct Hook editorUpdateHook;

    struct TR_Project *newGadgetDialog;
    BOOL editGadgetIsNew;
};

struct DgNode *add_dock_gadget(struct DockPrefs *prefs, Object *btn, STRPTR name);

VOID remove_dock_gadget(struct DgNode *dg);

BOOL load_config(struct DockPrefs *prefs);

BOOL save_config(struct DockPrefs *prefs, BOOL permanent);

VOID remove_dock_gadgets(struct DockPrefs *prefs);

VOID free_plugins(struct DockPrefs *prefs);

#endif // __PREFS_H__
