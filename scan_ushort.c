#include "djb/scan.h"

/* $id$
 *
 * (K) 000410 drt@ailis.de with bold inspiration from DJBs stuff
 *
 * $Log: scan_ushort.c,v $
 * Revision 1.1  2000/04/12 16:07:11  drt
 * Initial revision
 *
 */

unsigned int scan_ushort(char *s, unsigned short *u)
{
  unsigned long l;
  unsigned int r;

  r = scan_ulong(s, &l);
  *u = l;

  return r;
}
