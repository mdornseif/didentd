/* base64-encode.c */
int base64encode(char *dest, char *src, int l);
/* will encode l bytes from src acording to RfC1341 and write them to dest
 * dest must have space for 1/3 more bytes than l and padding to the next 
 * multiple of 3.
 * To be save you can malloc ((l/3)*4)+3 bytes.
 */
