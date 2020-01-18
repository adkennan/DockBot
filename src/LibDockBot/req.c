/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include <exec/types.h>
#include <intuition/intuition.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

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

