/* dident-decrypt.c by drt@ailis.de
 * - decryptor for encrypted didentd replys 
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd-decrypt.c,v $
 * Revision 1.1  2000/04/12 16:07:17  drt
 * Initial revision
 *
 */

#include <pwd.h>                 /* for getpwuid */
#include "djb/buffer.h"
#include "djb/fmt.h"
#include "djb/getln.h"
#include "djb/ip4.h"
#include "djb/stralloc.h"
#include "djb/strerr.h"
#include "djb/tai.h"
#include "djb/uint16.h"
#include "djb/uint32.h"
#include "caltime.h"
#include "leapsecs.h"
#include "rijndael.h"

static char *rcsid = "$Id: didentd-decrypt.c,v 1.1 2000/04/12 16:07:17 drt Exp $";

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
  char strnum[FMT_ULONG];
  char strip4[IP4_FMT];
  char x[4];
  char *key = KEY;
  int i;
  stralloc out = {0};
  struct tai t;
  struct passwd *pw;
  stralloc line = {0};
  uint32 uid = 0;
  uint32 lip = 0;
  uint32 rip = 0;
  uint16 lport = 0;
  uint16 rport = 0;
  struct caltime ct;

  if (leapsecs_init() == -1) 
    {
      strerr_die2x(111, FATAL, "unable to init leapsecs");
    }

  /* initialize rijndael */
  rijndaelKeySched(6, 4, key);

  while(match)
    {
      if (getln(buffer_0, &line, &match, '\n') == -1)
	strerr_die2sys(111,FATAL,"unable to read input: ");

      stralloc_0(&line);

      if (line.len < 34)
	return 0;
      
      for(i = 0; i < 32; i += 4)
	{
	  x[0] = line.s[i] - 60;
	  x[0] |= ((line.s[i+3]-60) & 0x30) << 2;
	  x[1] = line.s[i+1] - 60;
	  x[1] |= ((line.s[i+3]-60) & 0x0c) << 4;
	  x[2] = line.s[i+2] - 60;
	  x[2] |= ((line.s[i+3]-60) & 0x03) << 6;
	  stralloc_catb(&out, x, 3);
	}
            
      /* decrypt with rijndael */
      rijndaelDecrypt(out.s);
      
      uint32_unpack(out.s, &uid);
      uint32_unpack(&out.s[4], &lip);
      uint16_unpack(&out.s[8], &lport);
      uint32_unpack(&out.s[10], &rip);
      uint16_unpack(&out.s[14], &rport);
      tai_unpack(&out.s[16], &t);
  
      caltime_utc(&ct, &t,(int *) 0, (int *) 0);
      stralloc_ready(&out, 256);
      out.len = caltime_fmt(out.s, &ct);
      stralloc_cats(&out, " ");
      stralloc_0(&out);

      buffer_put(buffer_1, out.s, out.len);
      
      if(uid == 0xffffffff)
	{
	  buffer_puts(buffer_1, "unknown(-");
	}
      else
	{
	  pw = getpwuid(uid);
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




