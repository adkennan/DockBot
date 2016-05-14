/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCK_GADGET_H__
#define __DOCK_GADGET_H__

#include <intuition/intuition.h>
#include <intuition/classes.h>

Class *init_dock_gadget_class(VOID);

BOOL free_dock_gadget_class(Class * c);

#endif 
