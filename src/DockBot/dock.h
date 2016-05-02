/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCK_H__
#define __DOCK_H__


#include <exec/types.h>
#include <exec/lists.h>
#include <dos/dos.h>
#include <dos/notify.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/classes.h>

#include "dockbot.h"

#include "dock.h"
#include "dock_settings.h"

typedef enum {
    RS_STARTING = 1,
    RS_RUNNING = 2,
    RS_LAYOUT = 3,
    RS_QUITTING = 4,
    RS_STOPPED = 5
} RunState;

struct DockWindow 
{
	DockPosition pos;
	DockAlign align;
	struct Window *win;
    struct AppWindow *appWin;
    struct MsgPort *awPort;
	struct MinList gadgets;
	struct MinList libs;
    Class *gadgetClass;
    Class *handleClass;
    Class *buttonClass;
    struct MsgPort* notifyPort;
    struct NotifyRequest notifyReq; 
    struct timerequest *timerReq;
    struct MsgPort* timerPort;
    struct MsgPort* gadgetPort;
    struct Menu* menu;
    RunState runState;
    BOOL disableLayout;
};

struct DgNode
{
    struct MinNode n;
    Object *dg;
};

struct LibNode
{
	struct MinNode n;
	struct Library *lib;	
};

#define TIMER_INTERVAL 250L

VOID log_memory(VOID);

// Settings

BOOL load_config(struct DockWindow *dock);

BOOL init_config_notification(struct DockWindow *dock);

// Window

struct DockWindow* create_dock_window(VOID);

VOID close_dock_window(struct DockWindow* dock);

BOOL init_dock_window(struct DockWindow *dock);

struct DockWindow* create_dock_window(VOID);

VOID close_dock_window(struct DockWindow* dock);

// Events

BOOL init_timer_notification(struct DockWindow *dock);

VOID set_timer(struct DockWindow *dock, ULONG milliseconds);

VOID handle_drop_event(struct DockWindow* dock);

VOID handle_window_event(struct DockWindow *dock);

VOID handle_notify_message(struct DockWindow *dock);

VOID handle_timer_message(struct DockWindow *dock);

VOID handle_gadget_message(struct DockWindow *dock);

VOID run_event_loop(struct DockWindow *dock);

// Gadgets

BOOL create_dock_handle(struct DockWindow *dock);

BOOL init_gadget_classes(struct DockWindow *dock);

BOOL init_gadgets(struct DockWindow *dock);

VOID draw_gadgets(struct DockWindow *dock);

VOID draw_gadget(struct DockWindow *dock, Object *gadget);

VOID add_dock_gadget(struct DockWindow *dock, Object *dg);

VOID remove_dock_gadget(struct DockWindow *dock, Object *dg);

VOID remove_dock_gadgets(struct DockWindow *dock);

Object *get_gadget_at(struct DockWindow *dock, UWORD x, UWORD y);

Object *create_dock_gadget(struct DockWindow *dock, STRPTR name);

VOID close_class_libs(struct DockWindow *dock);

// Layout


ULONG get_window_top(struct Screen* screen, DockPosition pos, DockAlign align, UWORD height);

ULONG get_window_left(struct Screen* screen, DockPosition pos, DockAlign align, UWORD width);

VOID layout_gadgets(struct DockWindow *dock);

VOID disable_layout(struct DockWindow *dock);

VOID enable_layout(struct DockWindow *dock);


#endif

