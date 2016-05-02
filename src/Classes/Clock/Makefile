
LIBNAME=	clock.dg
VERSION=	1
REVISION=	0

INC=		INCLUDEDIRECTORY=///include INCLUDEDIRECTORY=/

LIBFILE=	$(LIBNAME).class
LIBDIR=		///bin/

FD_CONV=	SC:c/fd2pragma
FD_FILE=	/template.fd
PRAGMA_FILE=	pragmas.h

C_COMPILER=	SC:c/sc
C_OPTS=		STREQ STRMER NOSTKCHK NOSTANDARDIO $(INC)

LINKER=		SC:c/slink

C_SOURCES=	lib.c clock.c

OBJECTS=	lib.o clock.o

LIBS=		LIB:sc.lib LIB:amiga.lib

#LIBENT=		LIB:libent.o
#LIBINIT=	LIB:libinit.o

################
#
# Build Library

$(LIBFILE): $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIB $(LIBS)
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