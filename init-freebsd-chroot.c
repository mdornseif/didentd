/* $Id: init-freebsd-chroot.c,v 1.2 2001/10/14 05:57:15 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * this is initialisation code specific to freebsd
 * it is used by didentd and didentd-static
 *
 */

static char rcsid[] = "$Id: init-freebsd-chroot.c,v 1.2 2001/10/14 05:57:15 drt Exp $";

#include "env.h"
#include "prot.h"
#include "strerr.h"
#include <unistd.h> /*chdir(), chroot() */

#define FATAL "didentd: fatal: "

/* chroot() to $ROOT 
 * this is based on DJBs droproot
 *
 * Note that chrooting without dropping the userid to something
 * != 0 does not help much. root can break a chroot jail.
 */

void didentd_init()
{ 
  char *dir;

  dir = env_get("ROOT");
  if (!dir)
    strerr_die2x(111, FATAL, "$ROOT not set");
			     
  if (chdir(dir) == -1)
    strerr_die4sys(111, FATAL, "unable to chdir to ", dir, ": ");
  
  if (chroot(".") == -1)
    strerr_die4sys(111, FATAL, "unable to chroot to ", dir, ": ");
}
