/* $Id: get_info4_linux.c,v 1.2 2001/10/08 12:51:21 drt Exp $
 *  --drt@un.bewaff.net
 *
 * get info on an ipv4 connection on linux
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: get_info4_linux.c,v $
 * Revision 1.2  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.1  2000/05/08 14:26:04  drt
 * IPv6 support, first try
 *
 */

#include <unistd.h>              /* for close */
#include <pwd.h>                 /* for getpwuid */

#include "buffer.h"
#include "byte.h"
#include "env.h"
#include "fmt.h"
#include "getln.h"
#include "ip4.h"
#include "open.h"
#include "prot.h"
#include "scan.h"
#include "str.h"
#include "stralloc.h"
#include "strerr.h"
#include "timeoutread.h"
#include "uint16.h"
#include "uint32.h"

static char rcsid[] = "$Id: get_info4_linux.c,v 1.2 2001/10/08 12:51:21 drt Exp $";

#define NETINFOFILE4 "tcp"
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

   This is linux specific bound to the proc-filesystem. If somebody
   provides me with Documentation on how to to get connection
   Information on other Unices please contact me.
 */

uint32 get_connection_info4(char *lip, uint16 lport, char *rip, uint16 rport)
{
  int match = 1;
  int fd = 0;
  uint32 uid = 0xffffffff;   // that's our default for "not found"
  uint32 kernlport, kernrport;
  char  kernlip[4];
  char  kernrip[4];
  char bspace[1024];
  buffer b;
  stralloc line = {0};
  int i;
  
  fd = open_read(NETINFOFILE4);
  if (fd == -1)
      /* If opening failed quit */
      strerr_die3sys(111, FATAL, NETINFOFILE4, ": ");
  
  buffer_init(&b, read, fd, bspace, sizeof bspace);
  
  while (match) 
    {
      if (getln(&b, &line, &match, '\n') == -1)
	strerr_die2sys(111, FATAL, "unable to read line: ");
      
      /* example line:
	 sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode       
	 0: B09C9B3E:0AF1 B5B2C183:0015 08 00000000:00000001 00:00000000 00000000  1000        0 122140        
	 1: 00000000:0071 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 120494   

          5 7        14   21       30                                                 81         92
      */
      
      if((line.s[4] == ':') 
	 && (line.len > 80))
	{
	  if((line.s[14] !=  ':') 
	     || (line.s[19] !=  ' ') 
	     || (line.s[28] !=  ':') 
	     || (line.s[76] !=  ' '))
	    strerr_die3x(111, FATAL, "can't parse file ", NETINFOFILE4);
	  
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
	      scan_ulong(&line.s[77], &uid); 
	      break;
	    }
	}
    }
  close(fd);
  
  return uid;
}
