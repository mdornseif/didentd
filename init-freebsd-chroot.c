/* $Id: init-freebsd-chroot.c,v 1.1 2001/10/12 12:27:48 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * this is initialisation code specific to freebsd
 * it is used by didentd and didentd-static
 *
 */

static char rcsid[] = "$Id: init-freebsd-chroot.c,v 1.1 2001/10/12 12:27:48 drt Exp $";

#include "droproot.h"

void droppriv(char *dir, int dochroot);

void didentd_init()
{ 
  /* chroot() to $ROOT and switch to $UID:$GID */
  droproot("didentd: fatal: ");
}
