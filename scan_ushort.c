/* $id$
 *
 * 000410 drt@ailis.de with bold inspiration from DJBs stuff
 *
 * $Log: scan_ushort.c,v $
 * Revision 1.2  2000/04/28 12:54:56  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.1.1.1  2000/04/12 16:07:11  drt
 * initial revision
 *
 */

#include "scan.h"

unsigned int scan_ushort(char *s, unsigned short *u)
{
  unsigned long l;
  unsigned int r;

  r = scan_ulong(s, &l);
  *u = l;

  return r;
}
