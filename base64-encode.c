/* $Id: base64-encode.c,v 1.4 2000/04/25 23:29:32 drt Exp $
 * 
 * Found somewere on the internet stating:
 */

/* arbitrary data on stdin -> BASE64 data on stdout
 * UNIX's newline convention is used, i.e. one ASCII control-j (10 decimal).
 * 
 * public domain 
 */

/* Hacked by drt@ailis.de to be a library function working on memory blocks
 *
 * $Log: base64-encode.c,v $
 * Revision 1.4  2000/04/25 23:29:32  drt
 * Error with signed/unsigned wich didn't show up on PPC but on ix86 fixed
 *
 * Revision 1.3  2000/04/25 22:32:22  drt
 * Code Cleanups
 *
 * Revision 1.2  2000/04/19 13:39:17  drt
 * Changes for didentd
 *
 */ 

static char rcsid[] = "$Id: base64-encode.c,v 1.4 2000/04/25 23:29:32 drt Exp $"; 

static unsigned char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64encode(unsigned char* dest, unsigned char *src, int l)
{
    int cols, bits, c, char_count;
    int rpos;
    int wpos = 0;

    char_count = 0;
    bits = 0;
    cols = 0;

    for(rpos = 0; rpos < l; rpos++)
      {
	c = src[rpos];

	bits += c;
	char_count++;
	if (char_count < 3) 
	  {
	    bits <<= 8;
	  }
	else
	  {
	    dest[wpos++] = alphabet[bits >> 18];
	    dest[wpos++] = alphabet[(bits >> 12) & 0x3f];
	    dest[wpos++] = alphabet[(bits >> 6) & 0x3f];
	    dest[wpos++] = alphabet[bits & 0x3f];
	    bits = 0;
	    char_count = 0;
	  } 
      }
    
    if (char_count != 0) 
      {
	bits <<= 16 - (8 * char_count);
	dest[wpos++] = alphabet[bits >> 18];
	dest[wpos++] = alphabet[(bits >> 12) & 0x3f];
	if (char_count == 1) 
	  {
	    dest[wpos++] = '=';
	    dest[wpos++] = '=';
	} 
	else
	  {
	    dest[wpos++] =alphabet[(bits >> 6) & 0x3f];
	    dest[wpos++] = '=';
	  }
      }
    return wpos;
}
