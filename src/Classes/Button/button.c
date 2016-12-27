/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>

#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>


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

#include <stdio.h>

#include "dockbot.h"
#include "dock_settings.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include "class_def.h"

#define S_NAME      "name"
#define S_PATH      "path"
#define S_START     "start"
#define S_ARGS      "args"
#define S_CON       "console"
#define S_HOTKEY    "key"

#define ST_WB 0
#define ST_SH 1


struct Values StartValues[] = {
    { "wb", ST_WB },
    { "sh", ST_SH },
    { NULL, 0 }
};


#define DEFAULT_CONSOLE "NIL:"

#define WBSTART "C:WBRun"


#define COPY_STRING(src, dst) \
    l = strlen(src);\
    CopyMem(src, dst, l);\
    dst += l;\
    *dst = ' ';\
    dst++; 


struct Library *DOSBase;
struct Library *IconBase;

VOID dock_button_launch(struct ButtonGadgetData *dbd, Msg msg, STRPTR* dropNames, UWORD dropCount) 
{
    STRPTR cmd;
    STRPTR pos;
    STRPTR con;
    BPTR fhIn;
    BPTR fhOut;
    UWORD i, len, l;

    struct TagItem shellTags[] = {
        { SYS_UserShell, TRUE },
        { SYS_Asynch, TRUE },
        { SYS_Input, NULL },
        { SYS_Output, NULL },
        { TAG_DONE, 0 }
    };

    
    len = dbd->startType == ST_WB ? (strlen(WBSTART) + 1) : 0;
    len += strlen(dbd->path) + 1;
    if( dbd->args ) {
        len += strlen(dbd->args) + 1;
    }
    for( i = 0; i < dropCount; i++ ) {
        len += strlen(dropNames[i]) + 1;
    }
    if( cmd = (STRPTR)DB_AllocMem(len, MEMF_CLEAR) ) {
        
        pos = cmd;
        if( dbd->startType == ST_WB ) {
            COPY_STRING(WBSTART, pos);
        }
        
        COPY_STRING(dbd->path, pos);
                
        if( dbd->args ) {
            COPY_STRING(dbd->args, pos);
        }

        for( i = 0; i < dropCount; i++ ) {
            COPY_STRING(dropNames[i], pos);
        }
        pos--;
        *pos = '\0';

        con = dbd->con;
        if( con == NULL ) {
            con = DEFAULT_CONSOLE;
        }

        if( fhOut = Open(con, MODE_OLDFILE) ) {
            if( fhIn = Open(DEFAULT_CONSOLE, MODE_OLDFILE) ) {

                shellTags[2].ti_Data = fhIn;
                shellTags[3].ti_Data = fhOut;

                if( SystemTagList(cmd, (struct TagItem*)&shellTags) == -1 ) {
                    Close(fhIn);
                    Close(fhOut);
                }

            } else {
                Close(fhOut);
            }
        }
        DB_FreeMem(cmd, len);
    }
}

ULONG __saveds button_lib_init(struct ButtonLibData* cld)
{
    if( cld->dosBase = OpenLibrary("dos.library", 37) ) {
        DOSBase = (struct DosLibrary *)cld->dosBase;
        if( cld->iconBase = OpenLibrary("icon.library", 45) ) {
            IconBase = cld->iconBase;
            return 1;
        }
        CloseLibrary(cld->dosBase);
    }
    cld->iconBase = NULL;
    cld->dosBase = NULL;
    return 0;
}

ULONG __saveds button_lib_expunge(struct ButtonLibData *cld)
{
    if( cld->iconBase ) {
        CloseLibrary(cld->iconBase);
    }

    if( cld->dosBase ) {
        CloseLibrary(cld->dosBase);
    }

    return 1;
}

DB_METHOD_D(DISPOSE)
    FREE_STRING(data->name);
    FREE_STRING(data->path);
    FREE_STRING(data->args);
    FREE_STRING(data->con);    
    FREE_STRING(data->hotKey);

    if( data->diskObj ) {
        FreeDiskObject(data->diskObj);
    }

    return 1;
}

DB_METHOD_DM(DRAW, DockMessageDraw)

    struct Rect bounds;
    
    DB_GetDockGadgetBounds(o, &bounds);  

    if( bounds.w == 0 || bounds.h == 0 ) {
        return 1;
    }

    if( data->diskObj ) {

        DrawIconStateA(msg->rp, data->diskObj, 
                NULL, 
                bounds.x + (bounds.w - data->imageW) / 2, 
                bounds.y + (bounds.h - data->imageH) / 2,
                data->iconState, NULL); 
    }

    if( data->iconState == 0 ) {
        DB_DrawOutsetFrame(msg->rp, &bounds);
    } else {
        DB_DrawInsetFrame(msg->rp, &bounds);
    }

    return 1;
}

DB_METHOD_D(TICK)

    if( data->counter > 0 ) {
        data->counter--;
        if( data->counter <= 0 ) {
            data->iconState = 1 - data->iconState;
            DB_RequestDockGadgetDraw(o);
        }
    }

    return 1;
}

DB_METHOD_D(CLICK)

    dock_button_launch(data, m, NULL, 0);

    data->counter = 2;
    data->iconState = 1;
    DB_RequestDockGadgetDraw(o);

    return 1;
}

DB_METHOD_DM(DROP,DockMessageDrop)

    dock_button_launch(data, m, msg->paths, msg->pathCount);

    data->counter = 2;
    data->iconState = 1;
    DB_RequestDockGadgetDraw(o);

    return 1;
}

DB_METHOD_DM(GETSIZE,DockMessageGetSize)

    struct Rectangle r;

    msg->w = DEFAULT_SIZE;
    msg->h = DEFAULT_SIZE;

    if( data->diskObj ) {
        if( GetIconRectangleA(NULL, data->diskObj, NULL, &r, NULL) ) {
            msg->w = data->imageW = r.MaxX - r.MinX + 1;
            msg->h = data->imageH = r.MaxY - r.MinY + 1;
        }
    }

    return 1;
}

DB_METHOD_DM(READCONFIG,DockMessageConfig)
    
    struct DockSettingValue v;
    struct Values *vals;
    struct Screen *screen;
    UWORD len;

    while( DB_ReadSetting(msg->settings, &v) ) {
        
        if( IS_KEY(S_NAME, v) ) {
            GET_STRING(v, data->name)     
        }
        else if( IS_KEY(S_PATH, v) ) {
            GET_STRING(v, data->path)
        }
        else if( IS_KEY(S_START, v) ) {
            GET_VALUE(v, StartValues, vals, len, data->startType)
        }
        else if( IS_KEY(S_ARGS, v) ) {
            GET_STRING(v, data->args)
        }
        else if( IS_KEY(S_CON, v) ) {
            GET_STRING(v, data->con)
        }
        else if( IS_KEY(S_HOTKEY, v) ) {
            GET_STRING(v, data->hotKey)
        }
    }    

    if( data->diskObj = GetDiskObjectNew(data->path) ) {
        if( screen = LockPubScreen(NULL) ) {

            LayoutIconA(data->diskObj, screen, NULL);

            UnlockPubScreen(NULL, screen);
        }
    }

    return 1;
}

DB_METHOD_DM(GETHOTKEY,DockMessageGetHotKey)

    msg->hotKey = data->hotKey;

    return 1;
}

DB_METHOD_DM(GETLABEL,DockMessageGetLabel)

    msg->label = data->name;

    return 1;
}

