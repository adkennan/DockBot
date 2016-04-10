
#ifndef __DOCK_H__
#define __DOCK_H__

#include <exec/types.h>

#include <intuition/classes.h>

#include "dock.h"
#include "dockbot.h"

struct DockWindow; 

struct DockWindow* create_dock_window(VOID);

VOID close_dock_window(struct DockWindow* dock);

VOID run_event_loop(struct DockWindow *dock);

VOID add_dock_gadget(struct DockWindow *dock, Object *dg);

VOID remove_dock_gadget(struct DockWindow *dock, Object *dg);

VOID disable_layout(struct DockWindow *dock);

VOID enable_layout(struct DockWindow *dock);

#endif

