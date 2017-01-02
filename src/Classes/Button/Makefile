
LIBNAME=	DockButton
VERSION=	1
REVISION=	0

INC=		INCLUDEDIRECTORY=///include INCLUDEDIRECTORY=/

LIBFILE=	$(LIBNAME).class
LIBDIR=		///bin/Gadgets/

FD_CONV=	SC:c/fd2pragma
FD_FILE=	/template.fd
PRAGMA_FILE=	pragmas.h

C_COMPILER=	SC:c/sc

CPU=		68030
C_OPTS=		CPU=$(CPU) STREQ STRMER NOSTKCHK NOSTANDARDIO NOICONS $(INC)

LINKER=		SC:c/slink

C_SOURCES=	lib.c button.c triton_icon.c

OBJECTS=	lib.o button.o triton_icon.o

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

################
#
# Build Library

$(LIBFILE): $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIB $(LIBS) NOICONS
<

#LIBFD $(FD_FILE)
#LIBVERSION $(VERSION)
#LIBREVISION $(REVISION)


$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(C_COMPILER) $(C_OPTS) $*.c

lib.c: /template.c
	$(C_COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

reload:
	@copy $(LIBDIR)$(LIBFILE) LIBS:
	@version $(LIBFILE)
	@flushlibs