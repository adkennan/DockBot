
EXENAME=	DockBot
EXEPATH=	//bin/WbStartup/

INC=		//include

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib 

C_OPTS=		INCLUDEDIRECTORY=$(INC)

C_SOURCES=	main.c dock.c dock_gadget.c dock_handle.c dock_button.c events.c gadgets.c layout.c settings.c

OBJECTS=	main.o dock.o dock_gadget.o dock_handle.o dock_button.o events.o gadgets.o layout.o settings.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS)
<

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME)
