.PHONY: all clean install dist install debug valgrind

CC = gcc
#CC = i686-mingw32-gcc

VERSION := 0.07

BINDIR := bin/
SRCDIR := src/
INCDIR := includes/

CFLAGS += -O2
#CFLAGS += -O3 -s
CFLAGS += -Wall -I$(INCDIR) -D_POSIX_SOURCE -D_XOPEN_SOURCE=500 -DAVRDISAS_VERSION=\"$(VERSION)\"
#CFLAGS += -ansi -pedantic
#CFLAGS += -Wextra
#CFLAGS += -g
#INSTALLDIR = /usr/local/bin/
INSTALLDIR = /usr/bin/
ETCDIR = /etc/

#TESTOPTIONS = -a1 -p0 -o1 -l1 -ttools/AsmExample.tag -mm16 tools/AsmExample.bin
TESTOPTIONS = -a1 -q1 -l1 -mtn2313 tools/AVRLirc.bin -ttools/AVRLirc.tag

OBJS = $(BINDIR)Callbacks_Assembly.o $(BINDIR)Callbacks_PseudoCode.o $(BINDIR)Options.o $(BINDIR)JumpCall.o $(BINDIR)IORegisters.o $(BINDIR)MNemonics.o $(BINDIR)Tagfile.o $(BINDIR)Tools.o $(BINDIR)Opcodes.o

all: $(BINDIR)avrdisas

clean:
	rm -f $(BINDIR)avrdisas
	rm -f $(OBJS)

install: $(BINDIR)avrdisas
	strip $(BINDIR)avrdisas
	cp $(BINDIR)avrdisas $(INSTALLDIR)avrdisas
	chown root:root $(INSTALLDIR)avrdisas
	chmod 755 $(INSTALLDIR)avrdisas
	cp avrdisas.conf $(ETCDIR)avrdisas.conf
	chown root:root $(ETCDIR)avrdisas.conf
	chmod 644 $(ETCDIR)avrdisas.conf

dist:
	svn export . avrdisas-${VERSION}
	tar cfvj avrdisas-${VERSION}.tar.bz2 avrdisas-${VERSION}
	rm -fr avrdisas-${VERSION}

test: $(BINDIR)avrdisas
	$(BINDIR)avrdisas $(TESTOPTIONS)

debug: $(BINDIR)avrdisas
	gdb --args $(BINDIR)avrdisas $(TESTOPTIONS)

valgrind: $(BINDIR)avrdisas
	valgrind $(BINDIR)avrdisas $(TESTOPTIONS)

$(BINDIR)avrdisas: $(SRCDIR)avrdisas.c $(OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)avrdisas $(SRCDIR)avrdisas.c $(OBJS)

$(BINDIR)JumpCall.o: $(INCDIR)JumpCall.h $(SRCDIR)JumpCall.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)JumpCall.o $(SRCDIR)JumpCall.c

$(BINDIR)Opcodes.o: $(INCDIR)Opcodes.h $(SRCDIR)Opcodes.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Opcodes.o $(SRCDIR)Opcodes.c

$(BINDIR)Callbacks_Assembly.o: $(INCDIR)Callbacks_Assembly.h $(SRCDIR)Callbacks_Assembly.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Callbacks_Assembly.o $(SRCDIR)Callbacks_Assembly.c

$(BINDIR)Callbacks_PseudoCode.o: $(INCDIR)Callbacks_PseudoCode.h $(SRCDIR)Callbacks_PseudoCode.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Callbacks_PseudoCode.o $(SRCDIR)Callbacks_PseudoCode.c

$(BINDIR)Options.o: $(INCDIR)Options.h $(SRCDIR)Options.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Options.o $(SRCDIR)Options.c

$(BINDIR)IORegisters.o: $(INCDIR)IORegisters.h $(SRCDIR)IORegisters.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)IORegisters.o $(SRCDIR)IORegisters.c

$(BINDIR)MNemonics.o: $(INCDIR)MNemonics.h $(SRCDIR)MNemonics.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)MNemonics.o $(SRCDIR)MNemonics.c

$(BINDIR)Tagfile.o: $(INCDIR)Tagfile.h $(SRCDIR)Tagfile.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Tagfile.o $(SRCDIR)Tagfile.c

$(BINDIR)Tools.o: $(INCDIR)Tools.h $(SRCDIR)Tools.c
	$(CC) $(CFLAGS) -c -o $(BINDIR)Tools.o $(SRCDIR)Tools.c

