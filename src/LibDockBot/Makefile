
LIBNAME=	dockbot
VERSION=	1
REVISION=	0

INCDIR=		//include
INC=		IDIR=$(INCDIR)
DIR=		/bin

LIBFILE=	$(LIBNAME).library
LIBDIR=		/$(DIR)/

FD_FILE=	$(LIBNAME).fd

PRAGMA_FILE=	$(INCDIR)/$(LIBNAME)_pragmas.h

C_OPTS=		CPU=$(CPU) $(SC_OPTS) $(INC) $(DEF_DS) $(DEF_BV)

C_SOURCES=	lib.c gfx.c gadget.c settings.c memory.c dock_gadget.c classes.c req.c debug.c

OBJECTS=	lib.o gfx.o gadget.o settings.o memory.o dock_gadget.o classes.o req.o debug.o

################
#
# Build Library

$(LIBFILE): $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIBFD $(FD_FILE)
LIBVERSION $(VERSION)
LIBREVISION $(REVISION)
LIB $(LIBS) $(LD_OPTS)
<

$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

reload:
	@version $(LIBFILE)
	@flushlibs