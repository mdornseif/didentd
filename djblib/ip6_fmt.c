#include "fmt.h"
#include "ip6.h"
#include <stdio.h>

unsigned int ip6_fmt(char *s,char ip[16])
{
  unsigned int len;
  unsigned int i;
 
  len = 0;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[0] << 8) +
		   (unsigned long) (unsigned char) ip[1]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[2] << 8) +
		   (unsigned long) (unsigned char) ip[3]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[4] << 8) +
		   (unsigned long) (unsigned char) ip[5]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[6] << 8) +
		   (unsigned long) (unsigned char) ip[7]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[8] << 8) +
		   (unsigned long) (unsigned char) ip[9]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[10] << 8) +
		   (unsigned long) (unsigned char) ip[11]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[12] << 8) +
		   (unsigned long) (unsigned char) ip[13]); len += i; if (s) s += i;
  if (s) *s++ = ':'; ++len;
  i = fmt_xlong(s,((unsigned long) (unsigned char) ip[14] << 8) +
		   (unsigned long) (unsigned char) ip[15]); len += i; if (s) s += i;
  if (s) *s=0;
  return len;
}

static char tohex(char num) {
  if (num<10)
    return num+'0';
  else if (num<16)
    return num-10+'a';
  else
    return -1;
}

unsigned int ip6_fmt_flat(char *s,char ip[16])
{
  int i;
  for (i=0; i<16; i++) {
    *s++=tohex((unsigned char)ip[i] >> 4);
    *s++=tohex((unsigned char)ip[i] & 15);
  }
  return 32;
}
