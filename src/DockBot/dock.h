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
#define APP_VERSION     "1.0"
#define APP_DESCRIPTION "A Dock For AmigaOS 3"
#define APP_COPYRIGHT   "© 2019 Andrew Kennan"

#include <exec/types.h>
#include <exec/lists.h>
#include <dos/dos.h>
#include <dos/notify.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/classes.h>
#include <libraries/commodities.h>

#include "dockbot.h"

typedef enum {
    RS_STARTING = 1,
    RS_RUNNING = 2,
    RS_LOADING = 3,
	RS_HIDING = 4,
	RS_SHOWING = 5,
    RS_QUITTING = 6,
    RS_STOPPED = 7
} RunState;

typedef enum {
    MI_ABOUT = 1,
    MI_SETTINGS = 2,
    MI_QUIT = 3,
	MI_HIDE = 4,
    MI_HELP = 5
} MenuIndex;

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

    // Gadget name tooltips
    Object *hoverGad;
    struct Window *hoverWin;
    UWORD hoverCount;

    // Path to WBRun
    UBYTE progPath[2048];
};

#define TIMER_INTERVAL 250L

#define HOVER_COUNT 8

#define MIN_ICON "PROGDIR:" APP_NAME "Min"

VOID log_memory(VOID);

// Settings

BOOL load_config(struct DockWindow *dock);

BOOL init_config_notification(struct DockWindow *dock);

// Window

struct DockWindow* create_dock(VOID);

VOID free_dock(struct DockWindow *dock);

VOID hide_dock_window(struct DockWindow* dock);

BOOL show_dock_window(struct DockWindow* dock);

VOID show_about(struct DockWindow *dock);

VOID delete_port(struct MsgPort *port);

// Events

BOOL init_timer_notification(struct DockWindow *dock);

VOID set_timer(struct DockWindow *dock, ULONG milliseconds);

VOID handle_drop_event(struct DockWindow* dock);

VOID handle_window_event(struct DockWindow *dock);

VOID handle_notify_message(struct DockWindow *dock);

VOID handle_timer_message(struct DockWindow *dock);

VOID handle_gadget_message(struct DockWindow *dock);

VOID run_event_loop(struct DockWindow *dock);

BOOL show_app_icon(struct DockWindow *dock);

VOID free_app_icon(struct DockWindow *dock);

// Gadgets

BOOL create_dock_handle(struct DockWindow *dock);

BOOL init_gadget_classes(struct DockWindow *dock);

BOOL free_gadget_classes(struct DockWindow *dock);

BOOL init_gadgets(struct DockWindow *dock);

VOID draw_gadgets(struct DockWindow *dock);

VOID draw_gadget(struct DockWindow *dock, Object *gadget);

VOID remove_dock_gadgets(struct DockWindow *dock);

Object *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y);

VOID update_hover_gadget(struct DockWindow *dock);

// Layout


ULONG get_window_top(struct Screen* screen, DockPosition pos, DockAlign align, UWORD height);

ULONG get_window_left(struct Screen* screen, DockPosition pos, DockAlign align, UWORD width);

VOID layout_gadgets(struct DockWindow *dock);

VOID disable_layout(struct DockWindow *dock);

VOID enable_layout(struct DockWindow *dock);

// Commodities

BOOL init_cx_broker(struct DockWindow *dock);

VOID free_cx_broker(struct DockWindow *dock);

#endif

