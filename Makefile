# $Id: Makefile,v 1.15 2001/10/12 12:27:48 drt Exp $

PROGS = didentd didentd-decrypt didentd-name didentd-static didentd-conf didentd-name-conf

CFLAGS = -g -Wall -Idjblib

all: djblib.a $(PROGS) 

os:
	sh -c "uname -s|tr ' [A-Z]' '.[a-z]' > os"

get_info4.lib: os get_info4_*.o get_info6_*.o
	sh -c "echo get_info4_`cat os`.o > get_info4.lib" 

get_info6.lib: os get_info4_*.o get_info6_*.o
	sh -c "echo get_info6_`cat os`.o > get_info6.lib" 

init-chroot.lib: os init-freebsd-chroot.o init-linux-chroot.o
	sh -c "echo init-`cat os`-chroot.o > init-chroot.lib" 

init-chdir.lib: os init-freebsd-chdir.o init-linux-chdir.o
	sh -c "echo init-`cat os`-chdir.o > init-chdir.lib" 

didentd: didentd.o didentd-genanswer-crypt.o \
init-droppriv.o rijndael.o base64-encode.o pad.o txtparse.o scan_xlong.o \
scan_ushort.o djblib.a get_info4.lib get_info6.lib init-chroot.lib
	$(CC) $(CFLAGS) -o $@ didentd.o `cat get_info4.lib` `cat get_info6.lib`\
	didentd-genanswer-crypt.o `cat init-chroot.lib` rijndael.o base64-encode.o pad.o\
	txtparse.o scan_xlong.o scan_ushort.o djblib.a 

didentd-static: didentd.o didentd-genanswer-static.o \
base64-encode.o scan_xlong.o scan_ushort.o djblib.a  get_info4.lib \
get_info6.lib init-chroot.lib
	$(CC) $(CFLAGS) -o $@ didentd.o `cat get_info4.lib` `cat get_info6.lib` \
	didentd-genanswer-static.o `cat init-chroot.lib` base64-encode.o scan_xlong.o scan_ushort.o djblib.a

didentd-name: didentd.o didentd-genanswer-name.o \
base64-encode.o scan_xlong.o scan_ushort.o djblib.a get_info4.lib get_info6.lib init-chdir.lib
	$(CC) $(CFLAGS) -o $@ didentd.o `cat get_info4.lib` `cat get_info6.lib` \
	didentd-genanswer-name.o `cat init-chdir.lib` base64-encode.o scan_xlong.o scan_ushort.o djblib.a

didentd-decrypt: didentd-decrypt.o \
rijndael.o base64-decode.o pad.o txtparse.o buffer_0.o djblib.a 
	$(CC) $(CFLAGS) -o $@ didentd-decrypt.o \
	rijndael.o base64-decode.o pad.o txtparse.o buffer_0.o djblib.a 	

didentd-conf: didentd-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ didentd-conf.c djblib.a

didentd-name-conf: didentd-name-conf.c djblib.a
	$(CC) $(CFLAGS) -o $@ didentd-name-conf.c djblib.a

djblib.a: djblib/*.c djblib/*.h
	(cd djblib; make; ar cr ../djblib.a *.o)
	ar d djblib.a auto-str.o chkshsgr.o
	ranlib djblib.a

install: $(PROGS)
	install -m 755 -s didentd didentd-name didentd-static /usr/local/bin
	install -m 755 -s didentd-conf didentd-name-conf /usr/local/bin
	install -m 755 -s didentd-decrypt /usr/local/bin
	install -m 644 didentd.8 didentd-name.8 didentd-decrypt.8 /usr/local/man/man8

clean:
	rm -f $(PROGS) *.o 
	(cd djblib; rm *.o)

distclean:
	-rm -f core $(PROGS) *~ *.o *.a os *.lib
	(cd djblib; cat TARGETS |xargs rm -f)
	rm -f djblib/auto-str djblib/auto_home.c djblib/chkshsgr djblib/choose
	rm -f djblib/compile djblib/hasshsgr.h djblib/iopause.h djblib/load
	rm -f djblib/makelib djblib/select.h djblib/systype djblib/uint32.h 
	rm -f djblib/uint64.h
