/* $Id: didentd-genanswer-static.c,v 1.4 2001/10/08 12:51:21 drt Exp $
 *  --drt@un.bewaff.net
 *
 * generate a static username reply
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-genanswer-static.c,v $
 * Revision 1.4  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.3  2000/04/28 12:54:56  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.2  2000/04/25 22:55:01  drt
 * code cleanups
 *
 * Revision 1.1  2000/04/20 15:09:35  drt
 * Initial Revision
 */

#include "stralloc.h"

static char rcsid[] = "$Id: didentd-genanswer-static.c,v 1.4 2001/10/08 12:51:21 drt Exp $";

/* returns a pointer to a string describing a problem or NULL if
sucessfull, adds to answer the part after the ports of an RfC 1413
reply */

char *generate_answer(stralloc *answer, unsigned long uid)
{
  char *problem = "ok";

  stralloc_cats(answer, " : USERID : UNIX : nope\r\n");
  stralloc_0(answer);

  return problem;
}

