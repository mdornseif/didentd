/* rijndael.h 
 *
 * drt@un.bewaff.net
 *
 * $id$ 
 *
 * $Log: rijndael.h,v $
 * Revision 1.2  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.1.1.1  2000/04/12 16:07:17  drt
 * initial revision
 *
 */

/* Key Scheduler. Create expanded encryption key */
/* blocksize=32*nb bits. Key=32*nk bits */
/* currently nb,bk = 4, 6 or 8          */
/* key comes as 4*Nk bytes              */
void rijndaelKeySched(int nb, int nk, char *key);

void rijndaelEncrypt(char *buff);
void rijndaelDecrypt(char *buff);
