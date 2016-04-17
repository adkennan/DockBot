/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __GADGET_MSG_H__
#define __GADGET_MSG_H__

#include <exec/ports.h>
#include <intuition/classes.h>

typedef enum {
    GM_DRAW         = 0x1000,
    GM_QUIT         = 0x1001
} GadgetMessageType;

struct GadgetMessage {
    struct Message m;
    GadgetMessageType messageType;
    Object* sender;
};

#endif