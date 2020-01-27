/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/cghooks.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/graphics.h>
#include <clib/utility_protos.h>

#include <stdio.h>
#include <string.h>

#include "dockbot.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <libraries/triton.h>
#include <proto/triton.h>

#include "class_def.h"

#include "dockclock_cat.h"

extern struct GfxBase *GfxBase;

#define S_FORMAT "format"
#define DEFAULT_FORMAT "%Q:%M"

#define DATE_PREFS "SYS:Prefs/Time"

enum {
    OBJ_STR_FORMAT = 2001
};  

struct TextLine {
    struct MinNode n;
    struct IntuiText text;
    struct TextAttr ta;
    STRPTR format;
    UWORD pos;
    UWORD width;
    UWORD height;
    BOOL changed;
    UBYTE output[MAX_DISPLAY_LENGTH];
};

#define FOR_EACH_LINE for( line = (struct TextLine *)data->lines.mlh_Head; \
                                         line->n.mln_Succ; \
                                         line = (struct TextLine *)line->n.mln_Succ )


VOID create_lines(struct ClockGadgetData *data) 
{
    struct TextLine *line;
    STRPTR pos;
    STRPTR start;
    UWORD len = strlen(data->format) + 1;

    if( !(data->splitFormat = (STRPTR)DB_AllocMem(len, MEMF_ANY) ) ) {
        return;
    }

    CopyMem(data->format, data->splitFormat, len);     

    start = data->splitFormat;
    pos = start;
    while( *pos ) {
        if( *pos == '%' && *(pos + 1) == 'n' ) {
            *pos = '\0';
            pos++;

            if( line = (struct TextLine *)DB_AllocMem(sizeof(struct TextLine), MEMF_CLEAR) ) {

                line->format = start;
                start = pos + 1;             

                AddTail((struct List *)&data->lines, (struct Node *)line);
            }
        }
        pos++;
    }

    if( pos != start ) {
        
        if( line = (struct TextLine *)DB_AllocMem(sizeof(struct TextLine), MEMF_CLEAR) ) {

            line->format = start;

            AddTail((struct List *)&data->lines, (struct Node *)line);
        }
    }
}

VOID measure_text(struct ClockGadgetData *data, struct DrawInfo *drawInfo, UWORD *w, UWORD *h)
{
    struct TextLine *line;
    
    *h = 0;
    *w = 0;
    FOR_EACH_LINE {

        line->text.ITextFont = &line->ta;
        line->text.ITextFont->ta_Name = drawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
        line->text.ITextFont->ta_YSize = drawInfo->dri_Font->tf_YSize;
        line->text.ITextFont->ta_Style = drawInfo->dri_Font->tf_Style;
        line->text.ITextFont->ta_Flags = drawInfo->dri_Font->tf_Flags;
        line->text.FrontPen = drawInfo->dri_Pens[TEXTPEN];
        line->text.BackPen = drawInfo->dri_Pens[BACKGROUNDPEN];
        line->text.DrawMode = JAM1;
        line->text.IText = (STRPTR)&line->output;
        line->text.NextText = NULL;

        line->width = IntuiTextLength(&line->text);

        if( line->n.mln_Succ ) {
            line->text.NextText = &((struct TextLine *)line->n.mln_Succ)->text;
        }

        if( line->width > *w ) {
            *w = line->width;
        }

        line->height = drawInfo->dri_Font->tf_YSize;
        *h += line->height;
    }
}


VOID __saveds __asm write_char(
    register __a0 struct Hook *hook,
    register __a2 void *object,
    register __a1 char c)
{
    struct TextLine *line = (struct TextLine *)hook->h_Data;

    if( line->pos < MAX_DISPLAY_LENGTH ) {

        if( c != line->output[line->pos] ) {       
            line->changed = TRUE;
            line->output[line->pos] = c;
        }
        line->pos++;
    }
}

BOOL format_time(struct ClockGadgetData *data, struct DateStamp *ds)
{
    struct TextLine *line;
    BOOL changed = FALSE;
    struct Hook hook = {
        { NULL, NULL },
        (unsigned long (*)())write_char,
        NULL,
        NULL
    };

    FOR_EACH_LINE
    {
        line->pos = 0;
        line->changed = FALSE;

        hook.h_Data = line;
    
        FormatDate(data->locale, line->format, ds, &hook);

        if( line->changed ) {
            changed = TRUE;
        }   
    }

    return changed;
}


ULONG __saveds clock_lib_init(struct ClockLibData* cld)
{
    if( cld->dosBase = OpenLibrary("dos.library", 37) ) {
        DOSBase = (struct DosLibrary *)cld->dosBase;
        if( cld->gfxBase = OpenLibrary("graphics.library", 37) ) {
            GfxBase = (struct GfxBase *)cld->gfxBase;
       
            return 1;
        }
        CloseLibrary(cld->dosBase);
    }
    cld->dosBase = NULL;
    return 0;
}

ULONG __saveds clock_lib_expunge(struct ClockLibData *cld)
{
    if( cld->gfxBase ) {
        CloseLibrary(cld->gfxBase);
    }

    if( cld->dosBase ) {
        CloseLibrary(cld->dosBase);
    }

    return 1;
}

DB_METHOD_D(NEW)

    if( data->locale = OpenLocale(NULL) ) {

        NewList((struct List *)&data->lines);

        return 1;
    }
    return 0;
}

