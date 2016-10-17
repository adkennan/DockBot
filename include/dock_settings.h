/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCK_SETTINGS_H__
#define __DOCK_SETTINGS_H__

#include <exec/types.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>

#include <string.h>

struct Values
{
    STRPTR Name;
    ULONG Value;
};

#define IS_KEY(key, setting) (strncmp(key, setting.Key, strlen(key)) == 0)

#define GET_STRING(setting, dest) \
    dest = (STRPTR)DB_AllocMem(setting.ValueLength + 1, MEMF_CLEAR); \ 
    CopyMem(setting.Value, dest, setting.ValueLength); \
    dest[setting.ValueLength] = '\0'; 

#define GET_VALUE(setting, values, curr, len, dest) \
    curr = &values[0];\
    while( curr->Name ) { \
        len = strlen(curr->Name);\
        if( len == setting.ValueLength && strncmp(setting.Value, curr->Name, len) == 0) { \
            dest = curr->Value;\
            break; \
        }\
        curr++; \
    }

#define FREE_STRING(str) \
    if( str ) { \
        DB_FreeMem(str, strlen(str) + 1); \
        str = NULL; \
    }


#define CONFIG_FILE "ENV:DockBot.prefs"
#define CONFIG_FILE_PERM "ENVARC:DockBot.prefs"

#define S_ALIGN "align"
#define S_POSITION "position"
#define S_GADGET "gadget"


#endif
