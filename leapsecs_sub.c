/* from DJBs libtai
 * 
 * drt@ailis.de
 * 
 * $Id: leapsecs_sub.c,v 1.1 2000/04/12 16:07:19 drt Exp $
 * 
 * $Log: leapsecs_sub.c,v $
 * Revision 1.1  2000/04/12 16:07:19  drt
 * Initial revision
 *
 */

#include "leapsecs.h"
#include "djb/tai.h"

/* XXX: breaks tai encapsulation */

extern struct tai *leapsecs;
extern int leapsecs_num;

int leapsecs_sub(t)
struct tai *t;
{
  int i;
  uint64 u;
  int s;

  if (leapsecs_init() == -1) return 0;

  u = t->x;
  s = 0;

  for (i = 0;i < leapsecs_num;++i) {
    if (u < leapsecs[i].x) break;
    ++s;
    if (u == leapsecs[i].x) { t->x = u - s; return 1; }
  }

  t->x = u - s;
  return 0;
}
