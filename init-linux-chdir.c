/* $Id: init-linux-chdir.c,v 1.3 2001/10/15 07:44:32 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * this is code specific to linux and includes just
 * chdir()ing this decerases the security somehow
 * it is used by didentd-name 
 *
 */

#include <unistd.h> /* for chdir(2) */
#include "env.h"
#include "scan.h"
#include "prot.h"
#include "strerr.h"

#define FATAL "didentd: fatal:"

static char rcsid[] = "$Id: init-linux-chdir.c,v 1.3 2001/10/15 07:44:32 drt Exp $";

void droppriv(char *dir, int dochroot);

void didentd_init()
{
  char *x;
  unsigned long id;

  /* chdir() to /proc/net/ and switch to $UID:$GID */

  if (chdir("/proc/net/") == -1)
    strerr_die2sys(111,FATAL,"unable to chdir to '/proc/net/': ");

  x = env_get("GID");
  if (!x)
    strerr_die2x(111,FATAL,"$GID not set");
  scan_ulong(x,&id);
  if (prot_gid((int) id) == -1)
    strerr_die2sys(111,FATAL,"unable to setgid: ");

  x = env_get("UID");
  if (!x)
    strerr_die2x(111,FATAL,"$UID not set");
  scan_ulong(x,&id);
  if (prot_uid((int) id) == -1)
    strerr_die2sys(111,FATAL,"unable to setuid: ");
}
