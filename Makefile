# $Id: Makefile,v 1.13 2001/10/11 15:26:12 drt Exp $

PROGS = djblib.a didentd didentd-decrypt didentd-name didentd-static didentd-conf didentd-name-conf

CFLAGS = -g -Wall -Idjblib

all: $(PROGS) 

didentd: didentd.o get_info4_linux.o get_info6_linux.o didentd-genanswer-crypt.o \
init-linux-chroot.o rijndael.o base64-encode.o pad.o txtparse.o scan_xlong.o scan_ushort.o\
djblib.a
	$(CC) $(CFLAGS) -o $@ $^ 

didentd-name: didentd.o get_info4_linux.o get_info6_linux.o didentd-genanswer-name.o \
init-linux-chdir.o base64-encode.o scan_xlong.o scan_ushort.o\
djblib.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-static: didentd.o get_info4_linux.o get_info6_linux.o didentd-genanswer-static.o \
init-linux-chroot.o base64-encode.o scan_xlong.o scan_ushort.o\
djblib.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-decrypt: didentd-decrypt.o \
rijndael.o base64-decode.o pad.o txtparse.o \
buffer_0.o djblib.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-conf: didentd-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ $^ 

didentd-name-conf: didentd-name-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ $^ 

djblib.a: djblib/*.c djblib/*.h
	(cd djblib; make; ar cr ../djblib.a *.o)
	ar d djblib.a auto-str.o

install: $(PROGS)
	install -m 755 -s didentd didentd-name didentd-static /usr/local/bin
	install -m 755 -s didentd-conf didentd-name-conf /usr/local/bin
	install -m 755 -s didentd-decrypt /usr/local/bin
	install -m 644 didentd.8 didentd-name.8 didentd-decrypt.8 /usr/local/man/man8

clean:
	rm -f $(PROGS) *.o 
	(cd djblib; rm *.o)

distclean:
	-rm -f core $(PROGS) *~ *.o *.a 
	(cd djblib; cat TARGETS |xargs rm -f)

