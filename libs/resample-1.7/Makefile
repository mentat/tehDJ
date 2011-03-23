# Makefile for resample, windowfilter, and filterkit library

arch_flags=-DLINUX
# This isn't actually used in the code as yet:
ARCHS=m68k i386

DESTROOT = 
INSTALLDIR = $(DESTROOT)/usr/local/bin
MANDIR = $(DESTROOT)/usr/local/man/man1

################################################################################
# Not normally necessary to edit below this point

# Keep in synch with README file and resample.c banner:
VERSION = 1.7

TARDIR1 = /tmp
TARDIR2 = resample-$(VERSION)
TARDIR = $(TARDIR1)/$(TARDIR2)
TARFILE = resample-$(VERSION).tar.gz

PRODUCTS = resample windowfilter
BINARIES = $(PRODUCTS)
MANPAGES = resample.1 windowfilter.1

CFILES1 = resamplesubs.c sndlibextra.c audio.c io.c headers.c sound.c
CFILES2 = resample.c warp.c windowfilter.c filterkit.c
OFILES = $(CFILES1:.c=.o)
HFILES = resample.h filterkit.h stdefs.h smallfilter.h largefilter.h \
         sndlibextra.h sndlib.h sndlib-strings.h
SRCS = $(CFILES1) $(CFILES2) $(HFILES) $(MANPAGES) $(FILTERS)

BASE_CFLAGS = -I. -Wall $(arch_flags)
CFLAGS = $(BASE_CFLAGS) -O3
DEBUG_CFLAGS = $(BASE_CFLAGS) -g -DDEBUG

all: $(PRODUCTS)

debug:
	$(MAKE) all "CFLAGS = $(DEBUG_CFLAGS)"

resample: resample.c $(OFILES) libfilterkit.a
	$(CC) $(CFLAGS) resample.c -o resample $(OFILES) libfilterkit.a -lm

windowfilter: windowfilter.c stdefs.h resample.h filterkit.h libfilterkit.a
	$(CC) $(CFLAGS) windowfilter.c libfilterkit.a -lm -o windowfilter

libfilterkit.a: filterkit.c stdefs.h resample.h filterkit.h
	$(CC) $(CFLAGS) -c filterkit.c
	ar rc libfilterkit.a filterkit.o
	ranlib libfilterkit.a

clean:
	/bin/rm -f *.o libfilterkit.a $(PRODUCTS) ir.snd irt.snd ir.dat *.mb
	/bin/rm -f original.snd resampled.snd sweep.snd *~ warp

tags:
	etags -t $(CFILES1) $(CFILES2) $(HFILES)

installsrc:
	mkdir -m 755 $(SRCROOT)
	cp -p $(SRCS) README INSTALL Makefile README.deemph $(SRCROOT)

# creates products and installs them. Installs man pages also.
install: $(INSTALLDIR) $(MANDIR) $(BINARIES)
	install -s -m 755 $(BINARIES) $(INSTALLDIR)
	install -m 644 $(MANPAGES) $(MANDIR)

# Tests INSTALLED version only
test:
	(cd testdir; make)

$(INSTALLDIR): $(DESTROOT)
	mkdir -p $@

$(MANDIR): $(DESTROOT)
	mkdir -p $@

tarfile:
	-/bin/rm -rf $(TARDIR)
	make installsrc SRCROOT=$(TARDIR)
	(cd $(TARDIR1); tar cvf - $(TARDIR2) | gzip > $(TARFILE))
	mv $(TARDIR1)/$(TARFILE) .
	-/bin/rm -rf $(TARDIR)

.SUFFIXES: .c .o

.c.o: $*.h
	cc -c $*.c $(CFLAGS)
