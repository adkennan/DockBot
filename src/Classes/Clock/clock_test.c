
#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/classes.h>
#include <intuition/intuition.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>

#include <stdio.h>

struct Library *DockGadgetBase;
struct Library *IntuitionBase;

int main(int argc, char **argv)
{   

    Object *o;

    if( IntuitionBase = OpenLibrary("intuition.library", 39) ) {
        printf("Opened intuition\n");
        if( DockGadgetBase = OpenLibrary("clock.dg.class", 1) ) {

            printf("Opened library: %x\n", DockGadgetBase);

            if( o = NewObjectA(NULL, "clock.dg", TAG_DONE) ) {
                printf("Created object\n");
            
                DisposeObject(o);
        
                printf("Disposed Object\n");
                

            } else {
                printf("Could not create object\n");
            }

            CloseLibrary(DockGadgetBase);

            printf("Closed library\n");

        } else {
          printf("Failed to open library\n");
        }
        CloseLibrary(IntuitionBase);
    }
    
}