/* dident.c by drt@ailis.de
 * - core for an ident server 
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd.c,v $
 * Revision 1.1  2000/04/12 16:07:11  drt
 * Initial revision
 *
 */

#include <unistd.h>              /* for close */
#include <pwd.h>                 /* for getpwuid */

#include "djb/buffer.h"
#include "djb/byte.h"
#include "djb/env.h"
#include "djb/fmt.h"
#include "djb/getln.h"
#include "djb/ip4.h"
#include "djb/open.h"
#include "djb/prot.h"
#include "djb/scan.h"
#include "djb/stralloc.h"
#include "djb/strerr.h"
#include "djb/timeoutread.h"
#include "djb/uint16.h"
#include "djb/uint32.h"

static char *rcsid = "$Id: didentd.c,v 1.1 2000/04/12 16:07:11 drt Exp $";

/* returns a pointer to a string describing a problem or "ok" if
sucessfull, adds to stralloc *answer the part after the ports of an
RfC 1413 reply */
extern char *generate_answer(stralloc *answer, unsigned long uid);

// where to read the networkinfo
#define NETINFOFILE "tcp"
#define CHROOT "/proc/net/"

#define stderr 2
#define stdout 1
#define stdin 0
#define FATAL "didentd: fatal: "

uint32 lip = 0;
uint32 rip = 0;
uint16 lport = 0;
uint16 rport = 0;


/* returns the uid asociated with the connection defined by the
   globals lip, rip, lport, rport or 0xffffffff if unsucessfull 

   This is linux specific bound to the proc-filesystem
 */

unsigned get_connection_info()
{
  int match = 1;
  int fd = 0;
  unsigned long uid = 0xffffffff;   // that's our default for "not found"
  unsigned long kernlip, kernlport, kernrip, kernrport;
  char bspace[1024];
  buffer b;
  stralloc line = {0};
  
  fd = open_read(NETINFOFILE);
  if (fd == -1)
    {
      /* If opening failed quit */
      strerr_die3sys(111, FATAL, NETINFOFILE, ": ");
    }      
  
  buffer_init(&b, read, fd, bspace, sizeof bspace);
  
  while (match) 
    {
      if (getln(&b, &line, &match, '\n') == -1)
	strerr_die2sys(111, FATAL, "unable to read line: ");
      
      /* example line:
	sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode       
	0: B09C9B3E:0AF1 B5B2C183:0015 08 00000000:00000001 00:00000000 00000000  1000        0 122140        
	1: 00000000:0071 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 120494   
      */
		      
      if(line.s[4] == ':')
	{
	  if((line.s[14] !=  ':') ||
	     (line.s[19] !=  ' ') ||
	     (line.s[28] !=  ':') ||
	     (line.s[76] !=  ' '))
	    {
	      strerr_die3x(111, FATAL, "can't parse file ", NETINFOFILE);
	    }
	  
	  scan_xlong(&line.s[6], &kernlip); 
	  scan_xlong(&line.s[15], &kernlport); 
	  scan_xlong(&line.s[20], &kernrip); 
	  scan_xlong(&line.s[29], &kernrport); 
	}
      
      if((kernrport == rport) &&
	 (kernlport == lport) &&
	 (kernrip == rip) &&
	 (kernlip == lip))
	{
	  scan_ulong(&line.s[77], &uid); 
	  break;
	}
    }
  close(fd);

  return uid;
}


/* this is based on DJBs droproot */

void droppriv()
{
  char *x;
  unsigned long id;

  if (chdir(CHROOT) == -1)
    strerr_die4sys(111, FATAL, "unable to chdir to ", CHROOT, ": ");
  if (chroot(".") == -1)
    strerr_die4sys(111, FATAL, "unable to chroot to ", CHROOT, ": ");

  x = env_get("GID");
  if (!x)
    strerr_die2x(111, FATAL, "$GID not set");
  scan_ulong(x,&id);
  if (prot_gid((int) id) == -1)
    strerr_die2sys(111, FATAL, "unable to setgid: ");

  x = env_get("UID");
  if (!x)
    strerr_die2x(111, FATAL, "$UID not set");
  scan_ulong(x,&id);
  if (prot_uid((int) id) == -1)
    strerr_die2sys(111, FATAL, "unable to setuid: ");
}


