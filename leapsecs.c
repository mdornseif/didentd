/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: leapsecs.c,v 1.1 2000/04/12 16:07:18 drt Exp $
 * 
 * $Log
 */

#include <stdio.h>
#include "djb/tai.h"
#include "leapsecs.h"
#include "caldate.h"

/* XXX: breaks tai encapsulation */

/* XXX: output here has to be binary; DOS redirection uses ASCII */

char line[100];

main()
{
  struct caldate cd;
  struct tai t;
  char x[TAI_PACK];
  long leaps = 0;

  while (fgets(line,sizeof line,stdin))
    if (line[0] == '+')
      if (caldate_scan(line + 1,&cd)) {
	t.x = (caldate_mjd(&cd) + 1) * 86400ULL + 4611686014920671114ULL + leaps++;
        tai_pack(x,&t);
	fwrite(x,TAI_PACK,1,stdout);
      }
      
  exit(0);
}