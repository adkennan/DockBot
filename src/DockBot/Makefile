
INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

DIR=		/bin

EXENAME=	DockBot
EXEPATH=	/$(DIR)/

C_OPTS=		cpu=$(CPU) $(SC_OPTS) $(INC) $(DEF_DS) $(DEF_BV)

C_SOURCES=	main.c dock.c dockbot_cat.c /shared/dock_gadget.c dock_handle.c events.c gadgets.c layout.c settings.c debug.c screennotify.c window.c cx.c appicon.c timer.c

OBJECTS=	main.o dock.o dockbot_cat.o /shared/dock_gadget.o dock_handle.o events.o gadgets.o layout.o settings.o debug.o screennotify.o window.o cx.o appicon.o timer.o

$(EXENAME): catalog $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) $(LD_OPTS)
<

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) 

catalog: /Locale/Catalogs/DockBot.cd
	@Execute << 
		flexcat /Locale/Catalogs/DockBot.cd dockbot_cat.c=/Locale/App_c.sd NOBEEP
		flexcat /Locale/Catalogs/DockBot.cd dockbot_cat.h=/Locale/App_h.sd NOBEEP
		echo "Catalog Updated"
	<
