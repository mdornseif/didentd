/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: caldate_scan.c,v 1.1 2000/04/12 16:07:20 drt Exp $
 * 
 * $Log
 */

#include "caldate.h"

unsigned int caldate_scan(s,cd)
char *s;
struct caldate *cd;
{
  int sign = 1;
  char *t = s;
  unsigned long z;
  unsigned long c;

  if (*t == '-') { ++t; sign = -1; }
  z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
  cd->year = z * sign;

  if (*t++ != '-') return 0;
  z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
  cd->month = z;

  if (*t++ != '-') return 0;
  z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
  cd->day = z;

  return t - s;
}
