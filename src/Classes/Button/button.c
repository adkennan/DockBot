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

#include <clib/utility_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>

#include <stdio.h>

#include "class_def.h"

#include "button.h"
#include "dockbutton_cat.h"

#define S_NAME      "name"
#define S_PATH      "path"
#define S_BRUSH     "brush"
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

STRPTR startTypes[] = { NULL /*"Workbench"*/, NULL /*"Shell"*/, NULL };

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
    OBJ_STR_BRUSH,
    OBJ_STR_ARGS,
    OBJ_CYC_START,
    OBJ_STR_CON,
    OBJ_STR_HOTKEY,
    OBJ_ICON,
    OBJ_BTN_SELECT_TARGET,
    OBJ_BTN_SELECT_BRUSH,
    OBJ_TXT_PATH
};

struct Library *DOSBase;
struct Library *IconBase;
struct Library *GfxBase;
struct ButtonLibData *StaticData;

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

VOID load_icon(struct ButtonGadgetData *data, STRPTR path)
{
    struct Screen *screen;

    DEBUG(DB_Printf(__METHOD__ "path = %s\n", path));

    if( data->diskObj ) {
        FreeDiskObject(data->diskObj);
    }
    
    if( data->diskObj = GetDiskObjectNew(path) ) {
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

VOID select_target(struct ButtonGadgetData *data, struct TR_Project *window)
{
    STRPTR oldPath, newPath, newName;
    UWORD l;

    oldPath = data->selectedPath == NULL ? data->path : data->selectedPath;

    if( newPath = DB_SelectFile((STRPTR)MSG_FR_ChooseApplication
                                , (STRPTR)MSG_FR_OkText
                                , (STRPTR)MSG_FR_CancelText
                                , oldPath) ) {

        FREE_STRING(data->selectedPath);

        l = strlen(newPath) + 1;        
        if( data->selectedPath = (STRPTR)DB_AllocMem(l, MEMF_CLEAR) ) {
    
            CopyMem(newPath, data->selectedPath, l);
            TR_SetAttribute(window, OBJ_TXT_PATH, TRAT_Text, (ULONG)data->selectedPath);

            newName = FilePart(data->selectedPath);
            TR_SetAttribute(window, OBJ_STR_NAME, 0L, (ULONG)newName);
                
            load_icon(data, data->selectedPath);
            TR_SetAttribute(window, OBJ_ICON, TRAT_Icon_DiskObj, (ULONG)data->diskObj); 
        }

        FREE_STRING(newPath);
    }     
}

BOOL set_brush(struct ButtonGadgetData *data, struct TR_Project *window, STRPTR newPath)
{
    DEBUG(DB_Printf(__METHOD__ "newPath = %s\n", newPath));
    
    if( newPath ) {

        if( data->brushImg ) {
            DB_FreeBrush(data->brushImg);   
        }

        data->brushImg = DB_LoadBrush(newPath, BF_CREATE_MASK);

        TR_SetAttribute(window, OBJ_ICON, TRAT_Icon_Brush, (ULONG)data->brushImg);

        return TRUE;
    }

    return FALSE;
}

VOID select_brush(struct ButtonGadgetData *data, struct TR_Project *window)
{
    STRPTR oldPath, newPath;
    
    DEBUG(DB_Printf(__METHOD__ "\n"));

    oldPath = (STRPTR)TR_GetAttribute(window, OBJ_STR_BRUSH, 0L);

    if( newPath = DB_SelectFile((STRPTR)MSG_FR_ChooseBrush
                                , (STRPTR)MSG_FR_OkText
                                , (STRPTR)MSG_FR_CancelText
                                , oldPath) ) {

        DEBUG(DB_Printf(__METHOD__ "newPath = %s\n", newPath));

        if( set_brush(data, window, newPath) ) {
    
            DEBUG(DB_Printf(__METHOD__ "Set UI brush path\n"));            

            TR_SetAttribute(window, OBJ_STR_BRUSH, 0L, (ULONG)newPath);
        }

        DEBUG(DB_Printf(__METHOD__, "Free path string\n"));

        FREE_STRING(newPath);
    }

    DEBUG(DB_Printf(__METHOD__ "Done\n"));    
}

ULONG __saveds button_lib_init(struct ButtonLibData* cld)
{
    if( cld->dosBase = OpenLibrary("dos.library", 37) ) {

        DOSBase = cld->dosBase;

        if( cld->gfxBase = OpenLibrary("graphics.library", 37) ) {

            GfxBase = cld->gfxBase;

            if( cld->iconBase = OpenLibrary("icon.library", 46) ) {

                IconBase = cld->iconBase;

                return 1;
            }
            CloseLibrary(cld->gfxBase);
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

    if( cld->gfxBase ) {
        CloseLibrary(cld->gfxBase);
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
    FREE_STRING(data->brushPath);

    if( data->diskObj ) {
        FreeDiskObject(data->diskObj);
    }

    if( data->brushImg ) {
        DEBUG(DB_Printf(__METHOD__ "Free brush %lx\n", data->brushImg));
        DB_FreeBrush(data->brushImg);
    }

    return 1;
}

DB_METHOD_DM(DRAW, DockMessageDraw)

    struct GadgetEnvironment env;
    
    DB_GetDockGadgetEnvironment(o, &env);

    if( env.gadgetBounds.w == 0 || env.gadgetBounds.h == 0 ) {
        return 1;
    }

    if( data->brushImg ) {

        DB_DrawBrush(data->brushImg,
            msg->rp,
            0, 0,
            env.gadgetBounds.x + (env.gadgetBounds.w - data->imageW) / 2, 
            env.gadgetBounds.y + (env.gadgetBounds.h - data->imageH) / 2,
            data->imageW, data->imageH);

    }
    else if( data->diskObj ) {

        DrawIconState(msg->rp, data->diskObj, 
                NULL, 
                env.gadgetBounds.x + (env.gadgetBounds.w - data->imageW) / 2, 
                env.gadgetBounds.y + (env.gadgetBounds.h - data->imageH) / 2,
                data->iconState,
                ICONDRAWA_Borderless, TRUE, 
                ICONDRAWA_EraseBackground, FALSE,
                TAG_END); 
    }

    if( env.showBorders ) {
        if( data->iconState == 0 ) {
            DB_DrawOutsetFrame(msg->rp, &env.gadgetBounds);
        } else {
            DB_DrawInsetFrame(msg->rp, &env.gadgetBounds);
        }
    }
    return 1;
}

DB_METHOD_D(REMAP)

    struct Screen *screen;
    
    if( screen = LockPubScreen(NULL) ) {

        if( data->diskObj ) {
            LayoutIconA(data->diskObj, screen, NULL);
        }

        UnlockPubScreen(NULL, screen);
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

    if( data->brushImg ) {

        DB_GetBrushSize(data->brushImg, &data->imageW, &data->imageH);
        msg->w = data->imageW + 2;
        msg->h = data->imageH + 2;     

    } else if( data->diskObj ) {

        if( GetIconRectangle(NULL, data->diskObj, NULL, &r,
                ICONDRAWA_Borderless, TRUE, TAG_END) ) {

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
        else if( IS_KEY(S_BRUSH, v) ) {
            GET_STRING(v, data->brushPath)

            data->brushImg = DB_LoadBrush(data->brushPath, BF_CREATE_MASK);
        }
    }    

    load_icon(data, data->path);

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
    if( data->brushPath ) {
        DB_WriteSetting(s, S_BRUSH, data->brushPath);
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

    struct IconInit *ii = NULL;

    DEBUG(DB_Printf(__METHOD__ "name = %s, path = %s\n", data->name, data->path));

    if( ii = (struct IconInit *)DB_AllocMem(sizeof(struct IconInit), MEMF_ANY) ) {
        ii->DiskObj = data->diskObj;
        ii->Brush = data->brushImg;
    }

    startTypes[0] = (STRPTR)MSG_ST_Workbench;
    startTypes[1] = (STRPTR)MSG_ST_Shell;

    msg->uiTags = DB_MakeTagList(   
        VertGroupA,
            Space,
            HorizGroupC,
                Space,
                TROB_Icon, ii, ID(OBJ_ICON),
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
                    TextN(MSG_UI_Name),
                    Space,
                    HorizGroup,
                        StringGadget(data->name, OBJ_STR_NAME),                
                        GetFileButton(OBJ_BTN_SELECT_TARGET),
                    EndGroup,
                    Space,  
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_Brush),
                    Space,
                    HorizGroup,
                        StringGadget(data->brushPath, OBJ_STR_BRUSH),TRAT_Value,200L,
                        GetFileButton(OBJ_BTN_SELECT_BRUSH),
                    EndGroup,
                    Space,  
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_Args),
                    Space,
                    StringGadget(data->args, OBJ_STR_ARGS),TRAT_Value,200L,
                    Space,
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_Key),
                    Space,
                    StringGadget(data->hotKey, OBJ_STR_HOTKEY),
                    Space,
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_StartType),
                    Space,
                    CycleGadget(startTypes, data->startType, OBJ_CYC_START),
                    Space,  
                EndLine,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_Console),
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

                case OBJ_STR_BRUSH:
                    set_brush(data, msg->window, (STRPTR)msg->message->trm_Data);
                    break;

                default:
                    break;
            }
            break;

        case TRMS_ACTION:
            switch( msg->message->trm_ID ) {
                case OBJ_BTN_SELECT_TARGET:
                    select_target(data, msg->window);
                    break;
            
                case OBJ_BTN_SELECT_BRUSH:
                    select_brush(data, msg->window);
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
    FREE_STRING(data->brushPath)

    if( data->selectedPath ) {
        FREE_STRING(data->path)
        data->path = data->selectedPath;
        data->selectedPath = NULL;
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_NAME, 0);
    if( str && (len = strlen(str)) ) {        
        data->name = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->name, len + 1);
    }

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_BRUSH, 0);
    if( str && (len = strlen(str)) ) {        
        data->brushPath = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->brushPath, len + 1);
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

    DEBUG(DB_Printf(__METHOD__ "name = %s, path = %s\n", data->name, data->path));

    data->args = NULL;
    data->hotKey = NULL;
    data->con = NULL;

    load_icon(data, data->path);

    return 1;
}

DB_METHOD_M(LAUNCHED,DockMessageLaunch)

    FREE_STRING(msg->args);

    return 1;
}