int main()
{
  unsigned char *remotehost, *remoteip, *remoteport, *localip, *localport;
  unsigned char query[256] = {0};
  uint32 uid = 0xffffffff;          // that's our default for "not found"
  int query_len = 0;
  int pos = 0;
  char *problem = "ok";
  stralloc answer = {0};
  char ip[4];
  char strnum[FMT_ULONG];

  /* chroot() to ROOT and switch to $UID:$GID */
  droppriv();

  /* since we run under tcpserver, we can get all info 
     about the remote side from the enviroment */
  remotehost = env_get("TCPREMOTEHOST");
  if (!remotehost) remotehost = "unknown";

  remoteip = env_get("TCPREMOTEIP");
  if (remoteip && ip4_scan(remoteip, ip)) ; else byte_zero(ip,4);
  /* Is there a way to do this a nicer way */
  rip = ((uint32)ip[0]<<24) | ((uint32)ip[1]<<16) | ((uint32)ip[2]<<8) | (uint32)ip[3];
  if (!remoteip) remoteip = "0.0.0.0";

  remoteport = env_get("TCPREMOTEPORT");  
  if (remoteport) scan_ushort(remoteport, &rport); else remoteport = "0";

  localip = env_get("TCPLOCALIP");
  if (localip && ip4_scan(localip, ip)) ; else byte_zero(ip,4);
  /* Is there a way to do this a nicer way */
  lip = ((uint32)ip[0]<<24) | ((uint32)ip[1]<<16) | ((uint32)ip[2]<<8) | (uint32)ip[3];
  if (!localip) localip = "0.0.0.0";

  localport = env_get("TCPLOCALPORT");
  if (localport) scan_ushort(localport, &lport); else localport = "0";

  /* now: 
     remotehost  is the remote hostname or "unknown" 
     remoteip    is the remote ipadress or "0.0.0.0" 
     remoteport  is the port on the remote machine or "0"
     localip     is the local ipadress or "0.0.0.0"
     localport   is the port on the remote machine or "0"
     rip:rport are ulongs representing remoteip:remoteport
     lip:lport are ulongs representing localip:localport
  */

  /* Read the request from the client and \0-terminate it */
  /* timeout after 60 seconds */
  query_len = timeoutread(60, stdin, query, sizeof(query) - 1);
  query[query_len] = '\0';
     
  /* If there was any data we can go on */
  problem = "empty query";
  if (query_len > 0)
    {
      problem = "illegal query";
      pos = scan_ushort(query, &lport);
      if(query[pos++] == ' ')
	{
	  if(query[pos++] == ',')
	    {
	      if(query[pos++] == ' ')
		{
		  pos = scan_ushort(&query[pos], &rport);
		  problem = "ok";
		  
		  uid = get_connection_info();
		  
		  stralloc_copyb(&answer, strnum, fmt_ulong(strnum,lport));
		  stralloc_cats(&answer, " , ");
		  stralloc_catb(&answer, strnum, fmt_ulong(strnum,rport));
		  
		  problem = generate_answer(&answer, uid);
		  
		  buffer_puts(buffer_1, answer.s);
		  buffer_flush(buffer_1);
		}
	    }
	}
    }

  /* Do logging */
  buffer_puts(buffer_2, "[");
  buffer_puts(buffer_2, remoteip);  
  buffer_puts(buffer_2, ":");
  buffer_puts(buffer_2, remoteport);
  buffer_puts(buffer_2, "] [");
  buffer_puts(buffer_2, problem);
  buffer_puts(buffer_2, "] ");
  if(uid < 0xffffffff)
    {
      buffer_put(buffer_2, strnum, fmt_ulong(strnum,uid));
    }
  else
    {
       buffer_puts(buffer_2, "unknown");
    }   
  buffer_puts(buffer_2, " ");
  buffer_put(buffer_2, strnum, fmt_ulong(strnum,lport));
  buffer_puts(buffer_2, ", ");
  buffer_put(buffer_2, strnum, fmt_ulong(strnum,rport));
  buffer_puts(buffer_2, "\n");
  buffer_flush(buffer_2);

  return 0;
}







