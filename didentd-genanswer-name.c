/* dident-genanswer-name.c by drt@ailis.de
 * - generate a straightforward RfC 1413 username reply
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-genanswer-name.c,v $
 * Revision 1.1  2000/04/12 16:07:19  drt
 * Initial revision
 *
 */


#include "djb/stralloc.h"
#include "djb/uint16.h"
#include "djb/uint32.h"
#include <pwd.h>

static char *rcsid = "$Id: didentd-genanswer-name.c,v 1.1 2000/04/12 16:07:19 drt Exp $";

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

