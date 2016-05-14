
EXENAME=	DockBot
EXEPATH=	//bin/

INC=		//include

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib 

CPU=		68030
DEF_DEBUG_MEM=

C_OPTS=		opt noicons cpu=$(CPU) INCLUDEDIRECTORY=$(INC) $(DEF_DEBUG_MEM)

C_SOURCES=	main.c dock.c dock_gadget.c dock_handle.c dock_button.c events.c gadgets.c layout.c settings.c debug.c

OBJECTS=	main.o dock.o dock_gadget.o dock_handle.o dock_button.o events.o gadgets.o layout.o settings.o debug.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS
<
	@Copy /Images/$(EXENAME).info $(EXEPATH)
	@Copy /Images/$(EXENAME)Min.info $(EXEPATH)

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) $(EXEPATH)$(EXENAME).info $(EXEPATH)$(EXENAME)Min.info
