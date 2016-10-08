
#ifndef __PREFS_H__
#define __PREFS_H__

#include <exec/types.h>
#include <exec/lists.h>
#include <intuition/classes.h>

#include <libraries/triton.h>

#include "dockbot.h"

#include "dock_settings.h"

#define DB_ROOT_PREFS_CLASS "DockGadgetPrefsBase"

struct DockPrefs 
{
	// Dock Settings
	DockPosition pos;
	DockAlign align;

	struct List gadgets;

	// Libraries containing prefs classes
	struct MinList libs;

	// Built in classes.
	Class *baseClass;
	Class *buttonClass;
    Class *iconClass;
};

struct DgNode 
{
	struct Node n;
	Object *dg;
    struct TR_Project *editor;
};

struct LibNode
{
	struct MinNode n;
	struct Library *lib;
};

Class *init_prefs_base_class(VOID);

BOOL free_prefs_base_class(Class *c);

Class *init_dock_button_class(VOID);

BOOL free_dock_button_class(Class *c);

Class *init_icon_class(VOID);

BOOL free_icon_class(Class *c);

BOOL load_config(struct DockPrefs *prefs);

VOID remove_dock_gadgets(struct DockPrefs *prefs);

//BOOL save_config(struct DockPrefs *prefs);

//BOOL use_config(struct DockPrefs *prefs);

// Gadget Messages

typedef enum {

    DM_GET_NAME,
    DM_SET_NAME,
    DM_GET_INFO,
    DM_CAN_EDIT,
    DM_GET_EDITOR,
    DM_HANDLE_EVENT,
    DM_READ_CONFIG,
    DM_WRITE_CONFIG,
    DM_RESET

} DockPrefsMessage;

STRPTR dock_gadget_get_name(Object *obj);

VOID dock_gadget_set_name(Object *obj, STRPTR name);

VOID dock_gadget_get_info(Object *obj
            , STRPTR *name, STRPTR *version
            , STRPTR *description, STRPTR *copyright);

BOOL dock_gadget_can_edit(Object *obj);

struct TagItem *dock_gadget_get_editor(Object *obj);

VOID dock_gadget_handle_event(Object *obj, struct TR_Message *msg);

VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings);

VOID dock_gadget_write_settings(Object *obj, struct DockSettings *settings);

VOID dock_gadget_reset(Object *obj);

struct DockMessageSetName {
    ULONG MethodID;
    STRPTR name;
};

struct DockMessageHandleEvent {
    ULONG MethodID;
    struct TR_Message *msg;
};


struct DockMessageSettings {
	ULONG MethodID;
	struct DockSettings *settings;
};

struct TagItem *make_tag_list(ULONG data, ...);

#endif // __PREFS_H__

