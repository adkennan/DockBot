
#ifndef __DOCK_GADGET_H__
#define __DOCK_GADGET_H__

#include "dock.h"

#include <intuition/intuition.h>
#include <intuition/classes.h>

Class *init_dock_gadget_class(VOID);

ULONG free_dock_gadget_class(Class * c);

VOID dock_gadget_added(Object *obj, struct MsgPort *dockPort);

VOID dock_gadget_removed(Object *obj);

VOID dock_gadget_tick(Object *obj);

VOID dock_gadget_click(Object *obj, UWORD x, UWORD y);

VOID dock_gadget_drop(Object *obj, STRPTR path);

VOID dock_gadget_set_bounds(Object *obj, struct Rect *bounds);

VOID dock_gadget_get_bounds(Object *obj, struct Rect *bounds);

VOID dock_gadget_get_size(Object *obj
			, DockPosition position, DockAlign align
			, UWORD *w, UWORD *h);

VOID dock_gadget_draw(Object *obj, struct RastPort *rp);

BOOL dock_gadget_hit_test(Object *obj, UWORD x, UWORD y);

VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings);

#endif 
