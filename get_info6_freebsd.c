/* $Id: get_info6_freebsd.c,v 1.1 2001/10/12 12:27:48 drt Exp $
 *  --drt@un.bewaff.net - http://c0re-jp/c0de/didentd/ 
 *
 * get info on an ipv6 connection on darwin and possible other 4.4bsd variants
 * 
 * You might find more info at http://c0re.jp/c0de/didentd/
 *
 * this code is based on code from pidentd and i had a look at apples
 * modifications on it for darwin.
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

static char rcsid[] = "$Id: get_info6_freebsd.c,v 1.1 2001/10/12 12:27:48 drt Exp $";

#define FATAL "didentd: fatal: "

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/proc.h>

/* returns the uid asociated with the IPv4 connection defined by 
   lip, rip, lport, rport or 0xffffffff if unsucessfull 

   This is darwin specific and might work on other bsds
 */

uint32 get_connection_info6(char *lip, uint16 lport, char *rip, uint16 rport)
{
  struct ucred uc; 
  struct sockaddr_in sin4[2]; 
  size_t size; 

  strerr_die2sys(111, FATAL, "ipc6 not supported on this architecture: ");
  
  /* Using an array of two sockaddr_ins we look up the credentials for
   * the connection (which are returned by the sysctl
   * "net.inet.tcp.getcred" when we call it.)  This is mostly taken
   * from the internal ident support of FreeBSD's inetd.
   */
  
  bzero((char *) sin4, sizeof(sin4));
  sin4[0].sin_family = AF_INET;
  sin4[1].sin_family = AF_INET;
  sin4[0].sin_port = htons(lport);
  sin4[1].sin_port = htons(rport);
  /* TODO: check if we mixup host and network byte order */
  bcopy(lip, (char *)&sin4[0].sin_addr, 4);
  bcopy(rip, (char *)&sin4[1].sin_addr, 4);
  size = sizeof(uc); 
  if (sysctlbyname("net.inet.tcp.getcred", &uc, &size, sin4, sizeof(sin4)) == -1) 
    strerr_die2sys(111, FATAL, "unable to sysctl 'net.inet.tcp.getcred': ");

  return (uint32) uc.cr_uid;
}
