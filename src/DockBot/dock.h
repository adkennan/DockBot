/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __DOCK_H__
#define __DOCK_H__

#define APP_NAME        "DockBot"

#include <exec/types.h>
#include <exec/lists.h>
#include <dos/dos.h>
#include <dos/notify.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/classes.h>
#include <libraries/commodities.h>
#include <graphics/layers.h>

#include <stdio.h>

#include "dockbot.h"
#include "dockbot_protos.h"
#include "dockbot_pragmas.h"
#include "dockbot_cat.h"

#include "dock_gadget.h"

#include "debug.h"

#define DEFAULT_CONSOLE "NIL:"

#define COPY_STRING(src, dst) \
    l = strlen(src);\
    CopyMem(src, dst, l);\
    dst += l;\
    *dst = ' ';\
    dst++; 

#define COPY_STRING_QUOTED(src, dst) \
    l = strlen(src);\
    *dst = '"';\
    dst++;\
    CopyMem(src, dst, l);\
    dst += l;\
    *dst = '"';\
    dst++;\
    *dst = ' ';\
    dst++;

typedef enum {
    RS_STARTING = 1,
    RS_RUNNING = 2,
    RS_LOADING = 3,
	RS_ICONIFYING = 4,
	RS_UNICONIFYING = 5,
    RS_QUITTING = 6,
    RS_STOPPED = 7,
    RS_HIDING = 8,
    RS_SHOWING = 9,

    RS_START_EDIT = 10,
	RS_EDITING = 11,
    RS_STOP_EDIT = 12,
	RS_CHANGING = 13,

    RS_HIDDEN = 14
} RunState;

typedef enum {
    MI_ABOUT = 1,
    MI_OPEN_PREFS = 2,
    MI_QUIT = 3,
	MI_HIDE = 4,
    MI_HELP = 5,
	MI_EDIT = 6,
	MI_SAVE = 7,
    MI_REVERT = 8
} MenuIndex;

typedef enum {
	II_ARROW_LEFT = 0,
	II_ARROW_RIGHT = 1,
	II_ARROW_UP = 2,
	II_ARROW_DOWN = 3,
	II_DELETE = 4,
	II_ARROW_LEFT_INV = 5,
	II_ARROW_RIGHT_INV = 6,
	II_ARROW_UP_INV = 7,
	II_ARROW_DOWN_INV = 8,
	II_DELETE_INV = 9,
    II_NONE = 10
} IconIndex;

#define ICON_COUNT 10

typedef enum {
	EO_NONE = 0,
	EO_MOVE_UP = 1,
	EO_MOVE_DOWN = 2,
	EO_DELETE = 3
} EditOperation;

struct DockWindow 
{
	struct DockConfig cfg;

    // Window
	struct Window *win;
    struct AppWindow *appWin;
    struct MsgPort *awPort;
    struct Menu* menu;

    // Built in classes
    Class *handleClass;

    // Prefs file change notification
    struct MsgPort* notifyPort;
    struct NotifyRequest notifyReq; 
    BOOL notifyEnabled;
    BOOL notifySupported;

    // Timer
    struct timerequest *timerReq;
    struct MsgPort* timerPort;

    // Gadget communication
    struct MsgPort* gadgetPort;

    // AppIcon
    struct MsgPort *aiPort;
	struct DiskObject* iconObj;
	struct AppIcon* appIcon;

    // Public message port.
    struct MsgPort *pubPort;

    RunState runState;
    BOOL disableLayout;

	// Commodities
	struct MsgPort *cxPort;
	CxObj *cxBroker;

    // Screen Notifications
    APTR screenNotifyHandle;
    struct MsgPort *screenNotifyMsgPort;

    // Gadget name tooltips
    Object *hoverGad;
    struct Window *hoverWin;
    UWORD hoverCount;

    // Background image
    APTR bgBrush;

    // Path to WBRun
    UBYTE progPath[2048];

	// Editing
	APTR iconBrush;
	UWORD iconW;
	UWORD iconH;
	UWORD editCount;
	UWORD editOp;
	struct DgNode *editNode;

	// Rendering
    struct RastPort renderRP;
	struct Layer_Info *renderLI;
	struct Layer *renderL;
	struct BitMap *renderBM;
	UWORD renderW, renderH;
};

