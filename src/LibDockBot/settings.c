/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
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

#define MAX_PADDING 8

extern struct Library *DOSBase;
extern struct Library *SysBase;
extern struct DockBotLibrary *DockBotBaseFull;

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
    
                if( s = (struct DockSettings *)AllocMemInternal(DockBotBaseFull, sizeof(struct DockSettings), MEMF_CLEAR) ) {
                    
                    s->size = fib->fib_Size;
                    s->buffer = (STRPTR)AllocMemInternal(DockBotBaseFull, s->size, MEMF_CLEAR);
                    s->pos = 0;
                    s->depth = 0;
                    s->fh = 0;
                    if( ! Read(fh, s->buffer, s->size) ) {
        
                        FreeMemInternal(DockBotBaseFull, s->buffer, s->size);
                        FreeMemInternal(DockBotBaseFull, s, sizeof(struct DockSettings));
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

        if( s = (struct DockSettings *)AllocMemInternal(DockBotBaseFull, sizeof(struct DockSettings), MEMF_CLEAR) ) {
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
        FreeMemInternal(DockBotBaseFull, settings->buffer, settings->size);
    }
    FreeMemInternal(DockBotBaseFull, settings, sizeof(struct DockSettings));
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

BOOL add_dock_gadget(struct DockConfig *cfg, struct DockSettings *settings, STRPTR name)
{
    struct DgNode *n;
   
    if( n = DB_AllocGadget(name) ) {
    
        dock_gadget_read_settings(n->dg, settings);

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
    STRPTR gadName;
    BOOL r = FALSE;

    while( DB_ReadSetting(settings, &v) ) {
        if( IS_KEY(S_GADGET, v) ) {

            GET_STRING(v, gadName);
                        
            r = add_dock_gadget(cfg, settings, gadName);

            FREE_STRING(gadName);

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

                else if( IS_KEY(S_BORDERS, v) ) {
                    GET_VALUE(v, BooleanValues, vals, l, cfg->showGadgetBorders)
                }

                else if( IS_KEY(S_BACKGROUND, v) ) {
                    GET_STRING(v, cfg->bgBrushPath)
                }

                else if( IS_KEY(S_PADDING, v) ) {
                    GET_INTEGER(v, cfg->gadgetPadding)
                    if( cfg->gadgetPadding > MAX_PADDING ) {
                        cfg->gadgetPadding = MAX_PADDING;
                    }
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
    UBYTE padding[3] = {0,0,0};

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

        if( ! DB_WriteSetting(settings, S_BORDERS, get_name(BooleanValues, cfg->showGadgetBorders) ) ) {
            goto error;
        }

        if( cfg->bgBrushPath && ! DB_WriteSetting(settings, S_BACKGROUND, cfg->bgBrushPath) ) {
            goto error;
        }

        padding[0] = '0' + (UBYTE)cfg->gadgetPadding;
        if( ! DB_WriteSetting(settings, S_PADDING, (STRPTR)&padding) ) {
            goto error;
        }

        FOR_EACH_GADGET(&cfg->gadgets, curr) {

            if( DoMethod(curr->dg, DM_BUILTIN) ) {
                continue;
            }

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

struct DgNode * __asm __saveds DB_AllocGadget(
    register __a0 STRPTR name)
{
    Object *dg;
    struct DgNode *n;
    UWORD l;
    STRPTR nc;

    l = strlen(name) + 1;
    if( nc = AllocMemInternal(DockBotBaseFull, l, MEMF_CLEAR) ) {

        CopyMem(name, nc, l);
    
        if( dg = DB_CreateDockGadget(nc) ) {

            if( n = AllocMemInternal(DockBotBaseFull, sizeof(struct DgNode), MEMF_CLEAR) ) {
        
                n->n.ln_Name = nc;
                n->dg = dg;

                NewList((struct List *)&n->ports);
        
                return n;
            }

            DB_DisposeDockGadget(dg);
        }

        FreeMemInternal(DockBotBaseFull, nc, l);
    }

    return NULL;
}

VOID __asm __saveds DB_FreeGadget(
    register __a0 struct DgNode *dg)
{
    struct PortReg *pr;

    DEBUG(DebugLog(__FUNC__ ": Freeing %s\n", dg->n.ln_Name));
   
    FREE_STRING(dg->n.ln_Name);

    DB_DisposeDockGadget(dg->dg);
           
    while( !IsListEmpty((struct List *)&dg->ports) ) {

        if( pr = (struct PortReg *)RemTail((struct List *)&dg->ports) ) {

            FreeMemInternal(DockBotBaseFull, pr, sizeof(struct PortReg));
        }
    }

    FreeMemInternal(DockBotBaseFull, dg, sizeof(struct DgNode));    

    DEBUG(DebugLog(__FUNC__ ": Done.\n"));
}

VOID __asm __saveds DB_DisposeConfig(
    register __a0 struct DockConfig *cfg)
{
    struct DgNode *dg;

    while( ! IsListEmpty(&cfg->gadgets) ) {
    
        if( dg = (struct DgNode *)RemTail(&cfg->gadgets) ) {

            DB_FreeGadget(dg);
        }
    }    
}

