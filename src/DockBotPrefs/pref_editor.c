
#include "pref_editor.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <exec/memory.h>

#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>

#include <clib/alib_protos.h>

#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>

#include <string.h>

#include <stdio.h>

#include "button_editor.h"

extern struct Library *MUIMasterBase;

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


Object *get_settings_class(struct GadgetList *config, Object *o, STRPTR gadName)
{
//    STRPTR editorName;

    if( strcmp(gadName, "dockbutton") ) {
        return MUI_NewObject(BUTTON_EDITOR_CLASS, NULL);

//    } else {
//        editorName = (STRPTR)DoMethod(o, DM_SETTINGS_CLASS);
    }

    return NULL;
}

VOID read_gadget_settings(Object *editor, struct DockSettings *s) {
    struct DockMessageReadSettings msg = {
        DM_READCONFIG
    };

    DoMethodA(editor, (Msg)&msg);
}

struct DockGadgetInfo *get_gadget_info(struct GadgetList *config, struct DockSettings *s, STRPTR gadName)
{
    Object *o;
    char libName[50];
    struct Library *lib = NULL;
    UWORD len;
    STRPTR suffix = ".class";
    struct DockGadgetInfo *gi;
    
    if( gi = (struct DockGadgetInfo *)DB_AllocMem(sizeof(struct DockGadgetInfo), MEMF_CLEAR) ) {

        if( ! (o = NewObjectA(NULL, gadName, TAG_DONE) ) ) {

            len = strlen(gadName);
            CopyMem(gadName, &libName, len);
            CopyMem(suffix, &libName[len], strlen(suffix) + 1);

            if( lib = OpenLibrary(libName, 1) ) {
                o = NewObjectA(NULL, gadName, TAG_DONE);
            }
        }

        if( o ) {
            gi->editor = get_settings_class(config, o, gadName);

            if( gi->editor ) {
                read_gadget_settings(gi->editor, s);
            }

            DisposeObject(o);
        }
        if( lib ) {
            CloseLibrary(lib);
        }
    }        

    return gi;
}


BOOL read_dock_gadget(struct GadgetList *config, struct DockSettings *s)
{
    struct DockSettingValue v;

    STRPTR gadName;
    struct DockGadgetInfo *gi;

    while( DB_ReadSetting(s, &v) ) {
        if( IS_KEY(S_GADGET, v) ) {

            GET_STRING(v, gadName);
                            
            if( !(gi = get_gadget_info(config, s, gadName) ) ) {
                FREE_STRING(gadName);
                return FALSE;
            }

            AddTail((struct List *)&(config->gadgets), (struct Node *)gi);            
        }
    }
    return TRUE;
}


BOOL read_config(struct GadgetList *config)
{
    struct DockSettings *s;
    struct DockSettingValue v;
    struct Values* vals;
    UWORD l;
    BOOL r = TRUE;

    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        if( DB_ReadBeginBlock(s) ) {

            while( TRUE ) {

                if( DB_ReadBeginBlock(s) ) {
                    if( ! read_dock_gadget(config, s) ) {
                        r = FALSE;
                        break;
                    }
                    if( !DB_ReadEndBlock(s) ) {
                        r = FALSE;
                        break;
                    }
                }
                if( DB_ReadEndBlock(s) ) {
                    break;
                }
                if( DB_ReadSetting(s, &v) ) {
                    if( IS_KEY(S_ALIGN, v) ) {
                        GET_VALUE(v, AlignValues, vals, l, config->align)
                    }

                    else if( IS_KEY(S_POSITION, v) ) {
                        GET_VALUE(v, PositionValues, vals, l, config->pos)
                    }
                }
            }
        }

        DB_CloseSettings(s);
    }
    return r;    
}


struct GadgetList* load_config(VOID)
{
    struct GadgetList *config;

    if( config = (struct GadgetList *)DB_AllocMem(sizeof(struct GadgetList), MEMF_CLEAR) ) {

        config->pos = DP_BOTTOM;
        config->align = DA_LEFT;
        NewList((struct List*)&(config->libs));
        NewList((struct List*)&(config->gadgets));

        if( read_config(config) ) {
            return config;
        }

        DB_FreeMem(config, sizeof(struct GadgetList));
    }
    return NULL;
}

VOID free_config(struct GadgetList *config) 
{
    struct DockGadgetInfo *gi;

    if( config ) {

        while( ! IsListEmpty((struct List *)&config->gadgets) ) {
            if( gi = (struct DockGadgetInfo *)RemTail((struct List *)&config->gadgets) ) {

                DisposeObject(gi->editor);

                DB_FreeMem(gi, sizeof(struct DockGadgetInfo));
            }            
        }

        DB_FreeMem(config, sizeof(struct GadgetList));
    }
}


