/* $Id: didentd-conf.c,v 1.5 2001/10/08 12:51:21 drt Exp $
 *  --drt@un.bewaff.net
 *
 * create directory structure for using didentd with svscan
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-conf.c,v $
 * Revision 1.5  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.4  2000/04/30 02:01:58  drt
 * key is now taken from the enviroment
 *
 * Revision 1.3  2000/04/28 12:54:55  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.2  2000/04/25 22:32:22  drt
 * Code Cleanups
 *
 * Revision 1.1.1.1  2000/04/12 16:07:20  drt
 * initial revision
 *
 */

#include <pwd.h>

#include "auto_home.h"
#include "exit.h"
#include "generic-conf.h"
#include "strerr.h"

static char rcsid[] = "$Id: didentd-conf.c,v 1.5 2001/10/08 12:51:21 drt Exp $";

#define FATAL "didentd-conf: fatal: "

void usage(void)
{
  strerr_die1x(100,"didentd-conf: usage: didentd-conf acct logacct /didentd myip");
}

char *dir;
char *user;
char *loguser;
struct passwd *pw;
char *myip;

int main(int argc, char **argv)
{
  user = argv[1];
  if (!user) usage();
  loguser = argv[2];
  if (!loguser) usage();
  dir = argv[3];
  if (!dir) usage();
  if (dir[0] != '/') usage();
  myip = argv[4];
  if (!myip) usage();

  pw = getpwnam(loguser);
  if (!pw)
    strerr_die3x(111,FATAL,"unknown account ",loguser);

  init(dir,FATAL);
  makelog(loguser,pw->pw_uid,pw->pw_gid);

  start("run");
  outs("#!/bin/sh\nexec 2>&1\n");
  outs("IP="); outs(myip); outs("; export IP\n");
  outs("KEY=\"changemenow\\001\\002\\003\"; export KEY\n");
  outs("exec envuidgid "); outs(user);
  outs(" \\\nsoftlimit -d250000");
  outs(" \\\ntcpserver -RPHv $IP ident");
  outs(" "); outs(auto_home); outs("/bin/didentd");
  outs("\n");
  finish();
  perm(0700);

  return 0;
}






