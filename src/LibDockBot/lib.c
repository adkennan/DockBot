
#include <exec/types.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>

int __saveds __UserLibInit(void);
void __saveds __UserLibCleanup(void);

struct Library *SysBase = NULL;
struct Library *DOSBase = NULL;
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
 

int __saveds __UserLibInit(void)
{
    SysBase = (*((void **)4));

    if(IntuitionBase = OpenLibrary("intuition.library", 37) ) {
        if( GfxBase = OpenLibrary("graphics.library", 37) ) {
            if( DosBase = OpenLibrary("dos.library', 37) ) {
                return 0;
            } else {
                CloseLibrary(GfxBase);
                CloseLibrary(IntuitionBase);
            }
        } else {
            CloseLibrary(IntuitionBase);
        }
    }    

    return 1;

}

void __saveds __UserLibCleanup(void)
{
    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
    CloseLibrary(DOSBase);
}


