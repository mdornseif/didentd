/* dident-name-conf.c by drt@ailis.de
 * - create directory structure for using didentd with svscan
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-name-conf.c,v $
 * Revision 1.1  2000/04/12 16:07:20  drt
 * Initial revision
 *
 */

#include <pwd.h>
#include "djb/strerr.h"
#include "djb/exit.h"
#include "djb/auto_home.h"
#include "djb/generic-conf.h"

static char *rcsid="$Id: didentd-name-conf.c,v 1.1 2000/04/12 16:07:20 drt Exp $";

#define FATAL "didentd-name-conf: fatal: "

void usage(void)
{
  strerr_die1x(100,"didentd-name-conf: usage: didentd-name-conf acct logacct /didentd-name myip");
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
  outs("exec envuidgid "); outs(user);
  outs(" \\\nsoftlimit -d250000");
  outs(" \\\ntcpserver -RPHv $IP ident");
  outs(" "); outs(auto_home); outs("/bin/didentd-name");
  outs("\n");
  finish();
  perm(0755);

  return 0;
}





