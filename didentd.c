/* $Id: didentd.c,v 1.9 2001/10/15 00:27:22 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * core for an ident server 
 */

#include <unistd.h>              /* for close */
#include <pwd.h>                 /* for getpwuid */
#include <stdlib.h>              /* for random() */
#include <time.h>                /* time */

#include "buffer.h"
#include "env.h"
#include "fmt.h"
#include "getln.h"
#include "ip4.h"
#include "ip6.h"
#include "scan.h"
#include "str.h"
#include "stralloc.h"
#include "strerr.h"
#include "timeoutread.h"
#include "uint16.h"
#include "uint32.h"

static char rcsid[] = "$Id: didentd.c,v 1.9 2001/10/15 00:27:22 drt Exp $";

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

/* server specific initialisation */
extern void didentd_init();

#define stderr  2
#define stdout  1
#define stdin   0
#define FATAL "didentd: fatal: "

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

  didentd_init();

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

  srandom(((long long) getpid () *
	    (long long) time(0) *
	    (long long) getppid() * 
	    (long long) random() * 
	    (long long) clock()) % 0xffffffff);

  /* since we run under tcpserver, we can get all info 
     about the remote side from the enviroment */
  remotehost = env_get("TCPREMOTEHOST");
  if (!remotehost) remotehost = "unknown";
  remoteport = env_get("TCPREMOTEPORT");  
  if (remoteport) scan_ushort(remoteport, &rport); else remoteport = "0";
  localport = env_get("TCPLOCALPORT");
  if (localport) scan_ushort(localport, &lport); else localport = "0";

  remoteip = env_get("TCPREMOTEIP");
  if (!remoteip) remoteip = "0.0.0.0";
  localip = env_get("TCPLOCALIP");
  if (!localip) localip = "0.0.0.0";
  
  if(proto == 4)
    {
      if(remoteip) 
	ip4_scan(remoteip, rip);

      if(localip) 
	ip4_scan(localip, lip);
      
      /* seed some entropy into the by IPv4 unsused bytes */
      rip[5] = (char) random() & 0xff;
      rip[6] = (char) random() & 0xff;
    }

  if(proto == 6)
    {
      if (remoteip) 
	ip6_scan(remoteip, rip);

      if (localip) 
	ip6_scan(localip, lip);
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
	      
	      if(uid != 0xffffffff)
		problem = generate_answer(&answer, uid, lip, lport, rip, rport);
	      else
		generate_answer(&answer, uid, lip, lport, rip, rport);
	      
	      buffer_puts(buffer_1, answer.s);
	      buffer_flush(buffer_1);
	    }
    }

  /* Do logging */
  buffer_puts(buffer_2, localip);  
  buffer_puts(buffer_2, ":");
  buffer_put(buffer_2, strnum, fmt_ulong(strnum, lport));
  buffer_puts(buffer_2, " -> ");
  buffer_puts(buffer_2, remoteip);  
  buffer_puts(buffer_2, ":");
  buffer_put(buffer_2, strnum, fmt_ulong(strnum, rport));
  buffer_puts(buffer_2, " [");
  buffer_puts(buffer_2, problem);
  buffer_puts(buffer_2, "] ");
  if(uid < 0xffffffff)
      buffer_put(buffer_2, strnum, fmt_ulong(strnum,uid));
  else
       buffer_puts(buffer_2, "unknown");  
  buffer_puts(buffer_2, "\n");
  buffer_flush(buffer_2);

  return 0;
}







