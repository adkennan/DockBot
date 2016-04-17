/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCK_BUTTON_H__
#define __DOCK_BUTTON_H__

#include <intuition/classes.h>

Class *init_dock_button_class(VOID);

VOID free_dock_button_class(Class *c);

#endif