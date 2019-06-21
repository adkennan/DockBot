/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>
#include <libraries/asl.h>

#include <clib/utility_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>
#include <clib/asl_protos.h>

#include <stdio.h>

#include "class_def.h"

#include "button.h"

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

STRPTR startTypes[] = { "Workbench", "Shell", NULL };

#define DEFAULT_PATH "SYS:"

#define COPY_STRING(src, dst) \
    l = strlen(src);\
    CopyMem(src, dst, l);\
    dst += l;\
    *dst = ' ';\
    dst++; 

#define COPY_STRING_QUOTED(src, dst) \
    l = strlen(src);\
    *dst = '"';\
    dst++;\
    CopyMem(src, dst, l);\
    dst += l;\
    *dst = '"';\
    dst++;\
    *dst = ' ';\
    dst++;

enum {
    OBJ_STR_NAME = 1001,
    OBJ_STR_ARGS,
    OBJ_CYC_START,
    OBJ_STR_CON,
    OBJ_STR_HOTKEY,
    OBJ_ICON,
    OBJ_BTN_SELECT,
    OBJ_TXT_PATH
};


struct Library *DOSBase;
struct Library *IconBase;
struct Library *UtilityBase;
struct Library *AslBase;
struct ButtonLibData *StaticData;

struct TagItem *make_tag_list(ULONG data, ...)
{
    struct TagItem *tags = (struct TagItem *)&data;

    return CloneTagItems(tags);
}

VOID dock_button_launch(Object *o, struct ButtonGadgetData *dbd, Msg msg, STRPTR* dropNames, UWORD dropCount) 
{
    STRPTR args = NULL;
    STRPTR pos;
    UWORD i, len = 0, l;

    if( dbd->args ) {
        len += strlen(dbd->args) + 1;
    }
    for( i = 0; i < dropCount; i++ ) {
        len += strlen(dropNames[i]) + 3;
    }
    if( len > 0 ) {
        if( !( args = (STRPTR)DB_AllocMem(len, MEMF_CLEAR) ) ) {
            return;
        }

        pos = args;
        if( dbd->args ) {
            COPY_STRING(dbd->args, pos);
        }

        for( i = 0; i < dropCount; i++ ) {
            COPY_STRING_QUOTED(dropNames[i], pos);
        }
        pos--;
        *pos = '\0'; 
    }

    DB_RequestLaunch(o, dbd->path, args, dbd->con, dbd->startType == ST_WB ? TRUE : FALSE);
}


VOID load_icon(struct ButtonGadgetData *data)
{
    struct Screen *screen;
    
    if( data->diskObj = GetDiskObjectNew(data->path) ) {
        if( screen = LockPubScreen(NULL) ) {

            LayoutIconA(data->diskObj, screen, NULL);

            UnlockPubScreen(NULL, screen);
        }
    }
}

STRPTR get_start_type(struct Values *values, UWORD val) {

    while( values->Name ) {
        if( values->Value == val ) {
            return values->Name;
        }
        values++;
    }
    return NULL;
}



