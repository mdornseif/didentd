/* $Id: didentd-decrypt.c,v 1.9 2001/10/11 11:28:26 drt Exp $
 *  --drt@un.bewaff.net
 *
 * decryptor for encrypted didentd replys 
 * 
 * You might find more info at http://c0re.jp/c0de/didentd/
 *
 * I do not belive there is something like copyright. 
 *
 */

#include <pwd.h>                 /* for getpwuid */
#include <time.h>

#include "buffer.h"
#include "byte.h"
#include "env.h"
#include "fmt.h"
#include "getln.h"
#include "ip4.h"
#include "ip6.h"
#include "stralloc.h"
#include "strerr.h"
#include "tai.h"
#include "uint16.h"
#include "uint32.h"

#include "base64.h"
#include "pad.h"
#include "rijndael.h"
#include "txtparse.h"

static char rcsid[] = "$Id: didentd-decrypt.c,v 1.9 2001/10/11 11:28:26 drt Exp $";

#define stderr 2
#define stdout 1
#define stdin 0
#define FATAL "didentd-decrypt: fatal: "

uint32 lip = 0;
uint32 rip = 0;
uint16 lport = 0;
uint16 rport = 0;

int main(int argc, char *argv[])
{
  int match = 1;
  char *x;
  char strnum[FMT_ULONG];
  char strip4[IP4_FMT];
  stralloc key = {0};
  stralloc out = {0};
  struct passwd *pw;
  stralloc line = {0};
  uint32 uid = 0;
  char lip[16] = {0};
  char rip[16] = {0};
  uint16 lport = 0;
  uint16 rport = 0;
  time_t time;

  /* get key from enviroment */
  x = env_get("KEY");
  if (!x)
      strerr_die2x(111, FATAL, "$KEY not set");

  stralloc_copys(&key, x);
  txtparse(&key);
  pad(&key, 32);

  /* initialize rijndael */
  rijndaelKeySched(6, 8, key.s);

  while(match)
    {
      if (getln(buffer_0, &line, &match, '\n') == -1)
	strerr_die2sys(111,FATAL,"unable to read input: ");

      stralloc_0(&line);

      if (line.len < 33)
	return 0;
 
      stralloc_ready(&out, 24);
      base64decode(out.s, line.s, 32);

      /* decrypt with rijndael */
      rijndaelDecrypt(out.s);
      
      uint32_unpack(out.s, &uid);
      uint16_unpack(&out.s[4], &lport);
      uint16_unpack(&out.s[6], &rport);
      uint32_unpack(&out.s[8], &time);
      byte_copy(lip, 4, &out.s[12]);
      byte_copy(rip, 8, &out.s[16]);
  
      stralloc_copys(&out, ctime(&time));
      out.len--; /* remove \n */
      stralloc_cats(&out, " ");

      buffer_put(buffer_1, out.s, out.len);
      
      if(uid == 0xffffffff)
	  buffer_puts(buffer_1, "unknown(-");
      else
	{
	  pw = getpwuid(uid); /* XXX: check for error */
	  if(pw) buffer_puts(buffer_1, pw->pw_name);
	  buffer_puts(buffer_1, "(");
	  buffer_put(buffer_1, strnum, fmt_ulong(strnum,uid));	  
	}
      buffer_puts(buffer_1, ") ");
      buffer_put(buffer_1, strip4, ip4_fmt(strip4, (char *) &lip));
      buffer_puts(buffer_1, ":");
      buffer_put(buffer_1, strnum, fmt_ulong(strnum,lport));
      buffer_puts(buffer_1, " ");
      buffer_put(buffer_1, strip4, ip4_fmt(strip4, (char *) &rip));
      buffer_puts(buffer_1, ":");
      buffer_put(buffer_1, strnum, fmt_ulong(strnum,rport));
      buffer_puts(buffer_1, &line.s[32]);
      buffer_flush(buffer_1);
    }
  
  return 0; 
}





