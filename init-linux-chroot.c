/* $Id: init-linux-chroot.c,v 1.3 2001/10/15 07:44:32 drt Exp $
 *  --drt@un.bewaff.net - htto://c0re.jp/c0de/didentd/
 *
 * this is code specific to linux and includes chroot()ing;
 * it is used by didentd and didentd-static 
 *
 */

#include <unistd.h> /* for chroot(2) and chdir(2) */
#include "env.h"
#include "scan.h"
#include "prot.h"
#include "strerr.h"

static char rcsid[] = "$Id: init-linux-chroot.c,v 1.3 2001/10/15 07:44:32 drt Exp $";

#define FATAL "didentd: fatal: "

void didentd_init()
{
  /* chroot() to /proc/net/ and switch to $UID:$GID */
  char *x;
  unsigned long id;

  if (chdir("/proc/net/") == -1)
    strerr_die2sys(111,FATAL,"unable to chdir to '/proc/net/': ");
  if (chroot(".") == -1)
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
