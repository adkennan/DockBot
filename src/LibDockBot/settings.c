/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  � 2016 Andrew Kennan
**
************************************/

#include "dockbot.h"

#include "dockbot_protos.h"

#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>

#include <dos/dos.h>
#include <dos/dostags.h>
#include <clib/dos_protos.h>
#include <pragmas/dos_pragmas.h>

#include <intuition/classes.h>

#include <clib/alib_protos.h>

#include "lib.h"

#define INDENT "  "
#define SEPARATOR "="
#define ENDLINE "\n"
#define BEGIN "begin\n"
#define END "end\n"

struct Values AlignValues[] = {
    { "left", DA_LEFT },
    { "center", DA_CENTER },
    { "right", DA_RIGHT },
    { NULL, 0 }
};

struct Values PositionValues[] = {
    { "left", DP_LEFT },
    { "right", DP_RIGHT },
    { "top", DP_TOP },
    { "bottom", DP_BOTTOM },
    { NULL, 0 }
};

struct Values BooleanValues[] = {
    { "true", TRUE },
    { "false", FALSE },
    { NULL, 0 }
};

extern struct Library *DOSBase;
extern struct Library *SysBase;
extern struct DockBotLibrary *DockBotBase;

struct DockSettings
{    
    ULONG size;
    STRPTR buffer;
    UWORD pos;
    UWORD depth;
    BPTR fh;
};


struct DockSettings * __asm __saveds DB_OpenSettingsRead(
    register __a0 STRPTR filename)
{
    BPTR fh;
    struct FileInfoBlock *fib;
    struct DockSettings *s;

    s = NULL;
    
    if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {
           
        if( fh = Open(filename, MODE_OLDFILE) ) {

            if( ExamineFH(fh, fib) ) {
    
                if( s = (struct DockSettings *)AllocMemInternal(DockBotBase, sizeof(struct DockSettings), MEMF_CLEAR) ) {
                    
                    s->size = fib->fib_Size;
                    s->buffer = (STRPTR)AllocMemInternal(DockBotBase, s->size, MEMF_CLEAR);
                    s->pos = 0;
                    s->depth = 0;
                    s->fh = 0;
                    if( ! Read(fh, s->buffer, s->size) ) {
        
                        FreeMemInternal(DockBotBase, s->buffer, s->size);
                        FreeMemInternal(DockBotBase, s, sizeof(struct DockSettings));
                        s = NULL;
                    }
                }
            }
            Close(fh);
        }
        FreeDosObject(DOS_FIB, fib);
    }

    return s;
}

struct DockSettings * __asm __saveds DB_OpenSettingsWrite(
    register __a0 STRPTR filename)
{
    BPTR fh;
    struct DockSettings *s;

    s = NULL;
    if( fh = Open(filename, MODE_READWRITE) ) {
    
        SetFileSize(fh, 0, OFFSET_BEGINNING);

        if( s = (struct DockSettings *)AllocMemInternal(DockBotBase, sizeof(struct DockSettings), MEMF_CLEAR) ) {
            s->size = 0;
            s->buffer = NULL;
            s->pos = 0;
            s->depth = 0;  
            s->fh = fh;
        } else {
            Close(fh);            
        }    
    }

    return s;
}

VOID __asm __saveds DB_CloseSettings(
    register __a0 struct DockSettings *settings)
{
    if( settings->fh != 0 ) {
        Close(settings->fh);
    }

    if( settings->buffer ) {
        FreeMemInternal(DockBotBase, settings->buffer, settings->size);
    }
    FreeMemInternal(DockBotBase, settings, sizeof(struct DockSettings));
}

