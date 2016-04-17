/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCK_HANDLE_H__
#define __DOCK_HANDLE_H__

#include <intuition/classes.h>

Class *init_dock_handle_class(VOID);

VOID free_dock_handle_class(Class *c);

#endif