VOID select_file(struct ButtonGadgetData *data, struct TR_Project *window)
{
    STRPTR buf, t1, t2, t3;
    BOOL freePath = FALSE;
    struct FileRequester *fr;
    ULONG len;
    struct TagItem tags[] = {
        { ASL_Hail, (ULONG)"Choose an application" },
        { ASL_OKText, (ULONG)"Select" },
        { ASL_CancelText, (ULONG)"Cancel" },
        { ASL_File, NULL },
        { ASL_Dir, NULL }
    };        

    if( data->path ) {

        t1 = PathPart(data->path);
        len = strlen(data->path) - strlen(t1) + 1;
        if( buf = DB_AllocMem(len, MEMF_CLEAR) ) {

            t2 = data->path;
            t3 = buf;
            while( t2 != t1 ) {
                *(t3++) = *(t2++);
            }

            tags[4].ti_Data = (ULONG)buf;
            freePath = TRUE;
        }
    } else {
        tags[4].ti_Data = (ULONG)DEFAULT_PATH;
    }

    if( fr = (struct FileRequester *)AllocAslRequest(ASL_FileRequest, tags) ) {
        if( AslRequest(fr, NULL) ) {

            FREE_STRING(data->path);

            if( buf = DB_AllocMem(1024, MEMF_ANY) ) {
                CopyMem(fr->rf_Dir, buf, strlen(fr->rf_Dir) + 1);
                if( AddPart(buf, fr->rf_File, 1024) ) {
                    len = strlen(buf) + 1;
                    if( data->path = (STRPTR)DB_AllocMem(len, MEMF_CLEAR) ) {
                        CopyMem(buf, data->path, len);
                    }
                }
                TR_SetAttribute(window, OBJ_TXT_PATH, TRAT_Text, (ULONG)data->path);
                DB_FreeMem(buf, 1024);
            }

            FREE_STRING(data->name);
            len = strlen(fr->rf_File) + 1;
            if( data->name = (STRPTR)DB_AllocMem(len, MEMF_CLEAR) ) {
                CopyMem(fr->rf_File, data->name, len);
            }
            TR_SetAttribute(window, OBJ_STR_NAME, 0L, (ULONG)data->name);
                
            if( data->diskObj ) {
                FreeDiskObject(data->diskObj);
            }
            load_icon(data);
            TR_SetAttribute(window, OBJ_ICON, TRAT_Icon_DiskObj, (ULONG)data->diskObj); 

        }
        FreeAslRequest(fr);
    }

    if( freePath ) {
        DB_FreeMem((VOID *)tags[4].ti_Data, strlen((STRPTR)tags[4].ti_Data) + 1);
    }
}


ULONG __saveds button_lib_init(struct ButtonLibData* cld)
{
    cld->aslBase = NULL;
    AslBase = NULL;

    if( cld->dosBase = OpenLibrary("dos.library", 37) ) {
        DOSBase = (struct DosLibrary *)cld->dosBase;
        if( cld->iconBase = OpenLibrary("icon.library", 46) ) {
            IconBase = cld->iconBase;
            if( cld->utilityBase = OpenLibrary("utility.library", 37) ) {
                UtilityBase = cld->utilityBase;
                if( cld->aslBase = OpenLibrary("asl.library", 37) ) {
                    AslBase = cld->aslBase;

                    return 1;
                }
                CloseLibrary(cld->utilityBase);
            }
            CloseLibrary(cld->iconBase);
        }
        CloseLibrary(cld->dosBase);
    }
    cld->utilityBase = NULL;
    cld->iconBase = NULL;
    cld->dosBase = NULL;
    cld->aslBase = NULL;
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

    if( cld->utilityBase ) {
        CloseLibrary(cld->utilityBase);
    }

    if( cld->aslBase ) {
        CloseLibrary(cld->aslBase);
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

    dock_button_launch(o, data, m, NULL, 0);

    data->counter = 2;
    data->iconState = 1;
    DB_RequestDockGadgetDraw(o);

    return 1;
}

DB_METHOD_DM(DROP,DockMessageDrop)

    dock_button_launch(o, data, m, msg->paths, msg->pathCount);

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

    load_icon(data);

    return 1;
}

DB_METHOD_DM(WRITECONFIG,DockMessageConfig)

    struct DockSettings *s = msg->settings;
    if( data->name ) {
        DB_WriteSetting(s, S_NAME, data->name);
    }
    if( data->path ) {
        DB_WriteSetting(s, S_PATH, data->path);
    }
    if( data->args ) {
        DB_WriteSetting(s, S_ARGS, data->args);
    }
    if( data->hotKey ) {
        DB_WriteSetting(s, S_HOTKEY, data->hotKey);
    }

    DB_WriteSetting(s, S_START, get_start_type(StartValues, data->startType));

    if( data->con ) {
        DB_WriteSetting(s, S_CON, data->con);
    }

    return 1;
}

