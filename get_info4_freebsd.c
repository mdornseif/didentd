/* $Id: get_info4_freebsd.c,v 1.3 2001/10/14 05:57:15 drt Exp $
 *  --drt@un.bewaff.net - http://c0re.jp/c0de/didentd/
 *
 * get info on an ipv4 connection on FreeBSD >= 3.x
 * 
 * You might find more info at http://c0re.jp/c0de/didentd/
 *
 */

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/proc.h> 

#include "byte.h"
#include "strerr.h"
#include "uint16.h"
#include "uint32.h"

static char rcsid[] = "$Id: get_info4_freebsd.c,v 1.3 2001/10/14 05:57:15 drt Exp $";

#define WARN "didentd: warning: "


/* returns the uid asociated with the IPv4 connection defined by lip,
   rip, lport, rport or 0xffffffff if unsucessfull. lip and rip are
   expected to be in network byteorder, while lport and rport are
   expected to be in host byteorder.

   FreeBSD >= 3.x supports the sysctrl 'net.inet.tcp.getcred' as an
   quite elegant way to get information about an TCP connection. 
 */

uint32 get_connection_info4(char *lip, uint16 lport, char *rip, uint16 rport)
{
  struct ucred uc; 
  struct sockaddr_in sin4[2]; 
  size_t size; 
  
  /* Using an array of two sockaddr_ins we look up the credentials for
   * the connection (which are returned by the sysctl
   * "net.inet.tcp.getcred" when we call it.)  This is mostly taken
   * from the internal ident support of FreeBSD's inetd.
   */
  
  byte_zero((char *) sin4, sizeof(sin4));
  sin4[1].sin_family = AF_INET;
  sin4[0].sin_family = AF_INET;
  uint16_pack_big((void *) &sin4[0].sin_port, lport);
  uint16_pack_big((void *) &sin4[1].sin_port, rport);
  /* TODO: check if we mixup host and network byte order */
  byte_copy((char *)&sin4[0].sin_addr, 4, lip);
  byte_copy((char *)&sin4[1].sin_addr, 4, rip);
  size = sizeof(uc); 
  if (sysctlbyname("net.inet.tcp.getcred", &uc, &size, sin4, sizeof(sin4)) == -1) 
    {
      strerr_warn2(WARN, "unable to sysctl 'net.inet.tcp.getcred': ", &strerr_sys);
      return 0xffffffff;
    }

  return (uint32) uc.cr_uid;
}
