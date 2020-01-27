/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2020 Andrew Kennan
**
************************************/

#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#include <graphics/gfx.h>

#include "gadget_class.h"

// Strings catalogue
#include "eyes_cat.h"

// Class Information
#define CLASS_NAME      "Eyes"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (" DS ")"
#define CLASS_COPYRIGHT "© 2020 Andrew Kennan"

// Global data struct
#define GADGET_LIB_DATA     EyesLibData

// Instance data struct
#define PRIVATE_DATA        EyesGadgetData

// Global initialization and destruction
#define GADGET_LIB_INIT     eyes_lib_init
#define GADGET_LIB_EXPUNGE  eyes_lib_expunge

// Instance initialization and destruction
#define METHOD_NEW          eyes_new
#define METHOD_DISPOSE      eyes_dispose

// Called about every 250ms
#define METHOD_TICK         eyes_tick

// Query that returns the desired size of the gadget.
#define METHOD_GETSIZE      eyes_get_size

// Draws the gadget.
#define METHOD_DRAW         eyes_draw

// Config file handling
#define METHOD_READCONFIG   eys_read_config
#define METHOD_WRITECONFIG  eys_write_config

// Prefs editor handling
#define METHOD_GETEDITOR	eys_get_editor
#define METHOD_EDITORUPDATE eys_editor_update
#define METHOD_CANEDIT		eys_can_edit

// Global data stores pointers to libraries we need.
struct EyesLibData
{
	struct Library *gfxBase;
};

struct Eye {    
    UWORD sx, sy; // Screen position.
    UWORD cx, cy; // Centre relative to gadget top left.
    UWORD rx, ry; // Radius of eyeball.
    UWORD ix, iy; // Iris position relative to gadget top left.
};

// Instance data
struct EyesGadgetData 
{
    UWORD eyeCount;
    UWORD w, h;             // Dimensions of gadget
    UWORD d;                // Screen depth
    UWORD cx, cy;           // Position within window
    UWORD sx, sy;           // Position within screen
	UWORD aiBufSize, trBufSize;
    struct Eye *eyes;
	struct BitMap *bm;      // \                          |
	struct RastPort rp;     //  \                         |
	struct AreaInfo ai;     //   > For off-screen drawing |
	struct TmpRas tr;       //  /                         |
	UBYTE *aiBuf, *trBuf;   // /                          |
	BOOL initialized;
    BOOL needsRedraw;
};

#endif
