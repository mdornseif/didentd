/* $Id: didentd.c,v 1.3 2000/05/08 14:26:04 drt Exp $
 *  --drt@ailis.de
 *
 * core for an ident server 
 * 
 * You might find more info at http://rc23.cx/
 *
 * I do not belive there is something like copyright. 
 *
 * $Log: didentd.c,v $
 * Revision 1.3  2000/05/08 14:26:04  drt
 * IPv6 support, first try
 *
 * Revision 1.2  2000/04/28 12:54:56  drt
 * Cleanup, better integration of libtai and dnscache
 *
 * Revision 1.1.1.1  2000/04/12 16:07:11  drt
 * initial revision
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

static char rcsid[] = "$Id: didentd.c,v 1.3 2000/05/08 14:26:04 drt Exp $";

/* returns a pointer to a string describing a problem or "ok" if
sucessfull, adds to stralloc *answer the part after the ports of an
RfC 1413 reply */
extern char *generate_answer(stralloc *answer, unsigned long uid,
			     char *lip, uint16 lport, char *rip, uint16 rport);

/* returns the uid asociated with the IPv4 connection defined by 
   lip, rip, lport, rport or 0xffffffff if unsucessfull */
uint32 get_connection_info4(char *lip, uint16 lport, char *rip, uint16 rport);

/* returns the uid asociated with the IPv6 connection defined by 
   lip, rip, lport, rport or 0xffffffff if unsucessfull */
uint32 get_connection_info6(char *lip, uint16 lport, char *rip, uint16 rport);

// where to read the networkinfo
#define NETINFOFILE6 "tcp6"
#define CHROOT "/proc/net/"

#define stderr  2
#define stdout  1
#define stdin   0
#define FATAL "didentd: fatal: "

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
  char strnum[FMT_ULONG];
  int  proto = 0;
  char *x;
  char lip[16] = {0};
  char rip[16] = {0};
  uint16 lport = 0;
  uint16 rport = 0;

  /* chroot() to ROOT and switch to $UID:$GID */
  droppriv();

  x = env_get("PROTO");
  if(x)
    {
      if(str_equal(x, "TCP")) proto = 4;
      if(str_equal(x, "TCP6")) proto = 6;
    }
  
  if(proto == 0)
    {
      buffer_puts(buffer_2, "warning: can't determine $PROTO\n");
      buffer_flush(buffer_2);
    }
  /* since we run under tcpserver, we can get all info 
     about the remote side from the enviroment */
  remotehost = env_get("TCPREMOTEHOST");
  if (!remotehost) remotehost = "unknown";
  remoteport = env_get("TCPREMOTEPORT");  
  if (remoteport) scan_ushort(remoteport, &rport); else remoteport = "0";
  localport = env_get("TCPLOCALPORT");
  if (localport) scan_ushort(localport, &lport); else localport = "0";

  if(proto == 4)
    {
      remoteip = env_get("TCPREMOTEIP");
      if (remoteip && ip4_scan(remoteip, rip)) ; else byte_zero(rip,4);
      if (!remoteip) remoteip = "0.0.0.0";
      
      localip = env_get("TCPLOCALIP");
      if (localip && ip4_scan(localip, lip)) ; else byte_zero(lip,4);
      /* Is there a way to do this a nicer way */
      if (!localip) localip = "0.0.0.0";
    }

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
	if(query[pos++] == ',')
	  if(query[pos++] == ' ')
	    {
	      pos = scan_ushort(&query[pos], &rport);
	      problem = "ok";
	      
	      if(proto == 4)
		uid = get_connection_info4(lip, lport, rip, rport);
	      if(proto == 6)
		uid = get_connection_info6(lip, lport, rip, rport);
	      
	      stralloc_copyb(&answer, strnum, fmt_ulong(strnum, lport));
	      stralloc_cats(&answer, " , ");
	      stralloc_catb(&answer, strnum, fmt_ulong(strnum, rport));
	      
	      problem = generate_answer(&answer, uid, lip, lport, rip, rport);
	      
	      buffer_puts(buffer_1, answer.s);
	      buffer_flush(buffer_1);
	    }
    }

  /* Do logging */
  buffer_puts(buffer_2, remoteip);  
  buffer_puts(buffer_2, ":");
  buffer_puts(buffer_2, remoteport);
  buffer_puts(buffer_2, " [");
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







