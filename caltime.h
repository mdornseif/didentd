/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: caltime.h,v 1.1 2000/04/12 16:07:18 drt Exp $
 * 
 * $Log
 */

#ifndef CALTIME_H
#define CALTIME_H

#include "caldate.h"

struct caltime {
  struct caldate date;
  int hour;
  int minute;
  int second;
  long offset;
} ;

extern void caltime_tai();
extern void caltime_utc();

extern unsigned int caltime_fmt();
extern unsigned int caltime_scan();

#endif
