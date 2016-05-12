/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <pragmas/intuition_pragmas.h>

#include "dockbot.h"
#include "dockbot_protos.h"

extern struct Library *IntuitionBase;

VOID __asm __saveds DB_ShowMessage(
    register __a0 STRPTR message)
{
    struct EasyStruct es = {
        sizeof(struct EasyStruct),
        0,
        "Message",
        NULL,
        "Ok"
    };

    es.es_TextFormat = message;

    EasyRequest(NULL, &es, NULL);

}