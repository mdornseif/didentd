/* $Id: didentd-genanswer-crypt.c,v 1.12 2001/10/14 05:59:58 drt Exp $
 *  --drt@un.bewaff.net
 *
 * - generate a RfC 1413 reply containing 
 *   uid of connection localip, localport, 
 *   remoteip, remoteport and a timestamp
 *   in encrypted form
 * 
 * You might find more info at http://c0re.jp/c0de/didentd/
 *
 * I do not belive there is something like copyright. 
 *
 */

#include "time.h"

#include "env.h"
#include "stralloc.h"
#include "strerr.h"
#include "uint16.h"
#include "uint32.h"

#include "base64.h"
#include "pad.h"
#include "rijndael.h"
#include "txtparse.h"

static char rcsid[] = "$Id: didentd-genanswer-crypt.c,v 1.12 2001/10/14 05:59:58 drt Exp $";

/* returns a pointer to a string describing a problem or NULL if
sucessfull, adds to answer the part after the ports of an RfC 1413
reply */

char *generate_answer(stralloc *answer, uint32 uid, 
		      char *lip, uint16 lport, char *rip, uint16 rport)
{
  char *problem = "ok";
  char *x;
  char buf[5];
  stralloc out = {0};
  stralloc tmp = {0};
  stralloc key = {0};

  /* get key from enviroment */
  x = env_get("KEY");
  if (!x)
    {
      problem = "$KEY not set";
      strerr_warn1("didentd warning: $KEY not set using 'snakeoilkey'", NULL);
      x = "snakeoilkey";
    }
  
  /* initialize rijndael with $KEY */
  stralloc_copys(&key, x);
  txtparse(&key);
  pad(&key, 32);
  rijndaelKeySched(6, 8, key.s);
    
  /* build answer */
  stralloc_cats(answer, " : USERID : OTHER : ");

  uint32_pack(buf, uid); stralloc_catb(&tmp, buf, 4);
  uint16_pack(buf, lport); stralloc_catb(&tmp, buf, 2);
  uint16_pack(buf, rport); stralloc_catb(&tmp, buf, 2);
  uint32_pack(buf, time(NULL)); stralloc_catb(&tmp, buf, 4);
  stralloc_catb(&tmp, lip, 4);
  stralloc_catb(&tmp, rip, 8);

  /* encrypt last part of answer with rijndael */
  rijndaelEncrypt(tmp.s);
  
  stralloc_readyplus(&out, 32);
  base64encode(out.s, tmp.s, 24);
  
  stralloc_catb(answer, out.s, 32); 
  stralloc_cats(answer, "\r\n");
  stralloc_0(answer);
  
  return problem;
}

