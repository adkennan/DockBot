
INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

DIR=		/bin

EXENAME=	DockBotPrefs
EXEPATH=	/$(DIR)/

C_OPTS=		cpu=$(CPU) $(SC_OPTS) $(INC) $(DEF_DS) $(DEF_BV)

C_SOURCES=	main.c dockbotprefs_cat.c /shared/dock_gadget.c settings.c new_gadget.c gadget_list.c tags.c

OBJECTS=	main.o dockbotprefs_cat.o /shared/dock_gadget.o settings.o new_gadget.o gadget_list.o tags.o

LIBS=		$(LIBS) LIB:triton.lib

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

catalog: /Locale/Catalogs/DockBotPrefs.cd
	@Execute << 
		flexcat /Locale/Catalogs/DockBotPrefs.cd dockbotprefs_cat.c=C_c.sd NOBEEP
		flexcat /Locale/Catalogs/DockBotPrefs.cd dockbotprefs_cat.h=C_h.sd NOBEEP
		echo "Catalog Updated"
	<
