/* 
     from djblib 0.16                                                                                    
     
     20000331                                                                                            
     Copyright 2000                                                                                      
     SuperScript Technology, Inc.  
     
     $Log: scan_xlong.c,v $
     Revision 1.1  2000/04/12 16:07:11  drt
     Initial revision

     
     $Id: scan_xlong.c,v 1.1 2000/04/12 16:07:11 drt Exp $
*/

#include "djb/scan.h"

unsigned int scan_xlong(char *s,unsigned long *u)
{
  unsigned int pos; unsigned long result; unsigned long c;
  pos = 0; result = 0;
  while (((c = (unsigned long) (unsigned char) (s[pos] - '0')) < 10)
         ||(((c = (unsigned long) (unsigned char) (s[pos] - 'a')) < 6)
	    &&(c = c + 10))
	 ||(((c = (unsigned long) (unsigned char) (s[pos] - 'A')) < 6)
	    &&(c = c + 10))
	 ) /* XXX: this gets the job done */
    { result = result * 16 + c; ++pos; }
  *u = result; return pos;
}
