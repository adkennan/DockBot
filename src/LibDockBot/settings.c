/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include "dockbot.h"

#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>

#include <dos/dos.h>
#include <dos/dostags.h>
#include <clib/dos_protos.h>
#include <pragmas/dos_pragmas.h>

#define INDENT "  "
#define SEPARATOR "="
#define ENDLINE "\n"
#define BEGIN "begin\n"
#define END "end\n"

extern struct Library *DOSBase;
extern struct Library *SysBase;

struct DockSettings
{    
    ULONG size;
    STRPTR buffer;
    UWORD pos;
    UWORD depth;
    BPTR fh;
};

struct DockSettings * __asm __saveds OpenSettingsRead(
    register __a0 STRPTR filename)
{
    BPTR fh;
    struct FileInfoBlock *fib;
    struct DockSettings *s;

    s = NULL;
    
    if( fib = AllocDosObjectTags(DOS_FIB, TAG_DONE) ) {
           
        if( fh = Open(filename, MODE_OLDFILE) ) {

            if( ExamineFH(fh, fib) ) {
    
                if( s = (struct DockSettings *)AllocMem(sizeof(struct DockSettings), MEMF_CLEAR) ) {
                    
                    s->size = fib->fib_Size;
                    s->buffer = (STRPTR)AllocMem(s->size, MEMF_CLEAR);
                    s->pos = 0;
                    s->depth = 0;
                    s->fh = 0;
                    if( ! Read(fh, s->buffer, s->size) ) {
        
                        FreeMem(s->buffer, s->size);
                        FreeMem(s, sizeof(struct DockSettings));
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

struct DockSettings * __asm __saveds OpenSettingsWrite(
    register __a0 STRPTR filename)
{
    BPTR fh;
    struct DockSettings *s;

    s = NULL;
    if( fh = Open(filename, MODE_READWRITE) ) {
    
        if( s = (struct DockSettings *)AllocMem(sizeof(struct DockSettings *), MEMF_CLEAR) ) {
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

VOID __asm __saveds CloseSettings(
    register __a0 struct DockSettings *settings)
{
    if( settings->fh != 0 ) {
        Close(settings->fh);
    }

    if( settings->buffer ) {
        FreeMem(settings->buffer, settings->size);
    }
    FreeMem(settings, sizeof(struct DockSettings));
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

BOOL __asm __saveds ReadBeginBlock(
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

BOOL __asm __saveds ReadEndBlock(
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

BOOL __asm __saveds ReadSetting(
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

BOOL __asm __saveds WriteBeginBlock(
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

BOOL __asm __saveds WriteEndBlock(
    register __a0 struct DockSettings *settings)
{
    LONG len;

    if( ! WriteIndent(settings) ) {
        return FALSE;
    }

    len = strlen(END);
    if( Write(settings->fh, END, len) != len ) {
        return FALSE;
    }

    settings->depth--;
    return TRUE;
}

BOOL __asm __saveds WriteSetting(
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
