/* $Id: init-linux-chroot.c,v 1.2 2001/10/08 12:51:21 drt Exp $
 *  --drt@un.bewaff.net
 *
 * this is code specific to linux and includes chroot()ing;
 * it is used by didentd and didentd-static
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: init-linux-chroot.c,v $
 * Revision 1.2  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.1  2000/05/10 19:34:44  drt
 * Further seperated system specific functions
 * and got didentd-name working again.
 *
 */

static char rcsid[] = "$Id: init-linux-chroot.c,v 1.2 2001/10/08 12:51:21 drt Exp $";

void droppriv(char *dir, int dochroot);

void didentd_init()
{
  /* chroot() to /proc/net/ and switch to $UID:$GID */
  droppriv("/proc/net/", 1);
}