DB_METHOD_M(INITEDIT, DockMessageEditorInit)

    if( ! init_icon_class(msg->app) ) { 
        return 0;
    }

    return 1;
}

DB_METHOD_M(CANEDIT, DockMessageCanEdit)

    msg->canEdit = TRUE;

    return 1;
}

DB_METHOD_DM(GETEDITOR, DockMessageGetEditor)

    msg->uiTags = make_tag_list(   
        VertGroupA,
            Space,
            HorizGroupC,
                Space,
                TROB_Icon, data->diskObj, ID(OBJ_ICON),
                Space,
            EndGroup,
            Space,
            HorizGroupSC,
                Space,
                TextT(data->path), TRAT_ID, OBJ_TXT_PATH,
                Space,
            EndGroup,
            Space,
            LineArray,
                Space,
                BeginLine,
                    Space,
                    TextN("Name"),
                    Space,
                    HorizGroup,
                        StringGadget(data->name, OBJ_STR_NAME),                
                        GetFileButton(OBJ_BTN_SELECT),
                    EndGroup,
                    Space,  
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN("Arguments"),
                    Space,
                    StringGadget(data->args, OBJ_STR_ARGS),
                    Space,
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN("Key"),
                    Space,
                    StringGadget(data->hotKey, OBJ_STR_HOTKEY),
                    Space,
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN("Start Type"),
                    Space,
                    CycleGadget(startTypes, data->startType, OBJ_CYC_START),
                    Space,  
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN("Console"),
                    Space,
                    StringGadget(data->con, OBJ_STR_CON),
                        TRAT_Disabled, (data->startType == ST_WB),
                    Space,
                EndLine,
            EndArray,
        EndGroup,
        TAG_END);

    return 1;
}

DB_METHOD_DM(EDITOREVENT, DockMessageEditorEvent)

    switch( msg->message->trm_Class ) {
        case TRMS_NEWVALUE:
            switch( msg->message->trm_ID ) {
                case OBJ_CYC_START:
                    TR_SetAttribute(msg->window, OBJ_STR_CON, TRAT_Disabled, msg->message->trm_Data == ST_WB);
                    break;

                default:
                    break;
            }
            break;

        case TRMS_ACTION:
            switch( msg->message->trm_ID ) {
                case OBJ_BTN_SELECT:
                    select_file(data, msg->window);
                    break;

                default:
                    break;
            }
            break; 

        default:
            break;
        
    }

    return 1;
}

DB_METHOD_DM(EDITORUPDATE, DockMessageEditorUpdate)

    STRPTR str;
    UWORD len;
    struct TR_Project *proj = msg->window;

    FREE_STRING(data->name)
    FREE_STRING(data->args)
    FREE_STRING(data->con)
    FREE_STRING(data->hotKey)

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_NAME, 0);
    if( str && (len = strlen(str)) ) {        
        data->name = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->name, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_HOTKEY, 0);
    if( str && (len = strlen(str)) ) {        
        data->hotKey = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->hotKey, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_ARGS, 0);
    if( str && (len = strlen(str)) ) {        
        data->args = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->args, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_CON, 0);
    if( str && (len = strlen(str)) ) {        
        data->con = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->con, len + 1);
    }
    
    data->startType = (UWORD)TR_GetAttribute(proj, OBJ_CYC_START, TRAT_Value);

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

DB_METHOD_DM(INITBUTTON,DockMessageInitButton)

    UWORD len;

    len = strlen(msg->name) + 1;
    if( ! (data->name = DB_AllocMem(len, MEMF_ANY) ) ) {
        return 0;
    }
    CopyMem(msg->name, data->name, len);

    len = strlen(msg->path) + 1;
    if( ! (data->path = DB_AllocMem(len, MEMF_ANY) ) ) {
        return 0;
    }
    CopyMem(msg->path, data->path, len);

    data->args = NULL;
    data->hotKey = NULL;
    data->con = NULL;

    load_icon(data);

    return 1;
}

DB_METHOD_M(LAUNCHED,DockMessageLaunch)

    FREE_STRING(msg->args);

    return 1;
}

