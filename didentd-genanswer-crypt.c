/* $Id: didentd-genanswer-crypt.c,v 1.5 2000/04/30 02:01:58 drt Exp $
 *  --drt@ailis.de
 *
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
 * Revision 1.5  2000/04/30 02:01:58  drt
 * key is now taken from the enviroment
 *
 * Revision 1.4  2000/04/28 12:54:55  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.3  2000/04/25 22:55:01  drt
 * code cleanups
 *
 * Revision 1.2  2000/04/19 13:40:27  drt
 * base64 a la MIME encoding - see RfC1341
 *
 * Revision 1.1.1.1  2000/04/12 16:07:19  drt
 * initial revision
 *
 */

#include "env.h"
#include "stralloc.h"
#include "strerr.h"
#include "tai.h"
#include "uint16.h"
#include "uint32.h"

#include "base64.h"
#include "pad.h"
#include "rijndael.h"
#include "txtparse.h"

static char rcsid[] = "$Id: didentd-genanswer-crypt.c,v 1.5 2000/04/30 02:01:58 drt Exp $";

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
  char *x;
  char *problem = "ok";
  char buf[TAI_PACK];
  stralloc out = {0};
  stralloc tmp = {0};
  stralloc key = {0};
  struct tai now;

  /* get key from enviroment */
  x = env_get("KEY");
  if (!x)
    {
      strerr_die2x(111, "didentd fatal: ", "$KEY not set");
    }

  stralloc_copys(&key, x);
  txtparse(&key);
  pad(&key, 16);

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
 
  /* initialize rijndael */
  rijndaelKeySched(6, 4, key.s);
  
  /* encrypt with rijndael */
  rijndaelEncrypt(tmp.s);
  
  stralloc_readyplus(&out, 32);
  base64encode(out.s, tmp.s, 24);
  
  stralloc_catb(answer, out.s, 32); 
  stralloc_cats(answer, "\r\n");
  stralloc_0(answer);
  
  return problem;
}


