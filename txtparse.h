/* $Id: txtparse.h,v 1.2 2001/10/08 12:51:22 drt Exp $
 *  --drt@un.bewaff.net
 * 
 * $Log: txtparse.h,v $
 * Revision 1.2  2001/10/08 12:51:22  drt
 * uodated emailaddress
 *
 * Revision 1.1  2000/04/30 02:01:58  drt
 * key is now taken from the enviroment
 *
 */

#include "stralloc.h"

/* change encoded octets (\012) to their 'real' values (\n) */

extern void txtparse(stralloc *sa);
