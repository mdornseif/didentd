/* $Id: didentd-static-conf.c,v 1.2 2001/10/15 00:19:46 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * create directory structure for using didentd-static with svscan
 *
 */

#include <pwd.h>
#include <sys/stat.h> /* mkdir(2) */

#include "auto_home.h"
#include "exit.h"
#include "generic-conf.h"
#include "strerr.h"

static char rcsid[]="$Id: didentd-static-conf.c,v 1.2 2001/10/15 00:19:46 drt Exp $";

#define FATAL "didentd-static-conf: fatal: "

void usage(void)
{
  strerr_die1x(100,"didentd-static-conf: usage: didentd-static-conf acct logacct /didentd-static myip");
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

  if (mkdir("root",0700) == -1)
    strerr_die2sys(111,FATAL,"unable to create directory 'root': ");

  start("run");
  outs("#!/bin/sh\nexec 2>&1\n");
  outs("IP="); outs(myip); outs("; export IP\n");
  outs("ROOT="); outs(dir); outs("/root; export ROOT\n");
  outs("exec envuidgid "); outs(user);
  outs(" \\\nsoftlimit -d250000");
  outs(" \\\ntcpserver -RPHv $IP ident");
  outs(" "); outs(auto_home); outs("/bin/didentd-static");
  outs("\n");
  finish();
  perm(0755);

  return 0;
}