DB_METHOD_D(DISPOSE)

    if( data->locale ) {
        CloseLocale(data->locale);
    }
    
    while( ! IsListEmpty((struct List *)&data->lines) ) {
        DB_FreeMem(RemHead((struct List *)&data->lines), sizeof(struct TextLine));
    }

    if( data->splitFormat ) {
        DB_FreeMem(data->splitFormat, strlen(data->format) + 1);
    }

    FREE_STRING(data->format);

    return 1;
}

DB_METHOD_DM(DRAW,DockMessageDraw)

    struct TextLine *line;
    struct Rect b;
    UWORD textW, textH, yPos, winX, winY;
    struct Screen *screen;
    struct DrawInfo *drawInfo;

    DB_GetDockGadgetBounds(o, &b, &winX, &winY);

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {
    
            measure_text(data, drawInfo, &textW, &textH);

            SetAPen(msg->rp, drawInfo->dri_Pens[BACKGROUNDPEN]);
            RectFill(msg->rp, b.x, b.y, b.w + b.x - 1, b.h + b.y - 1);

            if( data->clicked ) {
                DB_DrawInsetFrame(msg->rp, &b);
            } else {
                DB_DrawOutsetFrame(msg->rp, &b);
            }

            yPos = b.y + (b.h - textH) / 2;

            FOR_EACH_LINE
            {
                line->text.LeftEdge = b.x + (b.w - line->width) / 2;
                line->text.TopEdge = yPos;

                yPos += line->height;
            }   

            PrintIText(msg->rp, &((struct TextLine *)data->lines.mlh_Head)->text, 0, 0);

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     

    return 1;    
}

DB_METHOD_D(CLICK)

    DB_RequestDockGadgetDraw(o);

    data->clicked = TRUE;

    DB_RequestLaunch(o, DATE_PREFS, NULL, NULL, TRUE);   

    return 1;
}

DB_METHOD_D(TICK)

    struct DateStamp ds;

    if( data->counter > 0 ) {
        data->counter--;
        return 1;
    }

    data->counter = 20;

    DateStamp(&ds);
    
    if( format_time(data, &ds)) {
   
        DB_RequestDockGadgetDraw(o);
    }

    return 1;
}

DB_METHOD_D(LAUNCHED)

    data->clicked = FALSE;

    DB_RequestDockGadgetDraw(o);

    return 1;
}

DB_METHOD_DM(GETSIZE,DockMessageGetSize)

    struct DateStamp ds;
    UWORD w, h;
    struct Screen *screen;
    struct DrawInfo *drawInfo;

    if( screen = LockPubScreen(NULL) ) {
    
        if( drawInfo = GetScreenDrawInfo(screen) ) {

            DateStamp(&ds);
            format_time(data, &ds);
            measure_text(data, drawInfo, &w, &h);

            msg->w = w + 4;
            msg->h = h + 4;

            FreeScreenDrawInfo(screen, drawInfo);
        }

        UnlockPubScreen(NULL, screen);
    }     

    return 1;
}

DB_METHOD_DM(READCONFIG,DockMessageConfig)

    struct DockSettingValue v;

    while( DB_ReadSetting(msg->settings, &v) ) {
        if( IS_KEY(S_FORMAT, v) ) {
            GET_STRING(v, data->format)
        }
    }

    if( ! data->format ) {
        if( data->format = (STRPTR)DB_AllocMem(strlen(DEFAULT_FORMAT) + 1, MEMF_ANY) ) {
            CopyMem(DEFAULT_FORMAT, data->format, strlen(DEFAULT_FORMAT) + 1);
        }
    }

    create_lines(data);

    return 1;
}

DB_METHOD_DM(WRITECONFIG,DockMessageConfig)

    struct DockSettings *s = msg->settings;

    DB_WriteSetting(s, S_FORMAT, data->format);

    return 1;
}

DB_METHOD_DM(GETEDITOR,DockMessageGetEditor)

    if( ! data->format ) {
        if( data->format = (STRPTR)DB_AllocMem(strlen(DEFAULT_FORMAT) + 1, MEMF_CLEAR) ) {
            CopyMem(DEFAULT_FORMAT, data->format, strlen(DEFAULT_FORMAT) + 1);
        }
    }

    msg->uiTags = DB_MakeTagList(   
        VertGroupA,
            Space,
            LineArray,
                Space,
                BeginLine,
                    Space,
                    TextN(MSG_UI_Format),
                    Space,
                    StringGadget(data->format, OBJ_STR_FORMAT),
                    Space,
                EndLine,
                Space,
            EndArray,
        EndGroup,
        TAG_END);

    return 1;
}

DB_METHOD_DM(EDITORUPDATE,DockMessageEditorUpdate)

    STRPTR str;
    UWORD len;
    struct TR_Project *proj = msg->window;

    FREE_STRING(data->format);

    str = (STRPTR)TR_GetAttribute(proj, OBJ_STR_FORMAT, 0);
    if( str && (len = strlen(str)) ) {        
        data->format = (STRPTR)DB_AllocMem(len + 1, MEMF_ANY);
        CopyMem(str, data->format, len + 1);
    }

    return 1;
}

DB_METHOD_M(CANEDIT, DockMessageCanEdit)

    msg->canEdit = TRUE;

    return 1;
}
