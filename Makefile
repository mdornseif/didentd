# $Id: Makefile,v 1.9 2000/05/09 06:47:34 drt Exp $

DOWNLOADER=wget

PROGS =  didentd didentd-decrypt didentd-name didentd-static didentd-conf didentd-name-conf

CFLAGS = -g -Wall -Idnscache

all: dnscache.a $(PROGS) 

didentd: didentd.o get_info4_linux.o get_info6_linux.o didentd-genanswer-crypt.o \
rijndael.o base64-encode.o pad.o txtparse.o scan_xlong.o scan_ushort.o\
dnscache.a
	$(CC) $(CFLAGS) -o $@ $^ 

didentd-name: didentd.o get_info4_linux.o  get_info6_linux.o didentd-genanswer-name.o \
base64-encode.o scan_xlong.o scan_ushort.o\
dnscache.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-static: didentd.o get_info4_linux.o get_info6_linux.o didentd-genanswer-static.o \
base64-encode.o scan_xlong.o scan_ushort.o\
dnscache.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-decrypt: didentd-decrypt.o \
rijndael.o base64-decode.o pad.o txtparse.o \
buffer_0.o dnscache.a
	$(CC) $(CFLAGS) -o $@ $^

didentd-conf: didentd-conf.c dnscache.a
	$(CC) $(CFLAGS) -o $@ $^ 

didentd-name-conf: didentd-name-conf.c dnscache.a
	$(CC) $(CFLAGS) -o $@ $^ 

install: $(PROGS)
	install -m 755 -s didentd didentd-name didentd-static /usr/local/bin
	install -m 755 -s didentd-conf didentd-name-conf /usr/local/bin
	install -m 755 -s didentd-decrypt /usr/local/bin
	install -m 644 didentd.8 didentd-name.8 didentd-decrypt.8 /usr/local/man/man8

clean:
	rm -f $(PROGS) *.o 

distclean:
	-rm -f core $(PROGS) *~ *.o *.a
	-rm -Rf dnscache*

dnscache.a:
	if [ ! -f dnscache-1.00-ipv6.diff5 ]; then \
		$(DOWNLOADER) http://www.fefe.de/dns/dnscache-1.00-ipv6.diff5;\
	fi
	if [ ! -d dnscache ]; then \
		$(DOWNLOADER) http://cr.yp.to/dnscache/dnscache-1.00.tar.gz; \
		tar xzvf dnscache-1.00.tar.gz; rm dnscache-1.00.tar.gz; \
		mv dnscache-1.00 dnscache; \
		cd dnscache; patch < ../dnscache-1.00-ipv6.diff5; \
        fi;	
	cd dnscache; \
	make; \
	grep -l ^main *.c | perl -npe 's/^(.*).c/\1.o/;' | xargs rm -f; \
	ar cr ../dnscache.a *.o;
