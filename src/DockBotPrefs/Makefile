
DIR=		/bin

EXENAME=	DockBotPrefs
EXEPATH=	/$(DIR)/

INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

CPU=		68030

DEF_DEBUG_MEM=
DEF_DEBUG=
DEF_STRIP=
DEF_OPT=
DEF_DEBUG=
DEF_DS=
DEF_BV=

C_OPTS=		noicons cpu=$(CPU) $(DEF_OPT) STRINGMERGE $(INC) $(DEF_DEBUG_MEM) $(DEF_DEBUG) $(DEF_DEBUG_OUT) $(DEF_DS) $(DEF_BV)

C_SOURCES=	main.c dockbotprefs_cat.c /shared/dock_gadget.c settings.c new_gadget.c gadget_list.c tags.c

OBJECTS=	main.o dockbotprefs_cat.o /shared/dock_gadget.o settings.o new_gadget.o gadget_list.o tags.o

$(EXENAME): catalog $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS $(DEF_STRIP) SMALLDATA
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
