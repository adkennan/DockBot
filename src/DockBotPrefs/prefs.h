/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __PREFS_H__
#define __PREFS_H__

#include <exec/types.h>
#include <exec/lists.h>
#include <intuition/classes.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "dockbotprefs_cat.h"

#include <stdio.h>

#include "dock_gadget.h"

#include "gadget_class.h"

#include <libraries/triton.h>

#define APP_NAME "DockBotPrefs"

enum {
    OBJ_MENU_ABOUT      = 1001,
    OBJ_MENU_QUIT       = 1002,

    OBJ_POSITION        = 1013,
    OBJ_ALIGNMENT       = 1014,
    OBJ_SHOW_LABELS     = 1015,
    OBJ_SHOW_BORDERS    = 1016,
    OBJ_BG_BRUSH        = 1017,
    OBJ_BTN_BG_BRUSH    = 1018,
    
    OBJ_GADGETS         = 1020,

    OBJ_BTN_NEW         = 1021,
    OBJ_BTN_DELETE      = 1022,
    OBJ_BTN_EDIT        = 1023,
    OBJ_BTN_UP          = 1024,
    OBJ_BTN_DOWN        = 1025,

    OBJ_BTN_SAVE        = 1031,
    OBJ_BTN_USE         = 1032,
    OBJ_BTN_TEST        = 1033,
    OBJ_BTN_CANCEL      = 1034,

    OBJ_BTN_GAD_OK      = 1041,
    OBJ_BTN_GAD_CAN     = 1042,

    OBJ_NEW_GADGET      = 1051,
    OBJ_BTN_NEW_OK      = 1052,
    OBJ_BTN_NEW_CAN     = 1053,
    OBJ_NEW_STR_NAME    = 1054,
    OBJ_NEW_STR_DESC    = 1055


};

struct DockPrefs 
{
	struct DockConfig cfg;
    struct TR_Project *mainWindow;

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

struct DgNode *add_dock_gadget(struct DockPrefs *prefs, STRPTR name);

VOID remove_dock_gadget(struct DgNode *dg);

VOID init_config(struct DockPrefs *prefs);

BOOL load_config(struct DockPrefs *prefs);

BOOL save_config(struct DockPrefs *prefs, BOOL permanent);

VOID remove_dock_gadgets(struct DockPrefs *prefs);

VOID free_plugins(struct DockPrefs *prefs);

VOID free_config(struct DockPrefs *prefs);

VOID revert_config(VOID);

VOID update_gadget_list(struct DockPrefs *prefs);

VOID edit_gadget(struct DockPrefs *prefs, struct DgNode *dg);

// new_gadget.c
VOID create_new_gadget(struct DockPrefs *prefs);

VOID class_selected(struct DockPrefs *prefs);

VOID open_new_gadget_dialog(struct DockPrefs *prefs);

VOID add_dropped_icon(struct DockPrefs *prefs, struct AppMessage *msg);

// gadget_list.c
VOID gadget_selected(struct DockPrefs *prefs, ULONG index);

VOID free_gadget_list(struct DockPrefs *prefs);

VOID update_gadget_list(struct DockPrefs *prefs);

struct DgNode *get_selected_gadget(struct DockPrefs *prefs);

VOID move_gadget_up(struct DockPrefs *prefs);

VOID move_gadget_down(struct DockPrefs *prefs);

VOID delete_gadget(struct DockPrefs *prefs);

VOID edit_gadget(struct DockPrefs *prefs, struct DgNode *dg);

// tags.c
struct TagItem *make_tag_list(ULONG data, ...);

ULONG count_tags(struct TagItem *list);

struct TagItem *copy_tags(struct TagItem *dest, struct TagItem *src);

struct TagItem *merge_tag_lists(struct TagItem *head, struct TagItem *middle, struct TagItem *tail);


#endif // __PREFS_H__