#define TIMER_INTERVAL 250L

#define HOVER_COUNT 8

#define MIN_ICON "PROGDIR:" APP_NAME "Min"

#define DOCK_HORIZONTAL(dock) (dock->cfg.pos == DP_TOP || dock->cfg.pos == DP_BOTTOM)

#define DOCK_EDITING(dock) (dock->runState == RS_EDITING || dock->runState == RS_CHANGING)

// dock.c - Startup/shutdown, general functions.

struct DockWindow *create_dock(VOID);

VOID free_dock(struct DockWindow *);

VOID execute_external(struct DockWindow* dock, STRPTR path, STRPTR args, STRPTR console, BOOL wb);

VOID open_help(struct DockWindow *dock);

VOID delete_port(struct MsgPort *port);

VOID show_about(struct DockWindow *dock);


// settings.c

BOOL load_config(struct DockWindow *dock);

VOID free_config(struct DockWindow *dock);

BOOL init_config_notification(struct DockWindow *dock);

VOID free_config_notification(struct DockWindow *dock);

VOID handle_notify_message(struct DockWindow *dock);

VOID load_icon_brushes(struct DockWindow *dock);

VOID free_icon_brushes(struct DockWindow *dock);

VOID save_config(struct DockWindow *dock);

VOID add_dropped_icon(struct DockWindow *dock, BPTR dir, STRPTR name);

// window.c - Window functions

VOID hide_dock_window(struct DockWindow* dock);

BOOL show_dock_window(struct DockWindow* dock);

VOID handle_drop_event(struct DockWindow* dock);

VOID handle_window_event(struct DockWindow *dock);

VOID open_settings(struct DockWindow *dock);

VOID handle_change_config(struct DockWindow *dock);

VOID update_settings_menu(struct DockWindow *dock);

VOID enable_notification(struct DockWindow *dock);

VOID disable_notification(struct DockWindow *dock);

// events.c 

VOID run_event_loop(struct DockWindow *);


// timer.c

BOOL init_timer_notification(struct DockWindow *dock);

VOID free_timer_notification(struct DockWindow *dock);

VOID set_timer(struct DockWindow *dock, ULONG milliseconds);

VOID handle_timer_message(struct DockWindow *dock);


// appicon.c

BOOL show_app_icon(struct DockWindow *dock);

VOID free_app_icon(struct DockWindow *dock);

VOID handle_icon_event(struct DockWindow *dock);


// gadgets.c

BOOL create_dock_handle(struct DockWindow *dock);

BOOL init_gadget_classes(struct DockWindow *dock);

BOOL free_gadget_classes(struct DockWindow *dock);

BOOL init_gadgets(struct DockWindow *dock);

VOID draw_gadgets(struct DockWindow *dock);

VOID draw_gadget(struct DockWindow *dock, Object *gadget);

VOID remove_dock_gadgets(struct DockWindow *dock);

struct DgNode *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y);

VOID update_hover_gadget(struct DockWindow *dock);

VOID handle_gadget_message(struct DockWindow *dock);

VOID remap_gadgets(struct DockWindow *dock);

ULONG get_custom_sigs(struct DockWindow *dock);

VOID handle_custom_message(struct DockWindow *dock, ULONG signal);

VOID hide_gadget_label(struct DockWindow *dock);

VOID update_entire_window(struct DockWindow *dock);

// layout.c

ULONG get_window_top(struct Screen* screen, DockPosition pos, DockAlign align, UWORD height);

ULONG get_window_left(struct Screen* screen, DockPosition pos, DockAlign align, UWORD width);

VOID layout_gadgets(struct DockWindow *dock);

VOID disable_layout(struct DockWindow *dock);

VOID enable_layout(struct DockWindow *dock);

// cx.c - Commodities Exchange handling

BOOL init_cx_broker(struct DockWindow *dock);

VOID free_cx_broker(struct DockWindow *dock);

VOID handle_cx_message(struct DockWindow *dock);


// screennotify.c - Handle screenmode changes

BOOL init_screennotify(struct DockWindow *dock);

BOOL free_screennotify(struct DockWindow *dock);

VOID handle_screennotify(struct DockWindow *dock);

#endif

