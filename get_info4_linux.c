/* $Id: get_info4_linux.c,v 1.4 2001/10/11 15:25:28 drt Exp $
 *  --drt@un.bewaff.net 
 *
 * get info on an ipv4 connection on linux
 * 
 * You might find more info at http://c0re.jp/c0de/didentd/
 *
 * I do not belive there is something like copyright. 
 *
 * try
 * gcc get_info4_linux.c -DUNITTEST -g -Idjblib djblib.a scan_xlong.c -o get_info4_linux.test
 * ./get_info4_linux.test
 * for a simple test.
 *
 */

#include <unistd.h>              /* for close */
#include <pwd.h>                 /* for getpwuid */

#include "buffer.h"
#include "byte.h"
#include "getln.h"
#include "open.h"
#include "scan.h"
#include "stralloc.h"
#include "strerr.h"
#include "uint16.h"
#include "uint32.h"

static char rcsid[] = "$Id: get_info4_linux.c,v 1.4 2001/10/11 15:25:28 drt Exp $";

#ifdef UNITTEST
#define NETINFOFILE "test/tcp4-linux"
#else
#define NETINFOFILE "tcp"
#endif
#define FATAL "didentd: fatal: "

static long int fromhex(unsigned char c) 
{
  /* from  scan_0x.c */
  
  if (c>='0' && c<='9')
    return c-'0';
  else if (c>='A' && c<='F')
    return c-'A'+10;
  else if (c>='a' && c<='f')
    return c-'a'+10;
  return -1;
}


/* returns the uid asociated with the IPv4 connection defined by 
   lip, rip, lport, rport or 0xffffffff if unsucessfull 

   This is linux specific bound to the proc-filesystem.
 */

uint32 get_connection_info4(char *lip, uint16 lport, char *rip, uint16 rport)
{
  int match = 1;
  int fd = 0;
  uint32 uid = 0xffffffff;   // that's our default for "not found"
  uint32 kernlport, kernrport;
  char kernlip[4];
  char kernrip[4];
  char bspace[1024];
  char *c;
  buffer b;
  stralloc line = {0};
  int i;
  
  fd = open_read(NETINFOFILE);
  if (fd == -1)
      /* If opening failed quit */
      strerr_die3sys(111, FATAL, NETINFOFILE, ": ");
  
  buffer_init(&b, read, fd, bspace, sizeof bspace);
  
  while (match) 
    {
      if (getln(&b, &line, &match, '\n') == -1)
	strerr_die2sys(111, FATAL, "unable to read line: ");
      stralloc_0(&line);

      /* example line:
  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode                               
   0: 2471DDD5:0050 942402D4:280A 01 0000D8B8:00000000 01:00000031 00000000 65534        0 155250592     
   1: 2471DDD5:0019 D2566ACF:0AF1 06 00000000:00000000 04:F754DEC0 00000000     0        0 0           
   2: 2471DDD5:0050 942402D4:2808 01 0000D8B8:00000000 01:0000001A 00000000 65534        0 155250685 
  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode	 
   0: B09C9B3E:0AF1 B5B2C183:0015 08 00000000:00000001 00:00000000 00000000  1000        0 122140
   1: 00000000:0071 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 120494   
    4 6        15   20       29   34 36                                     76
       */

      if((line.s[4] == ':') 
	 && (line.len > 80))
	{
	  if((line.s[14] !=  ':') 
	     || (line.s[19] !=  ' ') 
	     || (line.s[28] !=  ':') 
	     || (line.s[75] !=  ' '))
	    strerr_die5x(111, FATAL, "can't parse file ", NETINFOFILE, ":", &line.s[76]);
	  
	  /* check for connections not beeing in state 1 (ESTABLISHED) */
	  if((line.s[34] != '0') || (line.s[35] != '1'))
	    continue;
	
	  scan_xlong(&line.s[15], &kernlport); 
	  scan_xlong(&line.s[29], &kernrport); 
	  
	  for(i = 0; i < 4; i++)
	    {
	      /* check this on big endian machines */
	      kernlip[3-i] = (unsigned char) ((fromhex(line.s[(i*2)+6]) << 4) | fromhex(line.s[(i*2)+7])); 
	      kernrip[3-i] = (unsigned char) ((fromhex(line.s[(i*2)+20]) << 4) | fromhex(line.s[(i*2)+21])); 
	    }
	  
	  if((kernrport == rport) 
	     && (kernlport == lport) 
	     && byte_equal(kernrip, 4, rip) 
	     && byte_equal(kernlip, 4, lip))
	    {
	      for(c = &line.s[77]; *c == ' '; c++)
		;
	      scan_ulong(c, &uid); 
	      break;
	    }
	}
    }
  close(fd);
  
  return uid;
}

#ifdef UNITTEST

#include <stdio.h>

int main()
{

  uint32 r1, r2, r3;

  r1 = get_connection_info4("\177\000\000\001", 0x1000, "\177\000\000\001", 0x2000);
  r2 = get_connection_info4("\177\000\000\001", 0x1000, "\177\000\000\002", 0x2000);
  r3 = get_connection_info4("\177\000\000\001", 0x2000, "\177\000\000\002", 0x1000);

  if((r1 != 1) || (r2 != 2) || (r3 != 3))
    {
      puts("error");
      return 1;
    }
  else
    {
      puts("OK");
      return 0;
    }
}

#endif
