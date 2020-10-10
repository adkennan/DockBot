LIBNAME=	DockNet
VERSION=	1
REVISION=	0

INC=		INCLUDEDIRECTORY=///include INCLUDEDIRECTORY=/
DIR=		/bin

LIBFILE=	$(LIBNAME).class
LIBDIR=		//$(DIR)/Gadgets/

FD_FILE=	/template.fd
PRAGMA_FILE=	pragmas.h

C_OPTS=		CPU=$(CPU) $(SC_OPTS) NOSTANDARDIO $(INC) $(DEF_DS) $(DEF_BV)

C_SOURCES=	lib.c net.c docknet_cat.c

OBJECTS=	lib.o net.o docknet_cat.o

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

################
#
# Build Library

$(LIBFILE): catalog $(PRAGMA_FILE) $(OBJECTS) $(LIBS)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIB $(LIBS) $(LD_OPTS)
<

$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

lib.c: /template.c
	$(COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

catalog: //Locale/Catalogs/DockNet.cd
	@Execute << 
		flexcat //Locale/Catalogs/DockNet.cd docknet_cat.c=//Locale/Class_c.sd NOBEEP
		flexcat //Locale/Catalogs/DockNet.cd docknet_cat.h=//Locale/Class_h.sd NOBEEP
		echo "Catalog Updated"
	<
