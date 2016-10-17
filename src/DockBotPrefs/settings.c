
#include "prefs.h"

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include <stdio.h>

#include "dock_settings.h"

#include "dockbot_protos.h"
#include "dockbot_pragmas.h"

#include <proto/triton.h>

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

struct DgNode *add_dock_gadget(struct DockPrefs *prefs, Object *dg, STRPTR name)
{
    struct DgNode *n;

    if( n = DB_AllocMem(sizeof(struct DgNode), MEMF_CLEAR) ) {

        n->gadgetName = name;

        n->n.ln_Name = dock_gadget_get_name(dg);
        if( ! n->n.ln_Name ) {
            dock_gadget_set_name(dg, name);
            n->n.ln_Name = dock_gadget_get_name(dg);
        }
        n->dg = dg;

        AddTail((struct List *)&(prefs->gadgets), (struct Node *)n);
    }

    return n;
}

Object *create_dock_gadget(struct DockPrefs *prefs, STRPTR name)
{
    Object *o;
    char prefsClassName[50];
    char libName[50];
    struct Library *lib;
    struct LibNode *ln = NULL;

    sprintf(prefsClassName, "%sPrefs", name);

    if( ! (o = NewObjectA(NULL, prefsClassName, TAG_DONE) ) ) {

        sprintf(libName, "Gadgets/%s.class", prefsClassName);

        if( lib = OpenLibrary(libName, 1) ) {

            if( ln = (struct LibNode *)DB_AllocMem(sizeof(struct LibNode), MEMF_CLEAR) ) {

                if( o = NewObjectA(NULL, name, TAG_DONE) ) {

                    ln->lib = lib;
                    AddTail((struct List*)&(prefs->libs), (struct Node *)ln);

                    return o;
                } 
            }
        }

        if( !o && (o = NewObjectA(prefs->baseClass, NULL, TAG_DONE ) ) ) {
            return o;
        }      

        if( o ) {
            DisposeObject(o);
        }
        if( lib ) {
            CloseLibrary(lib);
        }
        if( ln ) {
            DB_FreeMem(ln, sizeof(struct LibNode));
        }
    }

    return o;
}


BOOL read_dock_gadget(struct DockPrefs *prefs, struct DockSettings *settings)
{
    struct DockSettingValue v;
    Object *gad;
    STRPTR gadName;

    while( DB_ReadSetting(settings, &v) ) {
        if( IS_KEY(S_GADGET, v) ) {

            GET_STRING(v, gadName);
            
            if( gad = create_dock_gadget(prefs, gadName) ) {

                dock_gadget_read_settings(gad, settings);
                add_dock_gadget(prefs, gad, gadName);

                return TRUE;                
            } else {
                printf("Can't create instance of %s\n", gadName);
            }

            FREE_STRING(gadName);
            break;
        }
    }
    return FALSE;
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


BOOL load_config(struct DockPrefs *prefs)
{
    struct DockSettings *s;
    struct DockSettingValue v;
    struct Values* vals;
    UWORD l;
    BOOL r;

    r = TRUE;
    if( s = DB_OpenSettingsRead(CONFIG_FILE) ) {

        if( DB_ReadBeginBlock(s) ) {

            while( TRUE ) {

                if( DB_ReadBeginBlock(s) ) {
                    if( ! read_dock_gadget(prefs, s) ) {
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
                        GET_VALUE(v, AlignValues, vals, l, prefs->align)
                    }

                    else if( IS_KEY(S_POSITION, v) ) {
                        GET_VALUE(v, PositionValues, vals, l, prefs->pos)
                    }
                }
            }
        }
        DB_CloseSettings(s);
    }
    return r;    
}

BOOL save_config(struct DockPrefs *prefs, BOOL writeToEnvarc)
{
    struct DockSettings *s = NULL;
    struct DgNode *curr;
    BOOL result = TRUE;

    if( s = DB_OpenSettingsWrite(
        writeToEnvarc ? CONFIG_FILE_PERM : CONFIG_FILE
    ) ) {
        
        if( DB_WriteBeginBlock(s) ) {

            if( ! DB_WriteSetting(s, S_ALIGN, get_name(AlignValues, prefs->align) ) ) {
                goto error;
            }

            if( ! DB_WriteSetting(s, S_POSITION, get_name(PositionValues, prefs->pos) ) ) {
                goto error;
            }

            for( curr = (struct DgNode *)prefs->gadgets.lh_Head;
                curr->n.ln_Succ;
                curr = (struct DgNode *)curr->n.ln_Succ ) {

                if( ! DB_WriteBeginBlock(s) ) {
                    goto error;
                }

                if( ! DB_WriteSetting(s, S_GADGET, curr->gadgetName) ) {
                    goto error;
                }

                dock_gadget_write_settings(curr->dg, s);

                if( ! DB_WriteEndBlock(s) ) {
                    goto error;
                }
            }

            if( ! DB_WriteEndBlock(s) ) {
                goto error;
            }
            
        } else {
            goto error;
        }

        goto exit;
    }

error:
    result = FALSE;

exit:
    if( s ) {
        DB_CloseSettings(s);
    }
    return result;
}

VOID remove_dock_gadgets(struct DockPrefs *prefs)
{
    struct DgNode *dg;

    while( ! IsListEmpty((struct List *)&prefs->gadgets) ) {
        if( dg = (struct DgNode *)RemTail((struct List *)&prefs->gadgets) ) {

            FREE_STRING(dg->gadgetName);

            DisposeObject(dg->dg);
            
            DB_FreeMem(dg, sizeof(struct DgNode));
        }
    }    
}

