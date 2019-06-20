/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __BUTTON_H__
#define __BUTTON_H__


#include <dos/dos.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <libraries/triton.h>
#include <proto/triton.h>

/****
** Icon Library v44+
*/
extern struct Library *IconBase;

#define CONST

#include "iconlib/icon.h"
#include "iconlib/icon_protos.h"
#include "iconlib/icon_pragmas.h"

/**
****/

#define TROB_Icon TRTG_PRVCLS(1)

#define TRAT_Icon_DiskObj TRTG_PRVOAT(1)

extern struct Library *DockBotBase;

BOOL init_icon_class(TR_App *app);

#endif