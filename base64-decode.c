/* $Id: base64-decode.c,v 1.4 2000/04/25 23:45:12 drt Exp $
 * 
 * Found somewere on the internet stating:
 */

/* public domain 
 * BASE64 on stdin -> converted data on stdout */

/* Hacked by drt@ailis.de to be a library function working on memory blocks
 *
 * $Log: base64-decode.c,v $
 * Revision 1.4  2000/04/25 23:45:12  drt
 * signed/unsigned error only popping up on ix86, not PPC
 *
 * Revision 1.3  2000/04/25 22:32:22  drt
 * Code Cleanups
 *
 * Revision 1.2  2000/04/20 15:05:07  drt
 * Changed for use with didentd
 *
 */ 

static char rcsid[] = "$Id: base64-decode.c,v 1.4 2000/04/25 23:45:12 drt Exp $";

unsigned char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64decode(unsigned char *dest, unsigned char *src, int l)
{
    static char inalphabet[256], decoder[256];
    int i, bits, c, char_count;
    int rpos;
    int wpos = 0;

    /* Build decodingtable */
    for (i = (sizeof alphabet) - 1; i >= 0 ; i--) 
      {
	inalphabet[alphabet[i]] = 1;
	decoder[alphabet[i]] = i;
      }

    char_count = 0;
    bits = 0;
    for(rpos = 0; rpos < l; rpos++)
      {
	c = src[rpos];

	if (c == '=')
	  {
	    break;
	  }

	if (c > 255 || ! inalphabet[c])
	  {
	    /* woa shouldn't we barf here */
	    continue;
	  }

	bits += decoder[c];
	char_count++;
	if (char_count < 4) 
	  {
	    bits <<= 6;
	  }	 
	else
	  {
	    dest[wpos++] = bits >> 16;
	    dest[wpos++] = (bits >> 8) & 0xff;
	    dest[wpos++] = bits & 0xff;
	    bits = 0;
	    char_count = 0;
	  }  
      }
      
    /* c == '=' */
    switch (char_count) 
      {
      case 1:
	return -1;
	break;
      case 2:
	dest[wpos++] = bits >> 10;
	break;
      case 3:
	dest[wpos++] = bits >> 16;
	dest[wpos++] = (bits >> 8) & 0xff;
	break;
      }
    
    return wpos;
}
