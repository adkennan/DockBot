/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <exec/types.h>
#include <exec/lists.h>

#include "dockbot.h"
#include "class_def.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "gadget_class.h"
#include "switcher_cat.h"

#define CLASS_NAME      "Switcher"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (" DS ")"
#define CLASS_COPYRIGHT "© 2019 Andrew Kennan"
#define PRIVATE_DATA    SwitcherGadgetData
#define PARENT_CLASS    DB_ROOT_CLASS

#define GADGET_LIB_DATA     SwitcherLibData

#define GADGET_LIB_INIT     switcher_lib_init
#define GADGET_LIB_EXPUNGE  switcher_lib_expunge

#define METHOD_NEW          switcher_new
#define METHOD_DISPOSE      switcher_dispose

#define METHOD_TICK         switcher_tick
#define METHOD_CLICK        switcher_click
#undef  METHOD_DROP         
#define METHOD_HOTKEY		switcher_hotkey
#undef  METHOD_LAUNCHED     
#define METHOD_MESSAGE      switcher_message

#define METHOD_GETSIZE      switcher_get_size
#define METHOD_DRAW         switcher_draw
#undef  METHOD_REMAP        

#define METHOD_READCONFIG   switcher_read_config
#define METHOD_WRITECONFIG  switcher_write_config
#undef METHOD_INITEDIT     
#define METHOD_GETEDITOR	switcher_get_editor
#undef METHOD_EDITOREVENT  
#define METHOD_EDITORUPDATE switcher_editor_update
#define METHOD_CANEDIT		switcher_can_edit
  
#define METHOD_GETHOTKEY    switcher_get_hotkey
#undef  METHOD_GETLABEL     

#undef  METHOD_INITBUTTON   

struct SwitcherLibData
{
    struct Library *gfxBase;
    struct Library *keymapBase;
};

struct SwitcherGadgetData
{
    struct Window *win;
	struct List items;
    UWORD width;
    UWORD height;
    UWORD closeTimer;
    UWORD bounceTimer;
    UWORD position;
    STRPTR hotkey;
};

#define POS_DOCK   0
#define POS_CENTRE 1

#define MARGIN_H 6
#define MARGIN_V 4

#define ICON_W      12
#define ICON_H       8
#define ICON_OFF     3

struct WindowListItem
{
    struct Node n;
    struct Screen *screen;
    struct Window *win;
    struct IntuiText text;
    struct TextAttr ta;
    UWORD rawKey;
    UWORD height;
    UWORD labelLen;
    BOOL selected;
};

#define FOR_EACH_ITEM(list, curr) FOR_EACH(struct WindowListItem, list, curr)

VOID switch_by_key(Object *o, struct SwitcherGadgetData *data, UWORD key);
VOID switch_by_mouse(Object *o, struct SwitcherGadgetData *data, UWORD yPos);
VOID toggle_window(Object *o, struct SwitcherGadgetData *data);
VOID close_window(Object *o, struct SwitcherGadgetData *data);
VOID open_window(Object *o, struct SwitcherGadgetData *data);


#endif
