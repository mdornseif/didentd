/* $Id: init-linux-chdir.c,v 1.1 2000/05/10 19:34:44 drt Exp $
 *  --drt@ailis.de
 *
 * this is code specific to linux and includes just
 * chdir()ing this decerases the security somehow
 * it is used by didentd-name 
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: init-linux-chdir.c,v $
 * Revision 1.1  2000/05/10 19:34:44  drt
 * Further seperated system specific functions
 * and got didentd-name working again.
 *
 */

static char rcsid[] = "$Id: init-linux-chdir.c,v 1.1 2000/05/10 19:34:44 drt Exp $";

void droppriv(char *dir, int dochroot);

void didentd_init()
{
  /* chdir() to /proc/net/ and switch to $UID:$GID */
  droppriv("/proc/net/", 0);
}
