/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/asl.h>
#include <dos/dos.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>

#include <string.h>

#include "lib.h"

#define DEFAULT_PATH "SYS:"

extern struct DockBotLibrary *DockBotBaseFull;
 
VOID ShowErrorInternal(STRPTR message)
{

    struct EasyStruct req = {
        sizeof(struct EasyStruct),
        0,
        "Error",
        "%s",
        "Ok"
    };


    EasyRequest(NULL, &req, NULL, message);
}

VOID __asm __saveds DB_ShowError(
    register __a0 STRPTR message) 
{
    ShowErrorInternal(message);
}

STRPTR __asm __saveds DB_SelectFile(
    register __a0 STRPTR title,
    register __a1 STRPTR okText,
    register __a2 STRPTR cancelText,
    register __a3 STRPTR path)
{
    STRPTR initialDir, initialFile, selectedFile = NULL;
    BOOL freeInitial = FALSE;
    struct FileRequester *fr;
    UWORD initialLen, selectedLen;
    struct TagItem tags[] = {
        ASL_Hail, NULL /* title */,
        ASL_OKText, NULL /* okText */,
        ASL_CancelText, NULL /* cancelText */,
        ASL_Dir, NULL,
        ASL_File, NULL,
        TAG_DONE
    };        
    
    tags[0].ti_Data = (ULONG)title;
    tags[1].ti_Data = (ULONG)okText;
    tags[2].ti_Data = (ULONG)cancelText;

    if( path ) {

        initialLen = strlen(path) + 1;
        if( initialDir = AllocMemInternal(DockBotBaseFull, initialLen, MEMF_CLEAR) ) {

            strcpy(initialDir, path);

            initialFile = FilePart(initialDir);
            *(initialFile - 1) = '\0';

            tags[4].ti_Data = (ULONG)initialFile;
            tags[3].ti_Data = (ULONG)initialDir;
            
            freeInitial = TRUE;
        }
    } else {
        tags[3].ti_Data = (ULONG)DEFAULT_PATH;
        tags[4].ti_Tag = TAG_DONE;
    }

    if( fr = (struct FileRequester *)AllocAslRequest(ASL_FileRequest, tags) ) {

        if( AslRequest(fr, NULL) ) {

            selectedLen = strlen(fr->rf_Dir) + strlen(fr->rf_File) + 2;
            if( selectedFile = AllocMemInternal(DockBotBaseFull, selectedLen, MEMF_ANY) ) {
              
                strcpy(selectedFile, fr->rf_Dir);

                AddPart(selectedFile, fr->rf_File, selectedLen);
            }
        }
        FreeAslRequest(fr);
    }

    if( freeInitial ) {
        FreeMemInternal(DockBotBaseFull, (VOID *)tags[4].ti_Data, initialLen);
    }

    return selectedFile;
}

