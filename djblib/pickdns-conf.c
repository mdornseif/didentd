#include <pwd.h>
#include "strerr.h"
#include "exit.h"
#include "auto_home.h"
#include "generic-conf.h"

#define FATAL "pickdns-conf: fatal: "

void usage(void)
{
  strerr_die1x(100,"pickdns-conf: usage: pickdns-conf acct logacct /pickdns myip");
}

char *dir;
char *user;
char *loguser;
struct passwd *pw;
char *myip;

main(int argc,char **argv)
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
  outs("ROOT="); outs(dir); outs("/root; export ROOT\n");
  outs("IP="); outs(myip); outs("; export IP\n");
  outs("exec envuidgid "); outs(user);
  outs(" \\\nsoftlimit -d250000");
  outs(" \\\n"); outs(auto_home); outs("/bin/pickdns");
  outs("\n");
  finish();
  perm(0755);

  makedir("root");
  perm(02755);
  start("root/data");
  finish();
  perm(0644);
  start("root/Makefile");
  outs("data.cdb: data\n");
  outs("\t"); outs(auto_home); outs("/bin/pickdns-data\n");
  finish();
  perm(0644);

  _exit(0);
}
