#ifndef IP6_H
#define IP6_H

extern unsigned int ip6_scan(char *,char *);
extern unsigned int ip6_fmt(char *,char *);

extern unsigned int ip6_scan_flat(char *,char *);
extern unsigned int ip6_fmt_flat(char *,char *);

/*
 ip6 address syntax: (h = hex digit), no leading '0' required
   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
   2. any number of 0000 may be abbreviated as "::", but only once
 flat ip6 address syntax:
   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
 */

#define IP6_FMT 40

#endif
