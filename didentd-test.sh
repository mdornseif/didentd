#!/bin/sh
# $Id: didentd-test.sh,v 1.2 2001/10/12 12:31:09 drt Exp $
# basic tests
export KEY=test
export ROOT=.
export GID=0
export PROTO=TCP
export TCPLOCALIP=127.0.0.1
export TCPLOCALPORT=4096
export TCPREMOTEIP=127.0.0.1
export TCPREMOTEPORT=8192
# do a test connection
tcpserver 127.0.0.1 4096 sh -c 'sleep 10'  &
tcpclient -RHl0 -p 8192 -- 127.0.0.1 4096 sh -c 'exec cat <&6' &
rm -f test.1 test.2 test.decrypt
echo "8192 , 4096" | env UID=0 ./didentd 2> test.2 > test.crypt
echo "8192 , 4096" | env UID=0 ./didentd-static 2>> test.2 >> test.1
echo "8192 , 4096" | env UID=0 ./didentd-name 2>> test.2 >> test.1
cat test.crypt | cut -c 32-63 | ./didentd-decrypt | cut -c 26-99 > test.decrypt
# create files for compariastion
USERID=`id -u`
USERNAME=`id -n -u`
rm -f test.should.1 test.should.2 test.should.decrypt
echo -e "8192 , 4096 : USERID : UNIX : nope\015" > test.should.1
echo -e "8192 , 4096 : USERID : UNIX : $USERNAME\015" >> test.should.1
echo "127.0.0.1:8192 -> 127.0.0.1:4096 [ok] $USERID 8192 , 4096" > test.should.2
echo "127.0.0.1:8192 -> 127.0.0.1:4096 [ok] $USERID 8192 , 4096" >> test.should.2
echo "127.0.0.1:8192 -> 127.0.0.1:4096 [ok] $USERID 8192 , 4096" >> test.should.2
echo "$USERNAME($USERID) 127.0.0.1:8192 127.0.0.1:4096" > test.should.decrypt
if [ ! -z "`diff test.1 test.should.1`" ]; then
   echo "error with didentd answer"
   exit 1
fi
if [ ! -z "`diff test.2 test.should.2`" ]; then
   echo "error with didentd logging"
   exit 1
fi
if [ ! -z "`diff test.decrypt test.should.decrypt`" ]; then
   echo "error with didentd decrypting"
   exit 1
fi
rm -f test.should.1 test.should.2 test.should.decrypt test.1 test.2 test.decrypt
echo OK

