/* from DJBs libtai
 * 
 * drt@ailis.de
 *
 * $Id: leapsecs.h,v 1.1 2000/04/12 16:07:18 drt Exp $
 * 
 * $Log
 */

#ifndef LEAPSECS_H
#define LEAPSECS_H

extern int leapsecs_init();
extern int leapsecs_read();

extern void leapsecs_add();
extern int leapsecs_sub();

#endif
