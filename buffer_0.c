/* $Id: buffer_0.c,v 1.2 2000/04/28 12:54:55 drt Exp $
 * 
 * from DJB
 *
 * $Log: buffer_0.c,v $
 * Revision 1.2  2000/04/28 12:54:55  drt
 * Cleanup, better integration of libtai and dnscache
 *
 */

#include "buffer.h"
#include "readwrite.h"

static char rcsid[] = "$Id: buffer_0.c,v 1.2 2000/04/28 12:54:55 drt Exp $";

int buffer_0_read(fd,buf,len) int fd; char *buf; int len;
{
  if (buffer_flush(buffer_1) == -1) return -1;
  return read(fd,buf,len);
}

char buffer_0_space[BUFFER_INSIZE];
static buffer it = BUFFER_INIT(buffer_0_read,0,buffer_0_space,sizeof buffer_0_space);
buffer *buffer_0 = &it;
