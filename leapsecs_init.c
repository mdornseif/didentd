/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: leapsecs_init.c,v 1.1 2000/04/12 16:07:19 drt Exp $
 * 
 * $Log
 */

#include "leapsecs.h"

static int flaginit = 0;

int leapsecs_init()
{
  if (flaginit) return 0;
  if (leapsecs_read() == -1) return -1;
  flaginit = 1;
  return 0;
}
