/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: buffer_0.c,v 1.1 2000/04/12 16:07:17 drt Exp $
 * 
 * $Log
 */

#include "djb/readwrite.h"
#include "djb/buffer.h"

int buffer_0_read(fd,buf,len) int fd; char *buf; int len;
{
  if (buffer_flush(buffer_1) == -1) return -1;
  return read(fd,buf,len);
}

char buffer_0_space[BUFFER_INSIZE];
static buffer it = BUFFER_INIT(buffer_0_read,0,buffer_0_space,sizeof buffer_0_space);
buffer *buffer_0 = &it;
