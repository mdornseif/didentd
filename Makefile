## $Id: Makefile,v 1.17 2001/10/14 05:58:33 drt Exp $

PROGS = didentd didentd-decrypt didentd-name didentd-static didentd-conf didentd-name-conf didentd-static-conf

CFLAGS = -g -Wall -Idjblib

all: djblib.a $(PROGS) 

os:
	sh -c "uname -s|tr ' [A-Z]' '.[a-z]' > os"

get_info4.c: os 
	sh -c "ln -sfv get_info4_`cat os`.c get_info4.c"
	touch get_info4.c

get_info6.c: os 
	sh -c "ln -sfv get_info6_`cat os`.c get_info6.c"
	touch get_info6.c

init-chroot.c: os
	sh -c "ln -sfv init-`cat os`-chroot.c init-chroot.c" 
	touch init-chroot.c

init-chdir.c: os 
	sh -c "ln -sfv init-`cat os`-chdir.c init-chdir.c" 
	touch init-chdir.c

didentd: didentd.o didentd-genanswer-crypt.o \
rijndael.o base64-encode.o pad.o txtparse.o scan_xlong.o \
scan_ushort.o djblib.a get_info4.o get_info6.o init-chroot.o
	$(CC) $(CFLAGS) -o $@ didentd.o get_info4.o get_info6.o \
	didentd-genanswer-crypt.o init-chroot.o rijndael.o base64-encode.o pad.o\
	txtparse.o scan_xlong.o scan_ushort.o djblib.a 

didentd-static: didentd.o didentd-genanswer-static.o \
base64-encode.o scan_xlong.o scan_ushort.o djblib.a get_info4.o \
get_info6.o init-chroot.o
	$(CC) $(CFLAGS) -o $@ didentd.o get_info4.o get_info6.o \
	didentd-genanswer-static.o init-chroot.o base64-encode.o scan_xlong.o scan_ushort.o djblib.a

didentd-name: didentd.o didentd-genanswer-name.o \
base64-encode.o scan_xlong.o scan_ushort.o djblib.a get_info4.o get_info6.o init-chdir.o
	$(CC) $(CFLAGS) -o $@ didentd.o get_info4.o get_info6.o \
	didentd-genanswer-name.o init-chdir.o base64-encode.o scan_xlong.o scan_ushort.o djblib.a

didentd-decrypt: didentd-decrypt.o \
rijndael.o base64-decode.o pad.o txtparse.o buffer_0.o djblib.a 
	$(CC) $(CFLAGS) -o $@ didentd-decrypt.o \
	rijndael.o base64-decode.o pad.o txtparse.o buffer_0.o djblib.a 	

didentd-conf: didentd-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ didentd-conf.c djblib.a

didentd-name-conf: didentd-name-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ didentd-name-conf.c djblib.a


didentd-static-conf: didentd-static-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ didentd-static-conf.c djblib.a

djblib.a: djblib/*.c djblib/*.h
	(cd djblib; make; ar cr ../djblib.a *.o)
	ar d djblib.a auto-str.o chkshsgr.o
	ranlib djblib.a

install: $(PROGS)
	install -m 755 -s didentd didentd-name didentd-static /usr/local/bin
	install -m 755 -s didentd-conf didentd-name-conf didentd-static-conf /usr/local/bin
	install -m 755 -s didentd-decrypt /usr/local/bin
	install -m 644 didentd.8 didentd-name.8 didentd-decrypt.8 /usr/local/man/man8

clean:
	rm -f $(PROGS) *.o 

distclean:
	-rm -f core $(PROGS) *~ *.o *.a os 
	-rm -f init-chroot.c init-chdir.c get_info4.c get_info6.c
	(cd djblib; cat TARGETS |xargs rm -f)
	rm -f djblib/auto-str djblib/auto_home.c djblib/chkshsgr djblib/choose
	rm -f djblib/compile djblib/hasshsgr.h djblib/iopause.h djblib/load
	rm -f djblib/makelib djblib/select.h djblib/systype djblib/uint32.h 
	rm -f djblib/uint64.h
