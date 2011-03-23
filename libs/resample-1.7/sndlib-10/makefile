srcdir = .
prefix = /usr/local
libdir = $(prefix)/lib
includedir = $(prefix)/include

CC = gcc
INSTALL = /usr/bin/install -c
DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2
LDFLAGS = 
LIBS = -ldl -lc -lm 
INSTALL = install
LDSO_FLAGS = -shared
SO_INSTALL = install
SO_LD = ld
A_LD = ld
A_LD_FLAGS = 

GUILE_LDFLAGS = -L/usr/local/lib -lguile -ldl -lm
GUILE_CFLAGS = -I/usr/local/include
AUDIO_LIB = 
LIB_NAME = sndlib.so
GDBM_LIB = 

CLM_O_FILES = clm.o clm2scm.o

.c.o:
	$(CC) -c $(DEFS) $(CFLAGS) $(GUILE_CFLAGS) -DHAVE_SNDLIB $<

SNDLIB_HEADERS = sndlib.h sndlib-strings.h vct.h clm.h
SNDLIB_O_FILES = headers.o audio.o io.o sound.o vct.o sndlib2scm.o

sndlib: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) $(CLM_O_FILES)
	$(SO_LD) $(LDFLAGS) $(SNDLIB_O_FILES) $(CLM_O_FILES) -o sndlib.so $(LDSO_FLAGS) $(GUILE_LDFLAGS) $(AUDIO_LIB) $(GDBM_LIB) $(LIBS)
	$(A_LD) -r $(LDFLAGS) $(SNDLIB_O_FILES) $(CLM_O_FILES) -o sndlib.a $(AUDIO_LIB) $(GDBM_LIB) $(A_LD_FLAGS)

$(SNDLIB_O_FILES): $(SNDLIB_HEADERS)
$(CLM_O_FILES): $(SNDLIB_HEADERS)

clean:
	rm -f $(SNDLIB_O_FILES)

sndplay: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) sndplay.o
	$(CC) sndplay.c -o sndplay $(libdir)/$(LIB_NAME) $(GUILE_LDFLAGS)

sndrecord: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) sndrecord.o
	$(CC) sndrecord.c -o sndrecord $(libdir)/$(LIB_NAME) $(GUILE_LDFLAGS)

sndinfo: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) sndinfo.o
	$(CC) sndinfo.c -o sndinfo $(libdir)/$(LIB_NAME) $(GUILE_LDFLAGS)

sndsine: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) sndsine.o
	$(CC) sndsine.c -o sndsine $(libdir)/$(LIB_NAME) $(GUILE_LDFLAGS)

audinfo: $(SNDLIB_HEADERS) $(SNDLIB_O_FILES) audinfo.o
	$(CC) audinfo.c -o audinfo $(libdir)/$(LIB_NAME) $(GUILE_LDFLAGS)

install: sndlib
	$(SO_INSTALL) sndlib.so $(libdir)/sndlib.so
	$(INSTALL) sndlib.a $(libdir)/sndlib.a
	$(INSTALL) sndlib.h $(includedir)/sndlib.h

uninstall:
	rm -f $(libdir)/sndlib.a
	rm -f $(libdir)/sndlib.so


Makefile: Makefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

configure: configure.in
	cd $(srcdir); autoconf

