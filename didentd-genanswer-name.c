/* $Id: didentd-genanswer-name.c,v 1.4 2001/10/08 12:51:21 drt Exp $
 *  --drt@un.bewaff.net
 * 
 * generate a straightforward RfC 1413 username reply
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-genanswer-name.c,v $
 * Revision 1.4  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.3  2000/04/28 12:54:55  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.2  2000/04/25 22:31:48  drt
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/04/12 16:07:19  drt
 * initial revision
 */

#include <pwd.h>

#include "stralloc.h"
#include "uint16.h"
#include "uint32.h"

static char rcsid[] = "$Id: didentd-genanswer-name.c,v 1.4 2001/10/08 12:51:21 drt Exp $";

/* returns a pointer to a string describing a problem or NULL if
sucessfull, adds to answer the part after the ports of an RfC 1413
reply */

char *generate_answer(stralloc *answer, unsigned long uid)
{
  char *problem = "ok";
  struct passwd *pw;

  if(uid == 0xffffffff) 
    {
      problem ="connection unknown";
      stralloc_cats(answer, " : ERROR : UNKNOWN-ERROR\r\n");
    }
  else
    {
      pw = getpwuid(uid);
      if (!pw)
	{
	  problem ="getpwuid failed";
	  stralloc_cats(answer, " : ERROR : UNKNOWN-ERROR\r\n");
	}
      else
	{
	  stralloc_cats(answer, " : USERID : UNIX : ");
	  stralloc_cats(answer, pw->pw_name);
	  stralloc_cats(answer, "\r\n");
	}		  
    }
  stralloc_0(answer);

  return problem;
}

