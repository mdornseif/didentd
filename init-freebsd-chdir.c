/* $Id: init-freebsd-chdir.c,v 1.2 2001/10/15 00:25:58 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0dr/didentd/
 *
 * system specific initialisation for freebsd - it DOES NOT CHROOT
 * 
 */

static char rcsid[] = "$Id: init-freebsd-chdir.c,v 1.2 2001/10/15 00:25:58 drt Exp $";

#include <unistd.h> /* for chdir(2) */
#include "env.h"
#include "scan.h"
#include "prot.h"
#include "strerr.h"

#define FATAL "didentd: fatal: " 

void didentd_init()
{
  char *x;
  unsigned long id;

  x = env_get("ROOT");
  if (!x)
     strerr_die2x(111,FATAL,"$ROOT not set");
  if (chdir(x) == -1)
    strerr_die4sys(111,FATAL,"unable to chdir to ",x,": ");

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
