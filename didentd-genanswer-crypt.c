/* dident.c by drt@ailis.de
 * - generate a RfC 1413 reply containing 
 *   uid of connection localip, localport, 
 *   remoteip, remoteport and a timestamp
 *   in encrypted form
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-genanswer-crypt.c,v $
 * Revision 1.1  2000/04/12 16:07:19  drt
 * Initial revision
 *
 */

#include "djb/stralloc.h"
#include "djb/tai.h"
#include "djb/uint16.h"
#include "djb/uint32.h"
#include "rijndael.h"

static char *rcsid = "$Id: didentd-genanswer-crypt.c,v 1.1 2000/04/12 16:07:19 drt Exp $";

#define NULL 0

extern uint32 lip;
extern uint32 rip;
extern uint16 lport;
extern uint16 rport;

/* returns a pointer to a string describing a problem or NULL if
sucessfull, adds to answer the part after the ports of an RfC 1413
reply */

char *generate_answer(stralloc *answer, uint32 uid)
{
  char *key = KEY;
  char *problem = "ok";
  char buf[TAI_PACK];
  char x[4];
  int i;
  stralloc out = {0};
  stralloc tmp = {0};
  struct tai now;
  
  /*  if(uid == 0xffffffff) 
    {
      problem ="connection unknown";
    }
    else */
    {
      tai_now(&now);
      
      stralloc_cats(answer, " : USERID : OTHER : ");
      uint32_pack(buf, uid);
      stralloc_catb(&tmp, buf, 4);
      uint32_pack(buf, lip);
      stralloc_catb(&tmp, buf, 4);
      uint16_pack(buf, lport);
      stralloc_catb(&tmp, buf, 2);
      uint32_pack(buf, rip);
      stralloc_catb(&tmp, buf, 4);
      uint16_pack(buf, rport);
      stralloc_catb(&tmp, buf, 2);
      tai_pack(buf, &now);
      stralloc_catb(&tmp, buf, sizeof buf);
    }		  
 
    /* initialize rijndael */
    rijndaelKeySched(6, 4, key);

    /* encrypt with rijndael */
    rijndaelEncrypt(tmp.s);

    for(i = 0; i < 24;)
      {
	x[0] = (tmp.s[i] & 0x3f) + 60;
	x[3] = (tmp.s[i++] & 0xc0) >> 2;
	x[1] = (tmp.s[i] & 0x3f) + 60;
	x[3] |= (tmp.s[i++] & 0xc0) >> 4;
	x[2] = (tmp.s[i] & 0x3f) + 60;
	x[3] |= (tmp.s[i++] & 0xc0) >> 6;
       	x[3] += 60;
	stralloc_catb(&out, x, 4);
      }

    stralloc_catb(answer, out.s, 32); 
    stralloc_cats(answer, "\r\n");
    stralloc_0(answer);
    
    return problem;
}