static BOOL WriteIndent(struct DockSettings *settings)
{
    LONG len;
    UWORD i;

    len = strlen(INDENT);
    for( i = 0; i < settings->depth; i++ ) {
        if( Write(settings->fh, INDENT, len) != len ) {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadString(struct DockSettings *settings, STRPTR str)
{
    UWORD i, len;
    STRPTR c, bufEnd;

    bufEnd = settings->buffer + settings->size;
    len = strlen(str);
    c = settings->buffer + settings->pos;

    for( i = 0; i < len; i++ ) {
        if( c == bufEnd || (*c) != str[i] ) {

            return FALSE;
        }   
        c++;
    }
    
    settings->pos += len;
    return TRUE;
}

static VOID ReadIndent(struct DockSettings *settings)
{
    UWORD i;
    STRPTR c, bufEnd;

    i = 0;
    bufEnd = settings->buffer + settings->size;
    c = settings->buffer + settings->pos;

    while(c < bufEnd && ((*c) == ' ' || (*c) == '\t')) {
        i++;
        c++;
    }

    settings->pos += i;
}


static BOOL ReadTo(struct DockSettings *settings, UBYTE b, UWORD *len, BOOL allowNewLine)
{
    UWORD i;
    STRPTR c, bufEnd;
    
    i = 0;
    bufEnd = settings->buffer + settings->size;
    c = settings->buffer + settings->pos;

    while( c < bufEnd && (allowNewLine || (*c) != '\n') ) {
        if( (*c) == b ) {
            (*len) = i;
            settings->pos += (i + 1);
            return TRUE;
        }
        c++;
        i++;
    }
    return FALSE;
}

BOOL __asm __saveds DB_ReadBeginBlock(
    register __a0 struct DockSettings *settings)
{
    UWORD pos;

    pos = settings->pos;

    ReadIndent(settings);

    if( ReadString(settings, BEGIN) ) {
        settings->depth++;
        return TRUE;
    }
    
    settings->pos = pos;
    return FALSE;
}

BOOL __asm __saveds DB_ReadEndBlock(
    register __a0 struct DockSettings *settings)
{
    UWORD pos;

    pos = settings->pos;
    
    ReadIndent(settings);

    if( ReadString(settings, END) ) {
        settings->depth--;
        return TRUE;
    }

    settings->pos = pos;

    return FALSE;
}

BOOL __asm __saveds DB_ReadSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 struct DockSettingValue *value)
{
    UWORD pos;

    pos = settings->pos;

    ReadIndent(settings);

    value->Key = settings->buffer + settings->pos;
    if( ! ReadTo(settings, SEPARATOR[0], &(value->KeyLength), FALSE) ) {
        settings->pos = pos;
        return FALSE;
    }

    value->Value = settings->buffer + settings->pos;
    if( ! ReadTo(settings, ENDLINE[0], &(value->ValueLength), TRUE) ) {
        settings->pos = pos;
        return FALSE;
    }
    
    return TRUE;
}

BOOL __asm __saveds DB_WriteBeginBlock(
    register __a0 struct DockSettings *settings)
{
    LONG len;

    if( ! WriteIndent(settings) ) {
        return FALSE;
    }

    len = strlen(BEGIN);
    if( Write(settings->fh, BEGIN, len) != len ) {
        return FALSE;
    }

    settings->depth++;
    return TRUE;
}

BOOL __asm __saveds DB_WriteEndBlock(
    register __a0 struct DockSettings *settings)
{
    LONG len;

    settings->depth--;

    if( ! WriteIndent(settings) ) {
        return FALSE;
    }

    len = strlen(END);
    if( Write(settings->fh, END, len) != len ) {
        return FALSE;
    }

    return TRUE;
}

BOOL __asm __saveds DB_WriteSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 STRPTR key, 
    register __a2 STRPTR value)
{
    LONG len;

    if( ! WriteIndent(settings) ) {
        return FALSE;
    }
    
    len = strlen(key);
    if( len != Write(settings->fh, key, len) ) {
        return FALSE;
    }
    settings->pos += len;

    len = strlen(SEPARATOR);
    if( Write(settings->fh, SEPARATOR, len) != len ) {
        return FALSE;
    } 
    settings->pos += len;

    len = strlen(value);
    if( Write(settings->fh, value, len) != len ) {
        return FALSE;
    }
    settings->pos += len;
    
    len = strlen(ENDLINE);
    if( Write(settings->fh, ENDLINE, len) != len ) {
        return FALSE;
    }

    settings->pos += len;

    return TRUE;
}


VOID dock_gadget_read_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageConfig msg = {
        DM_READCONFIG
    };
    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

VOID dock_gadget_write_settings(Object *obj, struct DockSettings *settings)
{
    struct DockMessageConfig msg = {
        DM_WRITECONFIG
    };
    msg.settings = settings;

    DoMethodA(obj, (Msg)&msg);
}

BOOL add_dock_gadget(struct DockConfig *cfg, Object *dg, STRPTR name)
{
    struct DgNode *n;

    if( n = AllocMemInternal(DockBotBase, sizeof(struct DgNode), MEMF_CLEAR) ) {
        n->n.ln_Name = name;
        n->dg = dg;
        AddTail(&cfg->gadgets, (struct Node *)n);

        return TRUE;
    }

    return FALSE;
}

BOOL read_dock_gadget(
    struct DockConfig *cfg,
    struct DockSettings *settings)
{
    struct DockSettingValue v;
    Object *gad;
    STRPTR gadName;
    BOOL r = FALSE;

    while( DB_ReadSetting(settings, &v) ) {
        if( IS_KEY(S_GADGET, v) ) {

            GET_STRING(v, gadName);
                        
            if( gad = DB_CreateDockGadget(gadName) ) {
                dock_gadget_read_settings(gad, settings);
                if( add_dock_gadget(cfg, gad, gadName) ) {

                    r = TRUE;                
                }
            }

            if( ! r ) {
                FREE_STRING(gadName);
            }
            break;
        }
    }
    return r;
}


STRPTR get_name(struct Values *values, UWORD val) {
    while( values->Name ) {
        if( values->Value == val ) {
            return values->Name;
        }
        values++;
    }
    return NULL;
}


BOOL __asm __saveds DB_ReadConfig(
    register __a0 struct DockConfig *cfg,
    register __a1 struct DockSettings *settings)
{
    struct DockSettingValue v;
    struct Values* vals;
    UWORD l;
    BOOL r;

    r = TRUE;
    if( DB_ReadBeginBlock(settings) ) {

        while( TRUE ) {

            if( DB_ReadBeginBlock(settings) ) {
                if( ! read_dock_gadget(cfg, settings) ) {
                    r = FALSE;
                    break;
                }
                if( !DB_ReadEndBlock(settings) ) {
                    r = FALSE;
                    break;
                }
            }
            if( DB_ReadEndBlock(settings) ) {
                break;
            }
            if( DB_ReadSetting(settings, &v) ) {
                if( IS_KEY(S_ALIGN, v) ) {
                    GET_VALUE(v, AlignValues, vals, l, cfg->align)
                }

                else if( IS_KEY(S_POSITION, v) ) {
                    GET_VALUE(v, PositionValues, vals, l, cfg->pos)
                }

                else if( IS_KEY(S_LABELS, v) ) {
                    GET_VALUE(v, BooleanValues, vals, l, cfg->showGadgetLabels)
                }
            }
        }
    }

    return r;
}

BOOL __asm __saveds DB_WriteConfig(
    register __a0 struct DockConfig *cfg,
    register __a1 struct DockSettings *settings)
{
    BOOL r = TRUE;
    struct DgNode *curr;

    if( DB_WriteBeginBlock(settings) ) {

        if( ! DB_WriteSetting(settings, S_ALIGN, get_name(AlignValues, cfg->align) ) ) {
            goto error;
        }

        if( ! DB_WriteSetting(settings, S_POSITION, get_name(PositionValues, cfg->pos) ) ) {
            goto error;
        }

        if( ! DB_WriteSetting(settings, S_LABELS, get_name(BooleanValues, cfg->showGadgetLabels) ) ) {
            goto error;
        }

        FOR_EACH_GADGET(&cfg->gadgets, curr) {

            if( ! DB_WriteBeginBlock(settings) ) {
                goto error;
            }

            if( ! DB_WriteSetting(settings, S_GADGET, curr->n.ln_Name) ) {
                goto error;
            }

            dock_gadget_write_settings(curr->dg, settings);

            if( ! DB_WriteEndBlock(settings) ) {
                goto error;
            }
        }

        if( ! DB_WriteEndBlock(settings) ) {
            goto error;
        }
            
    } else {
        goto error;
    }

    goto exit;

error:
    r = FALSE;
    
exit:
    return r;   
}
